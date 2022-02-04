/*************************************************************************
	> File Name: m_client.cpp
	> Author: ZHJ
	> Remarks: 客户端类
	> Created Time: Fri 04 Feb 2022 09:19:41 AM CST
 ************************************************************************/

#include"m_client.h"

m_client::m_client():
    _sock(INVALID_SOCKET)
{

}

m_client::~m_client()
{
    m_close();
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

    INFO("Client init(%d) success", _sock);
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
    
    INFO("Client connect(%d) success", _sock);
    return 0; 
}

void 
m_client::m_close()
{
    close(_sock);
}


