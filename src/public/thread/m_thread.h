/*************************************************************************
	> File Name: m_thread.h
	> Author: ZHJ
	> Remarks: 为项目Mark 封装的线程类
	> Created Time: Fri 14 Jan 2022 09:34:34 AM CST
 ************************************************************************/

#ifndef _M_THREAD_H_
#define _M_THREAD_H_

#include<functional>
#include"m_semaphore.h"

class m_thread
{
private:
    using event = std::function<void(m_thread*)>;

public:
    m_thread();
    m_thread(const m_thread&) = delete;
    m_thread& operator= (const m_thread&) = delete;

    /* 
    * 启动线程
    * 传入三个函数
    * 
    *  create: 线程启动时执行
    *     run: 线程常规运行中执行
    * destory: 线程退出时执行
    */
    void start(event create = nullptr, event run = nullptr, event destory = nullptr);
    
    /* 关闭线程 (安全退出) */
    void close();
    
    /* 退出线程 (不安全退出) */
    void exit();
    
    /* 是否运行中 */
    bool is_run();

protected:
    /* 工作函数 */
    void work();

private:
    //三个事件 创建/运行/销毁
    event _create;
    event _run;
    event _destory;
    
    //锁与信号量
    std::mutex _mutex;
    m_semaphore _semaphore;

    //线程是否启动
    bool _state;
};

#endif
