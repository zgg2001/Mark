/*************************************************************************
	> File Name: m_task_node.h
	> Author: ZHJ
	> Remarks: 任务节点
	> Created Time: Sun 06 Feb 2022 05:27:10 PM CST
 ************************************************************************/

#ifndef _M_TASK_NODE_H_
#define _M_TASK_NODE_H_

#include<unistd.h>
#include<mutex>
#include<list>
#include<functional>
#include"m_base_node.h"

class m_task_node : public m_base_node
{
public:
    using task = std::function<void()>;

public:
    m_task_node();
    virtual ~m_task_node();
    m_task_node(const m_task_node&) = delete;
    m_task_node& operator= (const m_task_node&) = delete;
    
    /*
    * 添加任务
    */
    void addtask(task t);
    
    /*
    * 启动与关闭登录节点
    */
    virtual void start() override;
    virtual void close_node() override;

protected:
    virtual void func_create(m_thread* thread) override;
    virtual void func_run(m_thread* thread) override;
    virtual void func_destory(m_thread* thread) override;

public:
    //id
    int id;

private:
    /*
    * task缓冲区相关
    */
    std::list<task> _tasks;
    std::list<task> _tasksbuf;
    std::mutex _mutex;
};

#endif
