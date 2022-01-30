/*************************************************************************
	> File Name: m_client_node.cpp
	> Author: ZHJ
	> Remarks: 客户端节点 一个实例对应一个客户端连接
	> Created Time: Thu 27 Jan 2022 07:50:39 PM CST
 ************************************************************************/

#include"m_client_node.h"

m_client_node::m_client_node(SOCKET sockfd):
    _sockfd(sockfd),
    _status(client_status::nologin),
    _user_id(-1),
    _group_id(-1),
    _msg_recv_buf(nullptr),
    _len_recv_buf(0),
    _hb_time(0)
{

}

m_client_node::~m_client_node()
{
    INFO("client_node socket(%d) uid(%d) gid(%d) quit",
         _sockfd, _user_id, _group_id);
    if(_msg_recv_buf != nullptr)
        delete[] _msg_recv_buf;
    close(_sockfd);
}

bool 
m_client_node::login(int uid, int gid)
{
    //已登录
    if(_status == client_status::logined)
        return false;

    //初始化id与缓冲区
    _user_id = uid;
    _group_id = gid;
    _msg_recv_buf = new char[CLIENT_RECV_BUF_SIZE];
    _len_recv_buf = 0;
    _status = client_status::logined;
    
    INFO("client_node socket(%d) uid(%d) gid(%d) login", 
         _sockfd, _user_id, _group_id);
    return true;
}

bool 
m_client_node::check_hb(double time_interval)
{
    _hb_time += time_interval;
    switch(_status)
    {
        //未登录状态
        case client_status::nologin:
            if(_hb_time > CLIENT_HREAT_TIME_NOLOGIN)
            {
                INFO("check_hb dead socket(%d) time(%lf)",
                     _sockfd, _hb_time);
                return true;
            }
        break;
        
        //已登录状态
        case client_status::logined:
            if(_hb_time > CLIENT_HREAT_TIME_LOGINED)
            {
                INFO("check_hb dead socket(%d) time(%lf) uid(%d) gid(%d)",
                     _sockfd, _hb_time, _user_id, _group_id);
                return true;
            }
        break;
    }
    return false;
}





