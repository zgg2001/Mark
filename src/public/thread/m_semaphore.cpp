/*************************************************************************
	> File Name: m_semaphore.cpp
	> Author: ZHJ
	> Remarks: 线程安全退出所使用的信号量类 
	> Created Time: Fri 14 Jan 2022 10:12:17 AM CST
 ************************************************************************/

#include"m_semaphore.h"

m_semaphore::m_semaphore():
    _wait(0),
    _wakeup(0)
{
}

void m_semaphore::wait()
{
    std::unique_lock<std::mutex> lock(_mutex);
    if(--_wait < 0)
    {
        //阻塞开始 等待唤醒
        _cv.wait(lock, [this]() 
        {
            return _wakeup > 0;
        });
        --_wakeup;
    }
}

void m_semaphore::wakeup()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(++_wait <= 0)
    {
        ++_wakeup;
        //通知第一个等待线程
        _cv.notify_one();
    }
}

