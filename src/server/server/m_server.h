/*************************************************************************
	> File Name: m_server.h
	> Author: ZHJ
	> Remarks: 服务端类 最外层
	> Created Time: Thu 27 Jan 2022 07:06:54 PM CST
 ************************************************************************/

#ifndef _M_SERVER_H_
#define _M_SERVER_H_

#include<unistd.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<string.h>
#include<errno.h>

#include<public/log/log.h>

class m_server
{
public:
    using SOCKET = int;
	static constexpr SOCKET INVALID_SOCKET = (SOCKET)(~0);
	static constexpr int SOCKET_ERROR = -1;

public:
    m_server();
    virtual ~m_server();

    /* 
    * 初始化
    * 数据读取/socket环境配置
    * 返回0为成功
    */
    int m_init();

    /*
    * 绑定ip/端口
    *   ip: 监听ip (nullptr为全部)
    * port: 监听端口
    * 返回0为成功
    */
    int m_bind(const char* ip, unsigned short port);

    /*
    * 开始监听端口
    * n: 连接队列大小
    * 返回0为成功
    */
    int m_listen(int n);

    /*
    * 接受客户端连接
    * 返回0为成功
    */
    int m_accept();

    /*
    * 关闭
    */
    void m_close();

    /*
    * 开始工作
    */
    void m_work();

private:
    //服务器socket
    SOCKET _sock;

};

#endif
