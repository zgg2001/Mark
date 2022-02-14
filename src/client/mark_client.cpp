/*************************************************************************
	> File Name: mark_client.cpp
	> Author: ZHJ
	> Remarks: Mark客户端
	> Created Time: Fri 04 Feb 2022 09:12:35 AM CST
 ************************************************************************/

#include<iostream>
#include<client/client/m_client.h>

int main()
{
    //SET_LEVEL(DEBUG);
    //SET_LEVEL(INFO);
    
    m_client::ins()->m_init();
    m_client::ins()->m_connect("xxx.xxx.xxx.xxx", 8888);
    m_client::ins()->m_start();
    m_client::ins()->m_work();
}


