/*************************************************************************
	> File Name: m_input.cpp
	> Author: ZHJ
	> Remarks: 输入部分 设置终端输入模式
	> Created Time: Tue 08 Feb 2022 10:29:14 PM CST
 ************************************************************************/

#include"m_input.h"

m_input::m_input():
    _status(false)
{

}

m_input::~m_input()
{
    if(_status == true)
    {
        recover();
    }
}

void 
m_input::input()
{
    struct termios term;
    
    //获取/保存当前终端设置
    tcgetattr(0, &term);
    memcpy(&_oldterm, &term, sizeof(struct termios));

    //设置
    term.c_lflag |= ICANON;
    term.c_lflag |= ECHO;
    term.c_lflag |= ISIG;
    tcsetattr(0, TCSANOW, &term);
    _status = true;
}

void 
m_input::recover()
{
    if(_status == false)
        return;

    //恢复
    tcsetattr(0, TCSANOW, &_oldterm);
    _status = false;
}

