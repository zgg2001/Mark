/*************************************************************************
	> File Name: m_task_node.cpp
	> Author: ZHJ
	> Remarks: 任务节点
	> Created Time: Sun 06 Feb 2022 05:38:29 PM CST
 ************************************************************************/

#include"m_task_node.h"

m_task_node::m_task_node():
    id(-1)
{

}

m_task_node::~m_task_node()
{
    close_node();
}

void 
m_task_node::addtask(task t)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tasksbuf.push_back(t);
}

void
m_task_node::start()
{
    INFO("task_node(%d) thread start", id);
    _thread.start(
        //create
        [this](m_thread*)
        {
            func_create(&_thread);
        },
        //run
        [this](m_thread*)
        {
            func_run(&_thread);
        },
        //destory
        [this](m_thread*)
        {
            func_destory(&_thread);
        }
    );
}

void
m_task_node::close_node()
{
    INFO("task_node(%d) thread close", id);
    _thread.close();
}

void 
m_task_node::func_create(m_thread* thread)
{
    DEBUG("task_node(%d) func_create() start", id); 
}

void 
m_task_node::func_run(m_thread* thread)
{
    DEBUG("task_node(%d) func_run() start", id);
    while(_thread.is_run())
    {
        //将缓冲区内数据加入 
        if(!_tasksbuf.empty()) 
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(auto& t : _tasksbuf)
            {
                _tasks.push_back(t);
            }
            _tasksbuf.clear();
        }
        
        //如果无任务
        if(_tasks.empty())
        {
            usleep(1000);//1ms
            continue;
        }
        
        //处理任务
        for(auto& t : _tasks)
        {
            t();
        }
        _tasks.clear();
    }
}

void 
m_task_node::func_destory(m_thread* thread)
{
    DEBUG("task_node(%d) func_destory() start", id);
}
