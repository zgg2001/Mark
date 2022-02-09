/*************************************************************************
	> File Name: m_input.h
	> Author: ZHJ
	> Remarks: 输入部分 设置终端输入模式
	> Created Time: Tue 08 Feb 2022 10:28:19 PM CST
 ************************************************************************/

#ifndef _M_INPUT_H_
#define _M_INPUT_H_

#include<string.h>
#include<termios.h>

class m_input
{
public:
    m_input();
    virtual ~m_input();

    /*
    * 设置为输入模式
    */
    void input();

    /*
    * 恢复
    */
    void recover();

private:
    struct termios _oldterm;
    
    //是否处于输入状态
    bool _status;
};

#endif
