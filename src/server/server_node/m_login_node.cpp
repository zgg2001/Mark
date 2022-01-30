/*************************************************************************
	> File Name: m_login_node.cpp
	> Author: ZHJ
	> Remarks: 服务器登录节点 处理用户登录报文
	> Created Time: Thu 27 Jan 2022 07:45:02 PM CST
 ************************************************************************/

#include"m_login_node.h"

m_login_node::m_login_node(int id,  m_server* server):
    _login_id(id),
    _server(server)
{
    _recv_buf = new char[RECV_BUF_SIZE];
}

m_login_node::~m_login_node()
{
    DEBUG("login_node(%d) ~login_node() start", _login_id);
    close_node();
    delete[] _recv_buf;
    DEBUG("login_node(%d) ~login_node() end", _login_id);
}

void
m_login_node::start()
{
    INFO("login_node(%d) thread start", _login_id);
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
m_login_node::close_node()
{
    INFO("login_node(%d) thread close", _login_id);
    _thread.close();
}

void 
m_login_node::addclient(m_client_node* client)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _clientbuf.push_back(client);
}

void 
m_login_node::func_create(m_thread* thread)
{
    DEBUG("login_node func_create() start"); 
}

void 
m_login_node::func_run(m_thread* thread)
{
    DEBUG("login_node func_run() start");
    
    //登录节点epoll仅关注未登录用户的登录事件
    int epollfd = epoll_create(1);
    struct epoll_event ev;
    struct epoll_event events[EVENT_MAX_SIZE];
    int event_count;

    //计时器->heartbeat
    m_timer timer;
    timer.update();

    //开始工作
    while(_thread.is_run())
    {
        //缓冲区socket加入epoll监听队列
        if(!_clientbuf.empty())
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(m_client_node* client : _clientbuf)
            {
                //epoll监听
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
            ERROR("login_node(%d) epoll_wait() error -- %s",
                  _login_id,  strerror(errno));
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
                continue;
            }
            iter++;
        }
    }

    close(epollfd);
}

void 
m_login_node::func_destory(m_thread* thread)
{
    DEBUG("login_node func_destory() start");
}

int 
m_login_node::recvdata(SOCKET sockfd)
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







