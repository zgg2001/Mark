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
m_login_node::addclient(SOCKET sockfd)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _clientbuf.push_back(sockfd);
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

    //开始工作
    while(_thread.is_run())
    {
        //缓冲区socket加入epoll监听队列
        if(!_clientbuf.empty())
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(SOCKET client : _clientbuf)
            {
                memset(&ev, 0, sizeof(struct epoll_event));
                ev.data.fd = client;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &ev);
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
        if(event_count == 0)
        {
            //DEBUG("login_node(%d) epoll_wait() timeout", _login_id);
            continue;
        }
        else//处理事件
        {
            for(int i = 0; i < event_count; ++i)
            {
                if(recvdata(events[i].data.fd) == -1)
                {
                    INFO("login_node(%d) socket:%d quit", 
                         _login_id, events[i].data.fd);
                    memset(&ev, 0, sizeof(struct epoll_event));
                    ev.data.fd = events[i].data.fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                }
            }
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









