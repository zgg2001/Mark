/*************************************************************************
	> File Name: m_semaphore.h
	> Author: ZHJ
	> Remarks: 线程安全退出所使用的信号量类
	> Created Time: Fri 14 Jan 2022 10:11:43 AM CST
 ************************************************************************/

#ifndef _M_SEMAPHORE_H_
#define _M_SEMAPHORE_H_

#include<condition_variable>//条件变量
#include<mutex>

class m_semaphore
{
public:
    m_semaphore();
    m_semaphore(const m_semaphore&) = delete;
    m_semaphore& operator= (const m_semaphore&) = delete;
    
    //开始阻塞
    void wait();
    
    //唤醒
    void wakeup();

private:
    //等待数与唤醒数
    int _wait;
    int _wakeup;
    
    //条件变量与锁
    std::condition_variable _cv;
    std::mutex _mutex;

};

#endif
