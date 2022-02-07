/*************************************************************************
	> File Name: m_send_node.cpp
	> Author: ZHJ
	> Remarks: 客户端send节点 向服务端send
	> Created Time: Fri 04 Feb 2022 09:40:37 AM CST
 ************************************************************************/

#include"m_send_node.h"

m_send_node::m_send_node(m_client* client):
    _client(client)
{

}

m_send_node::~m_send_node()
{
    close_node();
}

void 
m_send_node::send_login_data(std::string& name, std::string& passwd)
{
    login* l = new login();
    snprintf(l->username, 34, "%s", name.c_str());
    snprintf(l->password, 34, "%s", passwd.c_str());

    //addtask
    this->addtask([this, l]()
    {
        send(_sockfd, (const char*)l, sizeof(*l), 0); 
        delete l;
    });
}

void
m_send_node::start()
{
    DEBUG("send_node thread start");
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
m_send_node::close_node()
{
    DEBUG("send_node thread close");
    _thread.close();
}

void 
m_send_node::func_create(m_thread* thread)
{
    DEBUG("task_node func_create() start"); 
}

void 
m_send_node::func_run(m_thread* thread)
{
    DEBUG("task_node func_run() start");
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
m_send_node::func_destory(m_thread* thread)
{
    DEBUG("task_node func_destory() start");
}

void 
m_send_node::addtask(task t)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tasksbuf.push_back(t);
}





