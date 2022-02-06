# datagram
&emsp;&emsp;数据报文部分。</br>
&emsp;&emsp;一个完整的数据报文由报头 `struct header` 和其数据部分组成。

报头的数据结构:
```
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
```
目前报文的类型:
```
/*
* 命令(报文)类型
*/
enum cmd
{
    CMD_LOGIN,          //登录 
    CMD_LOGIN_RESULT,   //登录结果 
};
```
