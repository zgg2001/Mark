/*************************************************************************
	> File Name: m_recv_node.cpp
	> Author: ZHJ
	> Remarks: 客户端recv节点 监听客户端sockfd
	> Created Time: Fri 04 Feb 2022 09:40:01 AM CST
 ************************************************************************/

#include<client/client/m_client.h>
#include"m_recv_node.h"

m_recv_node::m_recv_node(m_client* client):
    _client(client),
    _sockfd(INVALID_SOCKET)
{
    //recv_buf初始化
    _recv_buf = new char[RECV_BUF_SIZE];
}

m_recv_node::~m_recv_node()
{
    DEBUG("recv_node ~ start");
    close_node();
    delete[] _recv_buf;
    DEBUG("recv_node ~ end");
}

void
m_recv_node::start()
{
    DEBUG("recv_node thread start");
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
m_recv_node::close_node()
{
    DEBUG("recv_node thread close");
    _thread.close();
}

void 
m_recv_node::func_create(m_thread* thread)
{
    DEBUG("recv_node func_create() start"); 
}

void 
m_recv_node::func_run(m_thread* thread)
{
    DEBUG("recv_node func_run() start");
    
    //无效套接字 
    if(_sockfd == INVALID_SOCKET)
    {
        ERROR("recv_node work faild -- socket not initialized");
        return;
    }

    //epoll
    int epollfd = epoll_create(1);//主线程epoll仅关注连接事件
    struct epoll_event ev;
    ev.data.fd = _sockfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, _sockfd, &ev);

    //开始工作
    while(_thread.is_run())
    {
        //epoll监听新连接
        struct epoll_event events;
        int event_count = epoll_wait(epollfd, &events, 1, 1);//阻塞 
        if(event_count < 0)
        {
            ERROR("recv_node epoll_wait() error -- %s", strerror(errno));
            break;
        }
        if(event_count > 0)
        {
            if(recvdata() == -1)
            {
                ERROR("recvdata error");
            }
        }
    }

    close(epollfd);
}

void 
m_recv_node::func_destory(m_thread* thread)
{
    DEBUG("recv_node func_destory() start");
}

int 
m_recv_node::recvdata()
{
    int buf_len = recv(_sockfd, _recv_buf, RECV_BUF_SIZE, 0);
    
    //退出
    if(buf_len <= 0)
    {
        return -1;
    }

    //操作
    header* ph = (header*)_recv_buf;
    if(buf_len < sizeof(header) || buf_len < ph->length)//防止半包
    {
        //这个部分回来还得用客户端二级缓冲 否则会丢包 稍后处理
        return 1;
    }

    //处理
    if(ph->cmd == CMD_LOGIN_RESULT)
    {
        login_result* plr = (login_result*)ph;
        //设置ret并唤醒
        _client->set_login_ret(plr->result);
        _client->m_login_wake(); 
    }
    else
    {
        WARN("login_node 收到非登录包");
    }
    
    
    return 0;
}
