/*************************************************************************
	> File Name: m_noecho.h
	> Author: ZHJ
	> Remarks: 取消反显 方便输密码
	> Created Time: Sun 06 Feb 2022 07:33:16 PM CST
 ************************************************************************/

#ifndef _M_NOECHO_H_
#define _M_NOECHO_H_

#include<string.h>
#include<termios.h>

class m_noecho
{
public:
    m_noecho();
    virtual ~m_noecho();

    /*
    * 设置无回显输入模式
    */
    void noecho();

    /*
    * 恢复
    */
    void recover();

private:
    struct termios _oldterm;
    
    //是否处于无回显状态
    bool _status;
};

#endif
