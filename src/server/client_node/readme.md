## m_client_node
&emsp;&emsp;此类为客户端节点类。</br>
&emsp;&emsp;每一个socket连接后，都会创建一个 `m_client_node` 来储存/管理这个连接。</br>
&emsp;&emsp;其中主要储存了：此连接的所属组id以及用户id、此连接的心跳计时，以及此连接的二级缓冲区。作用点分别为节点的分配、保活机制、以及对半包粘包问题的处理。
