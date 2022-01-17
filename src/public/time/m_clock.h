/*************************************************************************
	> File Name: m_clock.h
	> Author: ZHJ
	> Remarks: 高精度时间类
	> Created Time: Mon 17 Jan 2022 09:22:41 PM CST
 ************************************************************************/

#ifndef _M_CLOCK_H_
#define _M_CLOCK_H_

#include<chrono>

class m_clock
{
public:
    /* 
    * 获取当前时间戳 (毫秒)
    * 返回值为 time_t 类型
    */
    static time_t 
    get_now_millsec()
    {
        //获取高精度当前时间(毫秒) high_resolution_clock::now();
        //duration_cast是类型转换方法
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
};

#endif
