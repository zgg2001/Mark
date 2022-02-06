/*************************************************************************
	> File Name: m_datagram.h
	> Author: ZHJ
	> Remarks: 数据报文
	> Created Time: Sun 06 Feb 2022 12:00:35 PM CST
 ************************************************************************/

#ifndef _M_DATAGRAM_H_
#define _M_DATAGRAM_H_

/*
* 命令(报文)类型
*/
enum cmd
{
    CMD_LOGIN,          //登录 
    CMD_LOGIN_RESULT,   //登录结果 
};

/*
* 报头
*    cmd: 报文命令
* length: 报文长度
*/
struct header
{
    short cmd;
    short length;
};

/*
* 登录报文
*/
struct login : public header
{
    login()
    {
        this->cmd = CMD_LOGIN;
        this->length = sizeof(login);
    }
    char username[34];//用户名
    char password[34];//密码
};

/*
* 登录结果
*/
struct login_result : public header
{
    login_result()
    {
        this->cmd = CMD_LOGIN_RESULT;
        this->length = sizeof(login_result);
    }
    int result;
};

#endif
