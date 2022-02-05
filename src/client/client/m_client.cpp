/*************************************************************************
	> File Name: m_client.cpp
	> Author: ZHJ
	> Remarks: 客户端类
	> Created Time: Fri 04 Feb 2022 09:19:41 AM CST
 ************************************************************************/

#include"m_client.h"

m_client::m_client():
    _sock(INVALID_SOCKET),
    _status(false),
    _cl(nullptr),
    _rnode(nullptr)
{
    _rnode = new m_recv_node(this);
}

m_client::~m_client()
{
    m_close();
    
    delete _rnode;
    close(_sock);
    
    if(_cl != nullptr)
    {
        cmdline_exit_free(_cl);
        _cl = nullptr;
    }
}

int 
m_client::m_init()
{
    //创建socket
    if(_sock != INVALID_SOCKET)//存在socket
    {
        WARN("Client init warn -- socket:%d already exists", _sock);
        m_close();
    }
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == INVALID_SOCKET)
    {
        ERROR("Client init faild -- %s", strerror(errno));
        return -1;
    }

    DEBUG("Client init(%d) success", _sock);
    return 0;
}

int 
m_client::m_connect(const char* ip, unsigned short port)
{
    //无效套接字
    if(_sock == INVALID_SOCKET)
    {
        WARN("Client bind warn -- client socket not initialized");
        m_init();
    }

    //配置
   	sockaddr_in _sin = {};
   	_sin.sin_family = AF_INET;//IPV4
   	_sin.sin_port = htons(port);//端口
	_sin.sin_addr.s_addr = inet_addr(ip);//IP

    //connect
    int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(ret == SOCKET_ERROR)
	{
        ERROR("Client connect(%d) ip:%s port:%d faild -- %s", _sock, ip, port, strerror(errno));
        close(_sock);
        return -1;
	}
    
    DEBUG("Client connect(%d) success", _sock);
    _status = true;
    return 0; 
}

void 
m_client::m_start()
{
    DEBUG("Client m_start start");
    _rnode->set_sockfd(_sock);
    _rnode->start();
}

void
m_client::m_work()
{
    if(_status == false)
        return;
    printf("连接服务器... 成功\n");
    
    //新建命令行
    _cl = cmdline_get_new(nullptr, "Mark>"); 
    //开始交互工作
    cmdline_start_interact(_cl);
    //退出
    cmdline_exit_free(_cl);
    _cl = nullptr;
}

void 
m_client::m_close()
{
    DEBUG("Client m_close start");
    _rnode->close_node();
}


