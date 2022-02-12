/*************************************************************************
	> File Name: m_send_node.h
	> Author: ZHJ
    > Remarks: 客户端send节点 向服务端send
	> Created Time: Fri 04 Feb 2022 09:40:10 AM CST
 ************************************************************************/

#ifndef _M_SEND_NODE_H_
#define _M_SEND_NODE_H_

#include<list>
#include<string>
#include<string.h>

#include<public/time/m_timer.h>
#include<public/datagram/m_datagram.h>
#include"m_base_node.h"

class m_client;

class m_send_node : public m_base_node
{
public:
    using SOCKET = int;
    using task = std::function<void()>;

public:
    m_send_node(m_client* client);
    virtual ~m_send_node();
    m_send_node() = delete;
    m_send_node(const m_send_node&) = delete;
    m_send_node& operator= (const m_send_node&) = delete;
    
    //设置本机socket
    void set_sockfd(SOCKET sockfd) { _sockfd = sockfd; }

    /*
    * 发送登陆包
    */
    void send_login_data(std::string& name, std::string& passwd);

    /*
    * 新增计划报文
    */
    void send_add_plan_data(std::string& content, std::string& remark, int nowtime, int time);
    
    /*
    * 新增计划报文
    */
    void send_delete_plan_data(int id);

    /*
    * 更新计划报文
    */
    void send_update_time_data(int id, int time);
    void send_update_status_data(int id, int status);
    void send_update_content_data(int id, std::string& content, std::string& remark);

    /*
    * 展示计划报文
    */
    void send_show_plan_u_data(int mode);
    
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
    * 添加任务
    */
    void addtask(task t);

private:
    //所属client
    m_client* _client;
    
    //本机socket
    SOCKET _sockfd;
    
    /*
    * task缓冲区相关
    */
    std::list<task> _tasks;
    std::list<task> _tasksbuf;
    std::mutex _mutex;
};

#endif
