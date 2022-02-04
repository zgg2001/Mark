/*************************************************************************
	> File Name: m_client.h
	> Author: ZHJ
	> Remarks: 客户端类
	> Created Time: Fri 04 Feb 2022 09:19:36 AM CST
 ************************************************************************/

#ifndef _M_CLIENT_H_
#define _M_CLIENT_H_

#include<unistd.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<string.h>
#include<errno.h>

#include<public/log/log.h>

class m_client
{
public:
    using SOCKET = int;
    static constexpr SOCKET INVALID_SOCKET = (SOCKET)(~0);
    static constexpr int SOCKET_ERROR = -1;

public:
    m_client();
    virtual ~m_client();
    m_client(const m_client&) = delete;
    m_client& operator= (const m_client&) = delete;
    
    /* 
    * 初始化
    * socket环境配置
    * 返回0为成功
    */
    int m_init();

    /*
    * 连接服务端
    *   ip: 服务端ip
    * port: 服务端端口
    * 返回0为成功
    */
    int m_connect(const char* ip, unsigned short port);

    /*
    * 关闭
    */
    void m_close();

private:
    //本机socket
    SOCKET _sock;
};

#endif
