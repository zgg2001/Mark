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
    DEBUG("send_node ~ start");
    close_node();
    DEBUG("send_node ~ start");
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
m_send_node::send_add_plan_data(std::string& content, std::string& remark, int nowtime, int time)
{
    add_plan* ap = new add_plan();
    ap->create_time = nowtime;
    ap->plan_time = time;
    snprintf(ap->content, 102, "%s", content.c_str());
    snprintf(ap->remark, 102, "%s", remark.c_str());

    //addtask
    this->addtask([this, ap]()
    {
        send(_sockfd, (const char*)ap, sizeof(*ap), 0); 
        delete ap;
    }); 
}

void
m_send_node::send_delete_plan_data(int id)
{
    del_plan* dp = new del_plan();
    dp->plan_id = id;

    //addtask
    this->addtask([this, dp](){
        send(_sockfd, (const char*)dp, sizeof(*dp), 0); 
        delete dp;
    });
}

void 
m_send_node::send_update_time_data(int id, int time)
{
    upd_plan_t* upt = new upd_plan_t();
    upt->plan_id = id;
    upt->plan_time = time;

    //addtask
    this->addtask([this, upt]()
    {
        send(_sockfd, (const char*)upt, sizeof(*upt), 0); 
        delete upt;
    });  
}

void 
m_send_node::send_update_status_data(int id, int status)
{
    upd_plan_s* ups = new upd_plan_s();
    ups->plan_id = id;
    ups->status = status;

    //addtask
    this->addtask([this, ups]()
    {
        send(_sockfd, (const char*)ups, sizeof(*ups), 0); 
        delete ups;
    });  
}

void 
m_send_node::send_update_content_data(int id, std::string& content, std::string& remark)
{
    upd_plan_c* upc = new upd_plan_c();
    upc->plan_id = id;
    snprintf(upc->content, 102, "%s", content.c_str());
    snprintf(upc->remark, 102, "%s", remark.c_str());

    //addtask
    this->addtask([this, upc]()
    {
        send(_sockfd, (const char*)upc, sizeof(*upc), 0); 
        delete upc;
    });  
}

void
m_send_node::send_show_plan_u_data(int mode)
{
    show_plan_u* spu = new show_plan_u();
    spu->mode = mode;

    //addtask
    this->addtask([this, spu]()
    {
        send(_sockfd, (const char*)spu, sizeof(*spu), 0); 
        delete spu;
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
    DEBUG("send_node func_create() start"); 
}

void 
m_send_node::func_run(m_thread* thread)
{
    DEBUG("send_node func_run() start");
    
    //计时器->发心跳包
    m_timer timer;
    timer.update();
    double mytimer = 0.0;
    
    while(_thread.is_run())
    {
        //心跳包 两秒一发
        mytimer += timer.get_sec();
        timer.update();
        if(mytimer >= 2.0)
        {
            c2s_heart h;
            send(_sockfd, (const char*)&h, sizeof(h), 0);
            mytimer = 0.0;
        }
        
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
    DEBUG("send_node func_destory() start");
}

void 
m_send_node::addtask(task t)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tasksbuf.push_back(t);
}





