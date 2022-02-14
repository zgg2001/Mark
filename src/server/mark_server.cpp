/*************************************************************************
	> File Name: mark_server.cpp
	> Author: ZHJ
	> Remarks: Mark服务端
	> Created Time: Thu 27 Jan 2022 07:05:22 PM CST
 ************************************************************************/

#include<iostream>
#include<server/server/m_server.h>
#include<server/server_node/m_login_node.h>

int main()
{
    //SET_LEVEL(DEBUG);
    SET_LEVEL(INFO);
    //SET_LOG_PATH("./", "Mark.log", INFO);

    m_server s;
    s.m_init();
    s.m_bind(NULL, 8888);
    s.m_listen(5);
    s.m_start();
    s.m_work();
}
