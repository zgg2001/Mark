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
#include<memory>

#include<stdio.h>
#include<string.h>
#include<errno.h>

#include<nice_cmd/cmdline.h>
#include<public/log/log.h>
#include<client/client_node/m_recv_node.h>

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
    * 环境初始化
    * 返回0为成功
    */
    int m_init();

    /*
    * 连接服务端
    * 返回0为成功
    */
    int m_connect(const char* ip, unsigned short port);

    //接收/发送线程开始工作
    void m_start();
    
    //开始交互工作
    void m_work();

    //关闭client
    void m_close();

private:
    //本机socket
    SOCKET _sock;

    //连接状态
    bool _status;

    //交互式命令行
    struct cmdline* _cl;

    //接收节点
    m_recv_node* _rnode;
};

#endif
