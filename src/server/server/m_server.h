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

#include<server/database/m_db_mgr.h>
#include<server/server_node/m_login_node.h>
#include<server/server_node/m_group_node.h>
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
    m_server(const m_server&) = delete;
    m_server& operator= (const m_server&) = delete;

    /* 
    * 环境/数据初始化
    * 返回0为成功
    */
    int m_init();

    /*
    * 绑定ip/端口
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

    // 关闭server
    void m_close();

    // 启动登录节点
    void m_start();

    //开始工作
    void m_work();

    /*
    * (反序列化)
    * 新增组节点
    * 新增组用户
    * 新增组计划
    *
    * g_id: 组id 
    * 其余参数参考m_group内的addplan/adduser方法
    */
    void new_group_node(int g_id, const char* g_name, int a_id);
    void new_group_user(int g_id, int u_id, const char* name, const char* email);
    void new_group_plan(int g_id, int p_id, int u_id, int status,
                        int plan_id, int user_id,
                        int create_t, int plan_t,
                        const char* content_str, const char* remark_str);
    
    /*
    * 登录比对
    *
    * 返回值:
    * pair<uid, gid>
    * 登录失败时uid = gid = -1
    * 否则为用户的uid/gid
    */
    std::pair<int, int> login(const char* name, const char* passwd);

    /*
    * 客户端节点登录
    */
    void node_login(int gid, m_client_node* node);

private:
    //服务器socket
    SOCKET _sock;

    //数据库管理者
    m_db_mgr* _db;

    //登录节点
    m_login_node* _lnodes;

    //组节点
    std::unordered_map<int, m_group_node*> _gnodes;
};

#endif
