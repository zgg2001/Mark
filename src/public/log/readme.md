# log
&emsp;&emsp;此类为自实现日志类。</br>
&emsp;&emsp;其中实现了六种日志等级，以及日志输出的等级设置/重定向。

**使用说明:**

```
enum LOG_LEVEL                                             
{                                                           
    FATAL,                                                  
    ERROR,                                                  
    WARN,                                                   
    INFO,                                                   
    DEBUG,                                                  
    TRACE,                                                  
};  
```
如上为支持的日志输入等级。
```
//SET_LOG_PATH(log_dir, log_filename, level)
SET_LOG_PATH("./", "Mark.log", INFO);

//SET_LEVEL(level)
SET_LEVEL(DEBUG);
```
如上为设置日志输出路径和输出等级。
```
FATAL("test - %d", 1);
ERROR("test - %d", 2);
WARN("test - %d", 3);
INFO("test - %d", 4);
DEBUG("test - %d", 5);
TRACE("test - %d", 6);
```
如上为六种日志输出的调用。
