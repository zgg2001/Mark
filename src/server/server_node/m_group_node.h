/*************************************************************************
	> File Name: m_group_node.h
	> Author: ZHJ
	> Remarks: 服务器组节点 处理以组为单位的用户报文
	> Created Time: Thu 27 Jan 2022 07:48:53 PM CST
 ************************************************************************/

#ifndef _M_GROUP_NODE_H_
#define _M_GROUP_NODE_H_

#include<string.h>
#include<vector>
#include<map>
#include<mutex>
#include<server/client_node/m_client_node.h>
#include<server/data/m_group.h>
#include<public/time/m_timer.h>
#include<public/datagram/m_datagram.h>
#include"m_base_node.h"
#include"m_task_node.h"

class m_group_node : protected m_base_node
{
public:
    using SOCKET = int;
    static constexpr int GROUP_NAME_MAX_SIZE = 50;
    static constexpr int EVENT_MAX_SIZE = 100;
    static constexpr int RECV_BUF_SIZE = 4096;

public:
    m_group_node(int g_id, const char* g_name, int a_id, m_server* server);
    virtual ~m_group_node();
    m_group_node() = delete;
    m_group_node(const m_group_node&) = delete;
    m_group_node& operator= (const m_group_node&) = delete;

    /*
    * 添加新的监听socket
    */
    void addclient(m_client_node* client);

    /*
    * 添加新用户
    */
    void adduser(int u_id, const char* name, const char* email);

    /*
    * 添加新计划
    */
    void addplan(int p_id, int u_id, int status,
                 int plan_id, int user_id,
                 int create_t, int plan_t,
                 const char* content_str, const char* remark_str);

protected:
    virtual void start() override;
    virtual void close_node() override;
    virtual void func_create(m_thread* thread) override;
    virtual void func_run(m_thread* thread) override;
    virtual void func_destory(m_thread* thread) override;

private:
    /*
    * 处理客户端消息
    * 返回-1为客户端退出/收到非法包
    */
    int recvdata(SOCKET sockfd);
    
    /*
    * 发送s2c心跳包
    */
    void send_s2c_heart(SOCKET sockfd);
    
    /*
    * 发送操作结果包
    */
    void send_operate_result(SOCKET sockfd, int ret);

    /*
    * 发送show plan结果包
    */
    void send_show_result(SOCKET sockfd, int plan_id);
    void send_show_result_u(SOCKET sockfd, int uid, int mode);
    void send_show_result_g(SOCKET sockfd, int mode);

private:
    //所属server
    m_server* _server;
    //组id
    int _group_id;
    //组名
    char _group_name[GROUP_NAME_MAX_SIZE];
    //组名长度
    int _group_name_len;
    //管理员id
    int _admin_id;
    //所属组内容
    m_group* _group;

    //锁
    std::mutex _mutex;
    //监听队列及其缓冲区
    std::vector<m_client_node*> _clientbuf;
    std::unordered_map<SOCKET, m_client_node*> _clients;
    //当前监听用户数量
    int _client_num;

    //recv_buf
    char* _recv_buf;
    
    //所属task节点
    m_task_node _tnode;
};

#endif
