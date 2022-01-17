/*************************************************************************
	> File Name: m_timer.cpp
	> Author: ZHJ
	> Remarks: 高精度计时器类 
	> Created Time: Mon 17 Jan 2022 08:36:22 PM CST
 ************************************************************************/

#include"m_timer.h"

m_timer::m_timer()
{
    _begin = std::chrono::steady_clock::now();
    _end = std::chrono::steady_clock::time_point();
}

void 
m_timer::update()
{
    _begin = std::chrono::steady_clock::now();
}

void 
m_timer::update_safe()
{
    //lock
    std::lock_guard<std::mutex>lock(_lock);
    
    _begin = std::chrono::steady_clock::now();
}

double 
m_timer::get_sec()
{
    _end = std::chrono::steady_clock::now();
    //使用duration类型变量进行时间的储存   duration_cast是类型转换方法
    std::chrono::duration<double> temp = std::chrono::duration_cast<std::chrono::duration<double>>(_end - _begin);
    return temp.count();//count() 获取当前时间的计数数量
}

double 
m_timer::get_sec_safe()
{
    //lock
    std::lock_guard<std::mutex>lock(_lock);
    
    _end = std::chrono::steady_clock::now();
    //使用duration类型变量进行时间的储存   duration_cast是类型转换方法
    std::chrono::duration<double> temp = std::chrono::duration_cast<std::chrono::duration<double>>(_end - _begin);
    return temp.count();//count() 获取当前时间的计数数量
}
