/*************************************************************************
	> File Name: m_recv_node.h
	> Author: ZHJ
	> Remarks: 客户端recv节点 监听客户端sockfd
	> Created Time: Fri 04 Feb 2022 09:39:57 AM CST
 ************************************************************************/

#ifndef _M_RECV_NODE_H_
#define _M_RECV_NODE_H_

#include<public/datagram/m_datagram.h>
#include"m_base_node.h"

class m_client;

class m_recv_node : public m_base_node
{
public:
    using SOCKET = int;
    static constexpr SOCKET INVALID_SOCKET = (SOCKET)(~0);
    static constexpr int SOCKET_ERROR = -1;
    static constexpr int RECV_BUF_SIZE = 4096;

public:
    m_recv_node(m_client* client);
    virtual ~m_recv_node();
    m_recv_node() = delete;
    m_recv_node(const m_recv_node&) = delete;
    m_recv_node& operator= (const m_recv_node&) = delete;

    //设置本机socket
    void set_sockfd(SOCKET sockfd) { _sockfd = sockfd; }

    /*
    * 启动与关闭登录节点
    */
    virtual void start() override;
    virtual void close_node() override;

protected:
    virtual void func_create(m_thread* thread) override;
    virtual void func_run(m_thread* thread) override;
    virtual void func_destory(m_thread* thread) override;

private: 
    /*
    * 处理服务端消息
    * 返回-1为异常
    */
    int recvdata();

private:
    //所属client
    m_client* _client;
    
    //本机socket
    SOCKET _sockfd;
    
    //recv_buf
    char* _recv_buf;
};

#endif
