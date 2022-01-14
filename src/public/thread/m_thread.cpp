/*************************************************************************
	> File Name: m_thread.cpp
	> Author: ZHJ
	> Remarks: 为项目Mark 封装的线程类 
	> Created Time: Fri 14 Jan 2022 09:35:11 AM CST
 ************************************************************************/

#include<thread>
#include"m_thread.h"

m_thread::m_thread():
    _create(nullptr),
    _run(nullptr),
    _destory(nullptr),
    _state(false)
{
}

void m_thread::start(event create, event run, event destory)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(!_state)
    {
        //事件赋值
        if(create)
            _create = create;
        if(run)
            _run = run;
        if(destory)
            _destory = destory;
        //线程启动
        _state = true;
        std::thread t(std::mem_fn(&m_thread::work), this);
        t.detach();
    }
}

void m_thread::close()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_state)
    {
        _state = false;
        _semaphore.wait();
    }
}

void m_thread::exit()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(_state)
    {
        _state = false;
        //这里的目的是退出线程 没必要阻塞等信号量
    }
}

bool m_thread::is_run()
{
    return _state;
}

void m_thread::work()
{
    //创建
    if(_create)
        _create(this);
    //运行
    if(_run)
        _run(this);
    //销毁
    if(_destory)
        _destory(this);
    _semaphore.wakeup();
}
