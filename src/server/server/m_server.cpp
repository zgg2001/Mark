/*************************************************************************
	> File Name: m_server.cpp
	> Author: ZHJ
	> Remarks: 服务端类 最外层
	> Created Time: Thu 27 Jan 2022 07:06:58 PM CST
 ************************************************************************/

#include"m_server.h"

m_server::m_server():
    _sock(INVALID_SOCKET),
    _pk_plan_id(-1),
    _pk_user_id(-1),
    _pk_group_id(-1),
    _lnodes(nullptr)
{
    //数据库mgr
    _db = new m_db_mgr(this);

    //登录节点
    _lnodes = new m_login_node(0, this);
}

m_server::~m_server()
{
    m_close();

    delete _lnodes;
    //free组节点
    for(auto& iter : _gnodes)
    { 
        delete iter.second;
    }
    
    close(_sock);
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
    if(_db->init("localhost", "root", "Zhj@7512") < 0)
    {
        return -2;
    }
    _pk_plan_id = _db->get_pk_plan_id();
    _pk_user_id = _db->get_pk_user_id();
    _pk_group_id = _db->get_pk_group_id();
    DEBUG("plan_pk:%d user_pk:%d group_pk:%d", _pk_plan_id, _pk_user_id, _pk_group_id);

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
        close(_sock);
        _sock = INVALID_SOCKET;
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
        close(_sock);
        _sock = INVALID_SOCKET;
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
    
    //连接后分配给登录节点
    _lnodes->addclient(new m_client_node(temp_socket));
    
    INFO("Server accept(%d) success from %s", 
         temp_socket, inet_ntoa(clientAddr.sin_addr));
    return 0;
}

void
m_server::m_close()
{
    DEBUG("Server m_close start");
    _lnodes->close_node();
}

void
m_server::m_start()
{
    if(_lnodes != nullptr)
    {
        INFO("Server login_node start");
        _lnodes->start();
    }
    else
    {
        ERROR("Server login_node error -- not initialized");
    }
}

void
m_server::m_work()
{
    INFO("Server work start");
    
    //无效套接字 
    if(_sock == INVALID_SOCKET)
    {
        ERROR("Server work faild -- server socket not initialized");
        return;
    }

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
        int event_count = epoll_wait(epollfd, &events, 1, 1); 
        if(event_count < 0)
        {
            ERROR("Server epoll_wait() error -- %s", strerror(errno));
            break;
        }
        else if(event_count > 0)//处理新连接
        {
            m_accept();
        }
        
        //数据库更新task缓冲区 
        if(!_tasksbuf.empty()) 
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(auto& t : _tasksbuf)
            {
                _tasks.push_back(t);
            }
            _tasksbuf.clear();
        }
        
        //处理数据库更新任务
        if(_tasks.empty())
            continue;
        for(auto& t : _tasks)
        {
            t();
            DEBUG("while(1) add_plan end");
        }
        _tasks.clear();
    }
    
    close(epollfd);
    INFO("Server work finish");
}

void 
m_server::new_group_node(int g_id, const char* g_name, int a_id)
{
    _gnodes[g_id] = new m_group_node(g_id, g_name, a_id, this);
}

void
m_server::new_group_user(int g_id, int u_id, const char* name, const char* email)
{
    _gnodes[g_id]->adduser(u_id, name, email);
}

void 
m_server::new_group_plan(int g_id, int p_id, int u_id, int status,
                         int plan_id, int user_id,
                         int create_t, int plan_t,
                         const char* content_str, const char* remark_str)
{
    _gnodes[g_id]->addplan(p_id, u_id, status, plan_id, user_id,
                           create_t, plan_t, content_str, remark_str);
}

std::pair<int, int> 
m_server::login(const char* name, const char* passwd)
{
    std::string n {name};
    std::string p {passwd};
    return _db->login(n, p);
}

void 
m_server::node_login(int gid, m_client_node* node)
{
    if(_gnodes.count(gid) == 0)
    {
        ERROR("node_login error -- group node(%d) does not exist", gid);
        return;
    }
    DEBUG("node_login success");
    _gnodes[gid]->addclient(node);
}

int 
m_server::add_plan(int g_id, int p_id, int user_id,
                   int create_t, int plan_t,
                   const char* content_str, const char* remark_str)
{
    std::lock_guard<std::mutex> lock(_mutex);
    char* c_str = new char[102];
    char* r_str = new char[102];
    int id = _pk_plan_id;

    snprintf(c_str, 102, "%s", content_str);
    snprintf(r_str, 102, "%s", remark_str);

    _tasksbuf.push_back([=](){
        _db->add_plan(g_id, p_id, user_id,
                      id, create_t, plan_t,
                      c_str, r_str);
        delete[] c_str;
        delete[] r_str;
        DEBUG("taskbuf add_plan end");
    });
    DEBUG("m_server add_plan end");
    return _pk_plan_id++;
}




