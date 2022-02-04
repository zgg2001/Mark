/*************************************************************************
	> File Name: m_group_node.cpp
	> Author: ZHJ
	> Remarks: 服务器组节点 处理以组为单位的用户报文
	> Created Time: Thu 27 Jan 2022 07:49:01 PM CST
 ************************************************************************/

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
}

m_group_node::~m_group_node()
{
    DEBUG("group_node(%d) ~group_node() start", _group_id);
    close_node();
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
}

void
m_group_node::close_node()
{
    INFO("group_node(%d) thread close", _group_id);
    _thread.close();
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
            close_node();
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
    int buf_len = recv(sockfd, _recv_buf, RECV_BUF_SIZE, 0);

    //退出
    if(buf_len <= 0)
    {
        return -1;
    }

    //操作
    //
    return 0;    
}






