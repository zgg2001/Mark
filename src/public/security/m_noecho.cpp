/*************************************************************************
	> File Name: m_noecho.cpp
	> Author: ZHJ
	> Remarks: 取消反显 方便输密码
	> Created Time: Sun 06 Feb 2022 08:04:55 PM CST
 ************************************************************************/

#include"m_noecho.h"

m_noecho::m_noecho():
    _status(false)
{

}

m_noecho::~m_noecho()
{
    if(_status == true)
    {
        recover();
    }
}

void 
m_noecho::noecho()
{
    struct termios term;
    
    //获取/保存当前终端设置
    tcgetattr(0, &term);
    memcpy(&_oldterm, &term, sizeof(struct termios));

    //设置noecho
    term.c_lflag &= ~(ECHO);
    tcsetattr(0, TCSANOW, &term);
    _status = true;
}

void 
m_noecho::recover()
{
    if(_status == false)
        return;

    //恢复
    tcsetattr(0, TCSANOW, &_oldterm);
    _status = false;
}

