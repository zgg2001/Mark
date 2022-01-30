/*************************************************************************
	> File Name: m_client_node.h
	> Author: ZHJ
	> Remarks: 客户端节点 一个实例对应一个客户端连接
	> Created Time: Thu 27 Jan 2022 07:50:21 PM CST
 ************************************************************************/

#ifndef _M_CLIENT_NODE_H_
#define _M_CLIENT_NODE_H_

#include<unistd.h>
#include<time.h>
#include<public/log/log.h>

/*
* 客户端的两种状态
* 未登录 = 0 / 已登录 = 1
*/
enum class client_status
{
    nologin,
    logined
};

class m_client_node
{
public:
    using SOCKET = int;
    static constexpr int CLIENT_RECV_BUF_SIZE = 4096 * 5;
    /* 
    * 未登录: 60秒 死亡时间(客户端不会自动发心跳包 -- 60秒不登录即超时) 
    * 已登录: 20秒 死亡时间(客户端会自动发心跳包 -- 连接正常不会超时)
    */
    static constexpr double CLIENT_HREAT_TIME_NOLOGIN = 60.00;
    static constexpr double CLIENT_HREAT_TIME_LOGINED = 20.00;

public:
    m_client_node(SOCKET sockfd);
    virtual ~m_client_node();
    m_client_node(const m_client_node&) = delete;
    m_client_node& operator= (const m_client_node&) = delete;

    //get
    bool is_login() const { return _status == client_status::logined; }
    SOCKET get_sockfd() const { return _sockfd; }
    int get_uid() const { return _user_id; }
    int get_gid() const { return _group_id; }

    /*
    * recv缓冲区操作
    */
    char* get_recvbuf() const { return _msg_recv_buf; }
    int get_recvlen() const { return _len_recv_buf; }
    void set_recvlen(int len) { _len_recv_buf = len; }

    /*
    * 客户端登录
    * 记录id / 初始化缓冲区
    * 返回true为成功
    */
    bool login(int uid, int gid);

    /*
    * 心跳检测操作
    * 重置计数 / 检测(返回true为超时)
    */
    void reset_hb() { _hb_time = 0; }
    bool check_hb(double time_interval);

private:
    //socket
    SOCKET _sockfd;
    //status
    client_status _status;

    //所属 user/group id
    int _user_id;
    int _group_id;

    //缓冲区与其尾部下标
    char* _msg_recv_buf;
    int _len_recv_buf;

    //心跳计时器 单位:秒
    double _hb_time;
};

#endif
