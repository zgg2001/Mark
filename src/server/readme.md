## client_node
&emsp;&emsp;客户端节点部分。</br>
&emsp;&emsp;每一个客户端连接都对应一个客户端节点，其中储存了此连接的id部分、心跳部分、二级缓冲区部分等。
## data
&emsp;&emsp;数据结构部分。</br>
&emsp;&emsp;主要有三种数据结构：计划、用户、组。
## database
&emsp;&emsp;数据库管理部分。</br>
&emsp;&emsp;负责数据库的序列号和反序列化等工作。
## server
&emsp;&emsp;服务端类部分</br>
&emsp;&emsp;一个服务端实例包含一个登录节点和若干组节点与任务节点。
## server_node
&emsp;&emsp;服务端节点部分</br>
&emsp;&emsp;主要为登录节点、组节点以及任务节点。
