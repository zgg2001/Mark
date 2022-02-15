# Mark
&emsp;&emsp;基于Linux由C/C++编写的任务计划管理工具。可以实现以组为单位、多用户的计划管理。</br>

## 快速了解
&emsp;&emsp;项目核心采用C/S架构，使用多线程构造多节点作为实现异步收发的基础。异步接收主要使用 `epoll` 实现，而异步发送则是通过维护一个发送 `task` 链表来实现。</br>
&emsp;&emsp;项目中实现了新的日志系统并二次封装了线程、信号量、MySql库等内容，从而便于在本项目中使用。</br>
&emsp;&emsp;项目服务端中已登录用户与未登录用户分别由在不同线程的不同节点负责，而已登录用户也会根据所属组进行组节点的二次分配，从而实现一定程度上线程的负载均衡。同时对于每一个连接的客户端，也会为其建立单独的客户端对象，其中储存了二级消息缓冲区以及连接信息等内容。</br>
&emsp;&emsp;项目客户端主要通过个人的另一个项目(交互式命令行) `NiceCmdline` 来实现与用户的交互。由于客户端也实现了多线程多节点异步收发，所以需要使用信号量进行线程间同步，来保证用户的使用体验。</br>
&emsp;&emsp;服务端与客户端之间的数据联络基于自定义报文协议实现，通过统一报头以及上文中提到的二级缓存区来处理半包、粘包及分包问题。

## 项目安装
### 1.安装NiceCmdline 
客户端需要使用此交互式命令行库 `NiceCmdline`与用户进行交互，需要首先安装。
```shell
$ git clone https://github.com/zgg2001/NiceCmdline
$ cd NiceCmdline/
$ sudo make
$ sudo make install
```
### 2.MySql相关配置
首先确保环境中MySql已启动。
```shell
$ ps -ef | grep mysqld
```
其次确保已安装MySql的开发包 `mysql-devel`。
```shell
$ sudo yum install mysql-devel #centos
```
最后在MySql中新建 `mark` 库。
```shell
mysql> create database mark;
```
### 3.安装Mark
简单配置后 `make` 即可。(初始管理员账号 root/123456)
```shell
$ git clone https://github.com/zgg2001/Mark
$ cd Mark
$ vim src/server/mark_server.cpp +20     #修改服务端监听端口
$ vim src/server/server/m_server.cpp +55 #修改数据库用户/密码
$ vim src/client/mark_client.cpp +17     #修改客户端连接的ip/端口
$ make
```
## 功能介绍
待补充...
## 作者
zgg2001
## 版权说明
该项目签署了 Apache-2.0 授权许可，详情请参阅 [LICENSE](https://github.com/zgg2001/Mark/blob/main/LICENSE)
