/*************************************************************************
	> File Name: m_server.cpp
	> Author: ZHJ
	> Remarks: 服务端类 最外层
	> Created Time: Thu 27 Jan 2022 07:06:58 PM CST
 ************************************************************************/

#include"m_server.h"

m_server::m_server():
    _sock(INVALID_SOCKET)
{

}

m_server::~m_server()
{
    m_close();
}

int
m_server::m_init()
{
    //创建socket 
    if(_sock != INVALID_SOCKET)//存在socket
    {
        WARN("Server init warn -- socket:%d already exists", _sock);
        m_close();//m_server复位 
    }
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == INVALID_SOCKET)
    {
        ERROR("Server init faild -- %s", strerror(errno));
        return -1; 
    } 

    //数据库读取
    //
    
    INFO("Server init(%d) success", _sock);
    return 0;
}

int
m_server::m_bind(const char* ip, unsigned short port)
{
    //无效套接字
    if(_sock == INVALID_SOCKET)
    {
        WARN("Server bind warn -- server socket not initialized");
        m_init();
    }
    
    //bind
    sockaddr_in _myaddr = {};
    _myaddr.sin_family = AF_INET;//IPV4
    _myaddr.sin_port = htons(port);
    if(ip == nullptr) 
        _myaddr.sin_addr.s_addr = htonl(INADDR_ANY);//监听ALL
    else
        inet_pton(AF_INET, ip, &_myaddr.sin_addr.s_addr);
    if(bind(_sock, (sockaddr*)&_myaddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        ERROR("Server bind(%hd) faild -- %s", port, strerror(errno));
        return -1;
    }
    else
    {
        INFO("Server bind(%hd) success", port);
        return 0;
    }
}

int
m_server::m_listen(int n)
{
    //无效套接字 
    if(_sock == INVALID_SOCKET)
    {
        ERROR("Server listen(%d) faild -- server socket not initialized", n);
        return -1;
    }
    
    //开始监听
    if(listen(_sock, n) == SOCKET_ERROR) 
    {
        ERROR("Server listen(%d) faild -- %s", n, strerror(errno));
        return -2;
    }
    else
    {
        INFO("Server listen(%d) success", n);
        return 0;
    }
}

int
m_server::m_accept()
{
    sockaddr_in clientAddr = {};
    int addr_len = sizeof(sockaddr_in);
    SOCKET temp_socket = INVALID_SOCKET;
    
    //accept
    temp_socket = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&addr_len);
    if(temp_socket == INVALID_SOCKET)
    {
        ERROR("Server accept(%d) error -- %s", temp_socket, strerror(errno));
        return -1;
    }
    
    //连接后操作
    
    INFO("Server accept(%d) success from %s", 
         temp_socket, inet_ntoa(clientAddr.sin_addr));
    return 0;
}

void
m_server::m_close()
{
    close(_sock);
}

void
m_server::m_work()
{
    INFO("Server work start");
    
    //epoll
    int epollfd = epoll_create(1);//主线程epoll仅关注连接事件
    struct epoll_event ev;
    ev.data.fd = _sock;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, _sock, &ev);

    //处理
    while(1)
    {
        //epoll监听新连接
        struct epoll_event events;
        int event_count = epoll_wait(epollfd, &events, 1, -1);//阻塞 
        if(event_count < 0)
        {
            ERROR("Server epoll_wait() error -- %s", strerror(errno));
            break;
        }
        if(event_count == 0)
        {
            DEBUG("Server epoll_wait() timeout"); 
            continue;
        }
        else//处理新连接
        {
            m_accept();
        }
        
        //处理数据库更新任务
        //
    }
    
    close(epollfd);
    INFO("Server work finish");
}



