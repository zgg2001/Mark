/*************************************************************************
	> File Name: m_group_node.cpp
	> Author: ZHJ
	> Remarks: 服务器组节点 处理以组为单位的用户报文
	> Created Time: Thu 27 Jan 2022 07:49:01 PM CST
 ************************************************************************/

#include<server/server/m_server.h>
#include"m_group_node.h"

m_group_node::m_group_node(int g_id, const char* g_name, int a_id, m_server* server):
    _server(server),
    _group_id(g_id),
    _group_name_len(0),
    _admin_id(a_id),
    _group(nullptr),
    _client_num(0),
    _recv_buf(nullptr)
{
    //组名初始化
    memset(_group_name, 0, sizeof(_group_name));
    _group_name_len = snprintf(_group_name, GROUP_NAME_MAX_SIZE, "%s", g_name);
    
    //m_group初始化
    _group = new m_group(_group_id, _admin_id);
    
    //recv_buf初始化
    _recv_buf = new char[RECV_BUF_SIZE];

    _tnode.id = g_id;
}

m_group_node::~m_group_node()
{
    DEBUG("group_node(%d) ~group_node() start", _group_id);
    //关线程
    close_node();
    
    //释放客户端
    for(m_client_node* c : _clientbuf)
    {
        delete c;
    }
    for(auto& [s, c] : _clients)
    {
        delete c;
    }
    _clientbuf.clear();
    _clients.clear();
   
    delete _group;
    delete[] _recv_buf;
    DEBUG("group_node(%d) ~group_node() end", _group_id);
}

void 
m_group_node::addclient(m_client_node* client)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _clientbuf.push_back(client);
    
    //有客户端节点时需保证工作线程运行
    ++_client_num;
    if(_thread.is_run() == false)
    {
        this->start();
    }
}

void 
m_group_node::adduser(int u_id, const char* name, const char* email)
{
    bool ret;
    ret = _group->adduser(u_id, std::string{name}, std::string{email});
    if(ret == false)
    {
        ERROR("m_group_node adduser uid:%d name:%s error", u_id, name);
    }
}

void 
m_group_node::addplan(int p_id, int u_id, int status,
                      int plan_id, int user_id,
                      int create_t, int plan_t,
                      const char* content_str, const char* remark_str)
{
    int ret;
    ret = _group->addplan(p_id, u_id, status, plan_id, user_id, 
                          create_t, plan_t, content_str, remark_str);
    if(ret == -1)
    {
        ERROR("m_group_node addplan error -- 用户(%d)不存在", u_id);
    }
    else if(ret == -2)
    {
        ERROR("m_group_node addplan error -- 计划id(%d)重复", p_id);
    }
}

void
m_group_node::start()
{
    INFO("group_node(%d) thread start", _group_id);
    _thread.start(
        //create
        [this](m_thread*)
        {
            func_create(&_thread);
        },
        //run
        [this](m_thread*)
        {
            func_run(&_thread);
        },
        //destory
        [this](m_thread*)
        {
            func_destory(&_thread);
        }
    );
    //tnode
    _tnode.start();
}

void
m_group_node::close_node()
{
    INFO("group_node(%d) thread close", _group_id);
    _thread.close();
    _tnode.close_node();
}

void 
m_group_node::func_create(m_thread* thread)
{
    DEBUG("group_node(%d) func_create() start", _group_id); 
}

void 
m_group_node::func_run(m_thread* thread)
{
    DEBUG("group_node(%d) func_run() start", _group_id);
    //组节点epoll关注登录用户的请求事件
    int epollfd = epoll_create(1);
    struct epoll_event ev;
    struct epoll_event events[EVENT_MAX_SIZE];
    int event_count;

    //计时器->heartbeat
    m_timer timer;
    timer.update();

    while(_thread.is_run())
    {
        //缓冲区socket加入epoll监听队列
        if(!_clientbuf.empty())
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(m_client_node* client : _clientbuf)
            {
                //epoll监听队列
                memset(&ev, 0, sizeof(struct epoll_event));
                ev.data.fd = client->get_sockfd();
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client->get_sockfd(), &ev);
                //加入正式队列
                _clients[client->get_sockfd()] = client;
            }
            _clientbuf.clear();
        }
        
        //监听登录事件
        event_count = epoll_wait(epollfd, events, EVENT_MAX_SIZE, 1);
        if(event_count < 0)
        {
            ERROR("group_node(%d) epoll_wait() error -- %s",
                  _group_id,  strerror(errno));
            _thread.exit();
            break;
        }
        //存在则处理事件
        if(event_count > 0)
        {
            for(int i = 0; i < event_count; ++i)
            {
                if(recvdata(events[i].data.fd) == -1)
                { 
                    std::lock_guard<std::mutex> lock(_mutex);
                    //删除epoll事件监听
                    memset(&ev, 0, sizeof(struct epoll_event));
                    ev.data.fd = events[i].data.fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    
                    //释放客户端节点并移出队列
                    delete _clients[events[i].data.fd];
                    _clients.erase(events[i].data.fd);
                    
                    //更新计数
                    --_client_num;
                }
            }
        }
        
        //处理心跳检测
        double time_interval = timer.get_sec();
        timer.update();//更新时间
        for(auto iter = _clients.begin(); iter != _clients.end();)
        {
            //检测心跳是否超时
            if(iter->second->check_hb(time_interval) == true)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                //删除epoll事件监听
                memset(&ev, 0, sizeof(struct epoll_event));
                ev.data.fd = iter->first;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_DEL, iter->first, &ev);
                
                //移出队列
                delete iter->second;
                _clients.erase(iter++);//避免迭代器失效
                
                //更新计数
                --_client_num;
                continue;
            }
            iter++;
        }
        
        //当不存在监听客户端节点时 则关闭工作线程
        if(_client_num == 0)
        {
            INFO("group_node(%d) thread exit", _group_id);
            _thread.exit();
            _tnode.close_node();
        }
    }

    close(epollfd);
}

void 
m_group_node::func_destory(m_thread* thread)
{
    DEBUG("group_node(%d) func_destory() start", _group_id);
}

int 
m_group_node::recvdata(SOCKET sockfd)
{
    m_client_node* client = _clients[sockfd];
    int buf_len = recv(sockfd, _recv_buf, RECV_BUF_SIZE, 0);

    //退出
    if(buf_len <= 0)
    {
        return -1;
    }
    
    //内容转至二级缓冲
    memcpy(client->get_recvbuf() + client->get_recvlen(), _recv_buf, buf_len);
    client->set_recvlen(client->get_recvlen() + buf_len);
    
    //处理
    while(client->get_recvlen() >= (int)sizeof(header))
    {
        //不完整包
        header* ph = (header*)client->get_recvbuf();
        if(client->get_recvlen() < ph->length)
        {
            return 0;
        }
        //DEBUG("new data: cmd:%d length:%d", ph->cmd, ph->length);
        
        //处理
        switch(ph->cmd)
        {
            //心跳包
            case CMD_C2S_HEART:
            {
                //send s2c_heart
                _tnode.addtask([this, sockfd]()
                {
                    this->send_s2c_heart(sockfd);
                });
                
                //心跳重置
                client->reset_hb();
            }
            break;
            
            //新增计划
            case CMD_ADD_PLAN:
            {
                add_plan* ap = (add_plan*)ph;
                int plan_id = _group->get_plan_max_id() + 1;
                int user_id = client->get_uid();
                
                //数据库: add plan task
                int pk_id = _server->add_plan(_group_id, plan_id, user_id,
                                              ap->create_time, ap->plan_time,
                                              ap->content, ap->remark);
                //group_node: add plan
                _group->addplan(plan_id, user_id, 0,
                                pk_id, user_id,
                                ap->create_time, ap->plan_time,
                                ap->content, ap->remark);
                
                //add result ret
                _tnode.addtask([this, sockfd]()
                {
                    this->send_operate_result(sockfd, 0);
                });
            }
            break;
            
            //删除计划
            case CMD_DEL_PLAN:
            {
                del_plan* dp = (del_plan*)ph;
                int plan_id = dp->plan_id;
               
                //获取主键id
                int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id.load() : -1; 
                //group_node: del plan
                int ret = _group->delplan(client->get_uid(), plan_id);
                //数据库: del plan task
                if(id > 0 && ret > 0)
                    _server->del_plan(id);
                
                //del result ret
                _tnode.addtask([this, sockfd, ret]()
                {
                    this->send_operate_result(sockfd, ret);
                });
            }
            break;
            
            //更新计划 - time
            case CMD_UPD_PLAN_T:
            {
                upd_plan_t* upt = (upd_plan_t*)ph;
                int plan_id = upt->plan_id;
                
                //获取主键id
                int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id.load() : -1; 
                //group_node: upd plan
                int ret = _group->updplan_t(client->get_uid(), plan_id, upt->plan_time);
                //数据库: del plan task
                if(id > 0 && ret > 0)
                    _server->upd_plan_t(id, upt->plan_time);
                
                //upd result ret
                _tnode.addtask([this, sockfd, ret]()
                {
                    this->send_operate_result(sockfd, ret);
                });
            }
            break;
            
            //更新计划 - status
            case CMD_UPD_PLAN_S:
            {
                upd_plan_s* ups = (upd_plan_s*)ph;
                int plan_id = ups->plan_id;
                
                //获取主键id
                int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id.load() : -1; 
                //group_node: upd plan
                int ret = _group->updplan_s(client->get_uid(), plan_id, ups->status);
                //数据库: del plan task
                if(id > 0 && ret > 0)
                    _server->upd_plan_s(id, ups->status);
                
                //upd result ret
                _tnode.addtask([this, sockfd, ret]()
                {
                    this->send_operate_result(sockfd, ret);
                });
            }
            break;
            
            //更新计划 - content
            case CMD_UPD_PLAN_C:
            {
                upd_plan_c* upc = (upd_plan_c*)ph;
                int plan_id = upc->plan_id;
                
                //获取主键id
                int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id.load() : -1; 
                //group_node: upd plan
                int ret = _group->updplan_c(client->get_uid(), plan_id, upc->content, upc->remark);
                //数据库: del plan task
                if(id > 0 && ret > 0)
                    _server->upd_plan_c(id, upc->content, upc->remark);
                
                //upd result ret
                _tnode.addtask([this, sockfd, ret]()
                {
                    this->send_operate_result(sockfd, ret);
                });
            }
            break;
            
            //展示计划
            case CMD_SHOW_PLAN:
            {
                show_plan* sp = (show_plan*)ph;
                int plan_id = sp->plan_id;
                
                //交给send_node发包
                _tnode.addtask([this, sockfd, plan_id]()
                {
                    this->send_show_result(sockfd, plan_id);
                });
            }
            break;
            
            //展示计划 -- 个人
            case CMD_SHOW_PLAN_USER:
            {
                show_plan_u* spu = (show_plan_u*)ph;
                int mode = spu->mode;
                int uid = client->get_uid();
                
                //交给send_node迭代发包
                _tnode.addtask([this, sockfd, uid, mode]()
                {
                    this->send_show_result_u(sockfd, uid, mode);
                });
            }
            break;
            
            //展示计划 -- 组
            case CMD_SHOW_PLAN_GROUP:
            {
                show_plan_g* spg = (show_plan_g*)ph;
                int mode = spg->mode;
                
                //交给send_node迭代发包
                _tnode.addtask([this, sockfd, mode]()
                {
                    this->send_show_result_g(sockfd, mode);
                });
            }
            break;
            
            default:
            {
                ERROR("send_node 收到非法包");
                return -1;
            }
        }
        
        //消息前移
        int size = client->get_recvlen() - ph->length;//未处理size
        memcpy(client->get_recvbuf(), client->get_recvbuf() + ph->length, size);
        client->set_recvlen(size);
    } 
    return 0;    
}

void 
m_group_node::send_s2c_heart(SOCKET sockfd)
{
    s2c_heart h;
    send(sockfd, (const char*)&h, sizeof(h), 0);
}

void
m_group_node::send_operate_result(SOCKET sockfd, int ret)
{
    operate_result oret;
    oret.result = ret;
    send(sockfd, (const char*)&oret, sizeof(oret), 0);
}

void 
m_group_node::send_show_result(SOCKET sockfd, int plan_id)
{
    show_result sr;
    sr.plan_id = -1;
    if(_group->plans.count(plan_id) != 0)
    {
        m_plan& plan = _group->plans[plan_id];
        sr.plan_id = plan_id;
        sr.user_id = plan.now_user.load();
        snprintf(sr.create_name, 12, "%s", _group->users[sr.user_id].username.c_str());
        sr.create_id = plan.create_user.load();
        snprintf(sr.username, 12, "%s", _group->users[sr.create_id].username.c_str());
        sr.status = static_cast<int>(plan.status.load());
        sr.create_time = plan.create_time.load();
        sr.plan_time = plan.plan_time.load();
        snprintf(sr.content, 102, "%s", plan.get_content());
        snprintf(sr.remark, 102, "%s", plan.get_remark());
    }
    send(sockfd, (const char*)&sr, sizeof(sr), 0);
}

void 
m_group_node::send_show_result_u(SOCKET sockfd, int uid, int mode)
{
    //待发送的计划
    std::vector<std::list<int>*> lplans;
    
    //获取需发送的计划集
    for(int m : {0, 1, 2})
        if(mode == m || mode == 3)
            lplans.push_back(&(_group->users[uid].plans[m]));
    
    int now = 0;
    show_result_u sru;
    sru.sn = 1;
    sru.noap = now;

    //work
    for(auto& lnow : lplans)
    {
        for(auto iter = lnow->begin(); iter != lnow->end(); ++iter)
        {
            int plan_id = *iter;
            if(now == 10)
            {
                send(sockfd, (const char*)&sru, sizeof(sru), 0);
                now = 0;
            }
            m_plan& plan = _group->plans[plan_id];
            sru.plans[now].plan_id = plan_id;
            sru.plans[now].status = static_cast<int>(plan.status.load());
            sru.plans[now].create_time = plan.create_time.load();
            sru.plans[now].plan_time = plan.plan_time.load();
            snprintf(sru.plans[now].content, 102, "%s", plan.get_content());
            sru.noap = now++;
        }
    }
    sru.sn = 0;
    send(sockfd, (const char*)&sru, sizeof(sru), 0);
}

void 
m_group_node::send_show_result_g(SOCKET sockfd, int mode)
{
    //待发送的计划
    std::vector< std::pair<m_user&, std::list<int>*> > lplans;
    
    //获取需发送的计划集
    for(int m : {0, 1, 2})
        if(mode == m || mode == 3)
            for(auto& [uid, u] : _group->users)
                lplans.push_back({ u, &(_group->users[uid].plans[m]) }); 
    
    int now = 0;
    show_result_g srg;
    srg.sn = 1;
    srg.noap = now;

    //work
    for(auto& [u, lnow] : lplans)
    {
        for(auto iter = lnow->begin(); iter != lnow->end(); ++iter)
        {
            int plan_id = *iter;
            if(now == 10)
            {
                send(sockfd, (const char*)&srg, sizeof(srg), 0);
                now = 0;
            }
            m_plan& plan = _group->plans[plan_id];
            srg.plans[now].plan_id = plan_id;
            srg.plans[now].user_id = u.user_id;
            snprintf(srg.plans[now].username, 12, "%s", u.username.c_str());
            srg.plans[now].status = static_cast<int>(plan.status.load());
            srg.plans[now].create_time = plan.create_time.load();
            srg.plans[now].plan_time = plan.plan_time.load();
            snprintf(srg.plans[now].content, 102, "%s", plan.get_content());
            srg.noap = now++;
        }
    }
    srg.sn = 0;
    send(sockfd, (const char*)&srg, sizeof(srg), 0);
}



