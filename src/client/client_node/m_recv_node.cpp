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
    _sockfd(INVALID_SOCKET),
    _recv_len_2(0),
    _mytimer(0.0)
{
    //recv_buf初始化
    _recv_buf = new char[RECV_BUF_SIZE];
    //二级缓冲
    _recv_buf_2 = new char[RECV_BUF_SIZE * 5];
}

m_recv_node::~m_recv_node()
{
    DEBUG("recv_node ~ start");
    close_node();
    delete[] _recv_buf;
    delete[] _recv_buf_2;
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

    //计时器->心跳计时
    m_timer timer;
    timer.update();

    //epoll
    int epollfd = epoll_create(1);//主线程epoll仅关注连接事件
    struct epoll_event ev;
    ev.data.fd = _sockfd;
    ev.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, _sockfd, &ev);

    //开始工作
    while(_thread.is_run())
    {
        //更新心跳
        _mytimer += timer.get_sec();
        timer.update();
        if(_mytimer >= 5.0)//超时
        {
            DEBUG("recv_node thread exit");
            _thread.exit();
            _client->m_exit();
        }
        
        //epoll监听
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
                DEBUG("recv_node thread exit");
                _thread.exit();
                _client->m_exit();
            }
            _mytimer = 0.0;//心跳计时归零
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

    //内容转至二级缓冲
    memcpy(_recv_buf_2 + _recv_len_2, _recv_buf, buf_len);
    _recv_len_2 += buf_len;
    
    //处理
    while(_recv_len_2 >= (int)sizeof(header))
    {
        //不完整包
        header* ph = (header*)_recv_buf_2;
        if(_recv_len_2 < ph->length)
        {
            return 0;
        }
        
        //处理
        if(ph->cmd == CMD_LOGIN_RESULT)
        {
            login_result* plr = (login_result*)ph;
            //设置ret并唤醒
            _client->m_login_wake(plr->result); 
        }
        else if(ph->cmd == CMD_OPERATE_RESULT)
        {
            operate_result* oret = (operate_result*)ph;
            //设置ret并唤醒
            _client->m_operate_wake(oret->result);
        }
        else if(ph->cmd == CMD_S2C_HEART)
        {
            //DEBUG("收到心跳包");
        }
        else
        {
            WARN("login_node 未知包");
        } 
        
        //消息前移
        int size = _recv_len_2 - ph->length;//未处理size
        memcpy(_recv_buf_2, _recv_buf_2 + ph->length, size);
        _recv_len_2 = size;
    }
    
    return 0;
}
