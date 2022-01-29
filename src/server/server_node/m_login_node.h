/*************************************************************************
	> File Name: m_login_node.h
	> Author: ZHJ
	> Remarks: 服务器登录节点 处理用户登录报文
	> Created Time: Thu 27 Jan 2022 07:44:50 PM CST
 ************************************************************************/

#ifndef _M_LOGIN_NODE_H_
#define _M_LOGIN_NODE_H_

#include<mutex>
#include"m_base_node.h"

class m_login_node : public m_base_node
{
public:
    using SOCKET = int;
    static constexpr int EVENT_MAX_SIZE = 100;
    static constexpr int RECV_BUF_SIZE = 4096;

public:
    m_login_node(int id, m_server* server);
    virtual ~m_login_node();
    
    /*
    * 启动与关闭登录节点
    */
    virtual void start() override;
    virtual void close_node() override;

    /*
    * 添加新的监听socket
    */
    void addclient(SOCKET sockfd);

protected:
    virtual void func_create(m_thread* thread) override;
    virtual void func_run(m_thread* thread) override;
    virtual void func_destory(m_thread* thread) override;

private:
    /*
    * 处理客户端消息
    * 返回-1为客户端退出
    */
    int recvdata(SOCKET sockfd);

private:
    //id
    int _login_id;
    //所属server
    m_server* _server;

    //锁
    std::mutex _mutex;
    //监听队列缓冲区
    std::vector<SOCKET> _clientbuf;
    //recv_buf
    char* _recv_buf;
};

#endif
