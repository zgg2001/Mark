/*************************************************************************
	> File Name: m_login_node.cpp
	> Author: ZHJ
	> Remarks: 服务器登录节点 处理用户登录报文
	> Created Time: Thu 27 Jan 2022 07:45:02 PM CST
 ************************************************************************/

#include<server/server/m_server.h>
#include"m_login_node.h"

m_login_node::m_login_node(int id,  m_server* server):
    _login_id(id),
    _server(server)
{
    _recv_buf = new char[RECV_BUF_SIZE];
    _tnode.id = id;
}

m_login_node::~m_login_node()
{
    DEBUG("login_node(%d) ~login_node() start", _login_id);
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
    //tnode
    _tnode.start();
}

void
m_login_node::close_node()
{
    INFO("login_node(%d) thread close", _login_id);
    _thread.close(); 
    _tnode.close_node();
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
                int ret = recvdata(events[i].data.fd);
                if(ret != 0)
                { 
                    //删除epoll事件监听
                    memset(&ev, 0, sizeof(struct epoll_event));
                    ev.data.fd = events[i].data.fd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                    
                    //处理client_node
                    if(ret == -1 || ret == -2)//退出 / error
                    {
                        delete _clients[events[i].data.fd];
                    }
                    else if(ret == 1)//登录
                    {
                        _clients[events[i].data.fd]->reset_hb();//心跳重置
                        _server->node_login(ret, _clients[events[i].data.fd]);//分配至group_node
                    }
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
    
    //正常情况下只会收到登陆包/心跳包
    while(client->get_recvlen() >= (int)sizeof(header))
    {
        //不完整包
        header* ph = (header*)client->get_recvbuf();
        if(client->get_recvlen() < ph->length)
        {
            return 0;
        }
        
        //处理
        if(ph->cmd == CMD_LOGIN)
        {
            login* pl = (login*)ph;
            std::pair<int, int> ret; 
            ret = _server->login(pl->username, pl->password);
            DEBUG("login request(%d) n:%s", sockfd, pl->username);
            
            if(ret.first == -1)//failed
            {
                INFO("login request(%d) n:%s failed", sockfd, pl->username);
                _tnode.addtask([this, sockfd]() 
                {
                    this->send_loginresult(sockfd, -1);
                });
            }
            else//success
            {
                INFO("login request(%d) n:%s success", sockfd, pl->username); 
                _tnode.addtask([this, sockfd]() 
	            {
                    this->send_loginresult(sockfd, 0);
                });
                //操作
                _clients[sockfd]->login(ret.first, ret.second);
                return ret.second;
            }
        }
        else if(ph->cmd == CMD_C2S_HEART)
        {
            //send s2c_heart
            _tnode.addtask([this, sockfd]()
            {
                this->send_s2c_heart(sockfd);
            });
            //不重置心跳
        }
        else
        {
            ERROR("login_node 收到非登录包");
            return -2;
        }
        
        //消息前移
        int size = client->get_recvlen() - ph->length;//未处理size
        memcpy(client->get_recvbuf(), client->get_recvbuf() + ph->length, size);
        client->set_recvlen(size);
    }

    return 0;
}

void 
m_login_node::send_loginresult(SOCKET sockfd, int ret)
{
    login_result lr;
    lr.result = ret;
    send(sockfd, (const char*)&lr, sizeof(lr), 0);
}

void 
m_login_node::send_s2c_heart(SOCKET sockfd)
{
    s2c_heart h;
    send(sockfd, (const char*)&h, sizeof(h), 0);
}




