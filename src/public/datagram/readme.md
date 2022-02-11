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
    CMD_C2S_HEART,      //心跳 client to server
    CMD_S2C_HEART,      //心跳 server to client
    CMD_LOGIN,          //登录 
    CMD_LOGIN_RESULT,   //登录结果 
    CMD_ADD_PLAN,       //新增计划
    CMD_DEL_PLAN,       //删除计划
    CMD_UPD_PLAN_T,     //改动计划 - time
    CMD_UPD_PLAN_S,     //改动计划 - status
    CMD_UPD_PLAN_C,     //改动计划 - content
    CMD_OPERATE_RESULT  //增/删/改结果
};
```
