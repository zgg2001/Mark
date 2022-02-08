/*************************************************************************
	> File Name: log.h
	> Author: ZHJ
	> Remarks: 日志类 
	> Created Time: Wed 12 Jan 2022 06:52:24 PM CST
 ************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include<sys/types.h>
#include<mutex>

extern pid_t gettid();

/* 日志等级 */
enum LOG_LEVEL
{
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE,
};

/* 日志配置宏 */
#define LOG_DIR_MAX_SIZE 512
#define LOG_FILENAME_MAX_SIZE 128
#define LOG_DIR_PERMISSION 0777
#define LOG_BUF_MAX_SIZE 1024
#define LOG_FILE_PERMISSION 0666

/* 日志类 */
class log
{
public:
    static log* get_ins()
    {
        if(_instance == nullptr)
        {
            std::lock_guard<std::mutex>lock(_lock);
            if(_instance == nullptr)
            {
                _instance = new log();
            }
        }
        return _instance;
    }

    /*
    * 设置日志储存路径与等级
    *
    * log_path: 日志储存的目录
    * log_name: 日志文件名
    *    level: 日志等级
    */
    void set_path(const char* log_dir, const char* log_filename,int level);
    
    void set_level(int level);

    int get_level() const { return _level; }

    /*
    * 写日志
    *
    * prev_str: 日志前缀
    *   format: 可变参
    */
    void log_write(const char* prev_str, const char* format, ...);

private:
    log();
    log(const log&) = delete;
    log& operator= (const log&) = delete;

    //pid
    pid_t _pid;
    
    //日志相关
    char _log_dir[LOG_DIR_MAX_SIZE];
    char _log_filename[LOG_FILENAME_MAX_SIZE];
    char _log_path[LOG_DIR_MAX_SIZE + LOG_FILENAME_MAX_SIZE + 1];
    bool _path_state;//路径设置 true:输出至路径 false:标准输出流
    int _level;//等级

    //实例相关
    static log* _instance;
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

/* 日志类调用宏 */
#define SET_LOG_PATH(log_dir, log_filename, level)                  \
    do                                                              \
    {                                                               \
        log::get_ins()->set_path(log_dir, log_filename, level);     \
    }while(0)

#define SET_LEVEL(level)                                            \
    do                                                              \
    {                                                               \
        log::get_ins()->set_level(level);                           \
    }while(0)

#define FATAL(fmt, args...)                                         \
    do                                                              \
    {                                                               \
        log::get_ins()->log_write("[FATAL]",                        \
                                  "[%u]%s:%d(%s): " fmt "\n",       \
        gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);        \
    }while(0)

#define ERROR(fmt, args...)                                         \
    do                                                              \
    {                                                               \
        if(log::get_ins()->get_level() >= ERROR)                    \
        {                                                           \
            log::get_ins()->log_write("[ERROR]",                    \
                                      "[%u]%s:%d(%s): " fmt "\n",   \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);    \
        }                                                           \
    }while(0)

#define WARN(fmt, args...)                                          \
    do                                                              \
    {                                                               \
        if(log::get_ins()->get_level() >= WARN)                     \
        {                                                           \
            log::get_ins()->log_write("[WARN]",                     \
                                      "[%u]%s:%d(%s): " fmt "\n",   \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);    \
        }                                                           \
    }while(0)

#define INFO(fmt, args...)                                          \
    do                                                              \
    {                                                               \
        if(log::get_ins()->get_level() >= INFO)                     \
        {                                                           \
            log::get_ins()->log_write("[INFO]",                     \
                                      "[%u]%s:%d(%s): " fmt "\n",   \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);    \
        }                                                           \
    }while(0)

#define DEBUG(fmt, args...)                                         \
    do                                                              \
    {                                                               \
        if(log::get_ins()->get_level() >= DEBUG)                    \
        {                                                           \
            log::get_ins()->log_write("[DEBUG]",                    \
                                      "[%u]%s:%d(%s): " fmt "\n",   \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);    \
        }                                                           \
    }while(0)

#define TRACE(fmt, args...)                                         \
    do                                                              \
    {                                                               \
        if(log::get_ins()->get_level() >= TRACE)                    \
        {                                                           \
            log::get_ins()->log_write("[TRACE]",                    \
                                      "[%u]%s:%d(%s): " fmt "\n",   \
            gettid(), __FILE__, __LINE__, __FUNCTION__, ##args);    \
        }                                                           \
    }while(0)

#endif
