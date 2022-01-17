/*************************************************************************
	> File Name: m_timer.h
	> Author: ZHJ
	> Remarks: 高精度计时器类
	> Created Time: Mon 17 Jan 2022 08:36:03 PM CST
 ************************************************************************/

#ifndef _M_TIMER_H_
#define _M_TIMER_H_

#include<mutex>
#include<chrono>

class m_timer
{
public:
    m_timer();

    /*
    * 更新起始时间
    * safe为线程安全版
    */
    void update();
    void update_safe();

    /*
    * 计算当前时间与起始时间的时间差
    * safe为线程安全版
    *
    * 返回值为double类型 单位为秒
    */
    double get_sec();
    double get_sec_safe();

private:
    std::mutex _lock;
    
    //记录起始/终止时间
    std::chrono::steady_clock::time_point _begin;
    std::chrono::steady_clock::time_point _end;

};

#endif
