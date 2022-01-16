/*************************************************************************
	> File Name: m_mail.h
	> Author: ZHJ
	> Remarks: 简单封装的邮件类
	> Created Time: Sun 16 Jan 2022 05:06:16 PM CST
 ************************************************************************/

#ifndef _M_MAIL_H_
#define _M_MAIL_H_

#include<mutex>

#define MAIL_BUF_MAX_SIZE 2048

class m_mail
{
public:
    static m_mail* get_ins()
    {
        if(_instance == nullptr)
        {
            std::lock_guard<std::mutex>lock(_lock);
            if(_instance == nullptr)
            {
                _instance = new m_mail();
            }
        }
        return _instance;
    }
    
    /*
    * 设置邮件系统的状态
    * true  为开启
    * false 为关闭
    */
    void set_state(bool state) { _state = state; }

    /*
    * 获取邮件系统状态
    */
    bool get_state() const { return _state; }

    /*
    * 发送邮件
    *
    * address: 目的邮箱
    *   title: 邮件标题
    *  format: 邮件内容(可变参)
    */
    void send_mail(const char* address, const char* title, const char* format, ...);

private:
    m_mail();
    m_mail(const m_mail&) = delete;
    m_mail& operator= (const  m_mail&) = delete;

    //邮件系统状态
    bool _state;

    //实例相关
    static m_mail* _instance;
    static std::mutex _lock;

private:
    //删除器 退出自动释放实例
    class deletor
    {
    public:
        ~deletor()
        {
            if(_instance != nullptr)
                delete _instance;
        }
    };
    static deletor del;
};

/* 邮件类调用宏 */
#define SEND_MAIL(address, title, format, args...)                          \
    do                                                                      \
    {                                                                       \
        if(m_mail::get_ins()->get_state())                                  \
            m_mail::get_ins()->send_mail(address, title, format, ##args);   \
    }while(0)

#define SET_MAIL_STATE(state)                                               \
    do                                                                      \
    {                                                                       \
        m_mail::get_ins()->set_state(state);                                \
    }while(0)

#endif
