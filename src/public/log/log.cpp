/*************************************************************************
	> File Name: log.cpp
	> Author: ZHJ
	> Remarks: 日志类
	> Created Time: Wed 12 Jan 2022 06:52:47 PM CST
 ************************************************************************/

#include<unistd.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<fcntl.h> 
#include<stdarg.h>
#include<cstring>
#include"log.h"

/* 获取tid */
pid_t gettid()
{
    return syscall(__NR_gettid);
}

/* 静态成员初始化 */
log* log::_instance = nullptr;
std::mutex log::_lock;
log::deletor log::del;

/* 日志类相关定义 */
log::log():
    _path_state(false),
    _level(INFO)
{
    _pid = getpid();
}

void 
log::set_path(const char* log_dir, const char* log_filename,int level)
{
    //lock
    std::lock_guard<std::mutex>lock(_lock);
   
    //path
    if(!log_dir || !log_filename)
    {
        _path_state = false;
        fprintf(stderr, "log_dir or log_filename is empty\n");
    }
    else
    {
        snprintf(_log_dir, LOG_DIR_MAX_SIZE, "%s", log_dir);
        snprintf(_log_filename, LOG_FILENAME_MAX_SIZE, "%s", log_filename);
        
        //创建log_dir并检测其是否 存在 且 可写
        mkdir(_log_dir, LOG_DIR_PERMISSION);
        if(access(_log_dir, F_OK | W_OK) == -1)
        {
            _path_state = false;
            fprintf(stderr, "log_dir: %s error: %s\n", _log_dir, strerror(errno));
        }
        else
        {
            _path_state = true;
            snprintf(_log_path, LOG_DIR_MAX_SIZE + LOG_FILENAME_MAX_SIZE + 1, "%s/%s", log_dir, log_filename);
        }
    }

    //配置level
    if(level <= FATAL)
    {
        _level = FATAL;
    }
    else if(level >= TRACE)
    {
        _level = TRACE;
    }
    else
    {
        _level = level;
    }
}

void 
log::set_level(int level)
{
    //lock
    std::lock_guard<std::mutex>lock(_lock);

    if(level <= FATAL)
    {
        _level = FATAL;
    }
    else if(level >= TRACE)
    {
        _level = TRACE;
    }
    else
    {
        _level = level;
    } 
}

void 
log::log_write(const char* prev_str, const char* format, ...)
{
    char log_buf[LOG_BUF_MAX_SIZE];
    int log_len = 0;
    
    struct timeval now_time;
    time_t tt;
    tm *temp;

    //时间
    gettimeofday(&now_time, NULL);
    tt = now_time.tv_sec;
    temp = localtime(&tt);
   
    //前缀拼接
    log_len = snprintf(log_buf, LOG_BUF_MAX_SIZE, "%s[", prev_str);
    log_len += snprintf(log_buf + log_len, LOG_BUF_MAX_SIZE - log_len, "%04d-%02d-%02d %02d:%02d:%02d]", 
                        temp->tm_year + 1900, temp->tm_mon + 1, temp->tm_mday, temp->tm_hour, temp->tm_min, temp->tm_sec);
    
    //可变参
    va_list arg_ptr;
    va_start(arg_ptr, format);
    log_len += vsnprintf(log_buf + log_len, LOG_BUF_MAX_SIZE - log_len, format, arg_ptr);
    va_end(arg_ptr);

    //输出
    if(_path_state == true)
    { 
        int fd = open(_log_path, O_WRONLY | O_APPEND | O_CREAT, LOG_FILE_PERMISSION);
        write(fd, log_buf, log_len);
        close(fd);
    }
    else
    {
        write(1, log_buf, log_len);
    }
}




