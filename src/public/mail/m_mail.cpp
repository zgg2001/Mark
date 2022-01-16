/*************************************************************************
	> File Name: m_mail.cpp
	> Author: ZHJ
	> Remarks: 简单封装的邮件类 
	> Created Time: Sun 16 Jan 2022 05:06:40 PM CST
 ************************************************************************/

#include<sys/types.h>
#include<stdarg.h>
#include"m_mail.h"

/* 静态成员初始化 */
m_mail* m_mail::_instance = nullptr;
std::mutex m_mail::_lock;
m_mail::deletor m_mail::del;

/* 邮件类相关定义 */
m_mail::m_mail():
    _state(true)
{
}

void 
m_mail::send_mail(const char* address, const char* title, const char* format, ...)
{
    char mail_buf[MAIL_BUF_MAX_SIZE];
    int mail_len = 0;

    //前段
    mail_len = snprintf(mail_buf, MAIL_BUF_MAX_SIZE, "echo -e \'");

    //可变参
    va_list arg_ptr;
    va_start(arg_ptr, format);
    mail_len += vsnprintf(mail_buf + mail_len, MAIL_BUF_MAX_SIZE - mail_len, format, arg_ptr);
    va_end(arg_ptr);

    //后段
    mail_len += snprintf(mail_buf + mail_len, MAIL_BUF_MAX_SIZE - mail_len, "\' | mail -s \'%s\' %s", title, address);

    //echo -e '内容' | mail -s '标题' 地址
    popen(mail_buf, "r");
    
    /*
    FILE* pipe = popen(mail_buf, "r");
    if(!pipe)
        return;
    */
}

