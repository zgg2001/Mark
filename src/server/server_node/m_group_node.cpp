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
        
        //处理
        if(ph->cmd == CMD_C2S_HEART)//心跳包
        {
            //send s2c_heart
            _tnode.addtask([this, sockfd]()
            {
                this->send_s2c_heart(sockfd);
            });
            
            //心跳重置
            client->reset_hb();
        }
        else if(ph->cmd == CMD_ADD_PLAN)//新增计划
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
        else if(ph->cmd == CMD_DEL_PLAN)//删除计划
        {
            del_plan* dp = (del_plan*)ph;
            int plan_id = dp->plan_id;
           
            //获取主键id
            int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id : -1; 
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
        else if(ph->cmd == CMD_UPD_PLAN_T)//更新计划 - time
        {
            upd_plan_t* upt = (upd_plan_t*)ph;
            int plan_id = upt->plan_id;
            
            //获取主键id
            int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id : -1; 
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
        else if(ph->cmd == CMD_UPD_PLAN_S)//更新计划 - status
        {
            upd_plan_s* ups = (upd_plan_s*)ph;
            int plan_id = ups->plan_id;
            
            //获取主键id
            int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id : -1; 
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
        else if(ph->cmd == CMD_UPD_PLAN_C)//更新计划 - content
        {
            upd_plan_c* upc = (upd_plan_c*)ph;
            int plan_id = upc->plan_id;
            
            //获取主键id
            int id = _group->plans.count(plan_id) != 0 ? _group->plans[plan_id].id : -1; 
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
        else
        {
            ERROR("login_node 收到非法包");
            return -1;
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


