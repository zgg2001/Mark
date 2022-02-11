/*************************************************************************
	> File Name: m_client.cpp
	> Author: ZHJ
	> Remarks: 客户端类
	> Created Time: Fri 04 Feb 2022 09:19:41 AM CST
 ************************************************************************/

#include"m_client.h"
#include"m_cmd.h"

/* 静态成员初始化 */
m_client* m_client::_instance = nullptr;
std::mutex m_client::_lock;
m_client::deletor m_client::del;

m_client::m_client():
    _sock(INVALID_SOCKET),
    _status(false),
    _login_ret(-1),
    _operate_ret(-1),
    _cl(nullptr),
    _rnode(nullptr)
{
    _rnode = new m_recv_node(this);
    _snode = new m_send_node(this);
}

m_client::~m_client()
{
    DEBUG("Client ~m_client start");
    m_close();
    
    delete _rnode;
    delete _snode;
    close(_sock);
    
    if(_cl != nullptr)
    {
        cmdline_exit_free(_cl);
        _cl = nullptr;
    }
    DEBUG("Client ~m_client end");
}

int 
m_client::m_init()
{
    //创建socket
    if(_sock != INVALID_SOCKET)//存在socket
    {
        DEBUG("Client init warn -- socket:%d already exists", _sock);
        m_close();
    }
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == INVALID_SOCKET)
    {
        DEBUG("Client init faild -- %s", strerror(errno));
        return -1;
    }

    DEBUG("Client init(%d) success", _sock);
    return 0;
}

int 
m_client::m_connect(const char* ip, unsigned short port)
{
    //无效套接字
    if(_sock == INVALID_SOCKET)
    {
        DEBUG("Client bind warn -- client socket not initialized");
        m_init();
    }

    //配置
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;//IPV4
    _sin.sin_port = htons(port);//端口
    _sin.sin_addr.s_addr = inet_addr(ip);//IP

    //connect
    int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
    if(ret == SOCKET_ERROR)
    {
        DEBUG("Client connect(%d) ip:%s port:%d faild -- %s", _sock, ip, port, strerror(errno));
        close(_sock);
        return -1;
    }
    
    DEBUG("Client connect(%d) success", _sock);
    _status = true;
    return 0; 
}

void 
m_client::m_start()
{
    DEBUG("Client m_start start");
    _rnode->set_sockfd(_sock);
    _rnode->start();
    _snode->set_sockfd(_sock);
    _snode->start();
}

void
m_client::m_work()
{
    if(_status == false)
    {
        printf("connect... failed\n");
        return;
    }
    printf("connect... success\n");
   
    //登录
    if(m_login() == false)
    {
        printf("login... failed\n");
        return;
    }
    printf("login... success\n");

    //新建命令行
    _cl = cmdline_get_new(main_ctx, "Mark> "); 
    //开始交互工作
    cmdline_start_interact(_cl);
    //退出
    cmdline_exit_free(_cl);
    _cl = nullptr;
}

void 
m_client::m_close()
{
    DEBUG("Client m_close start");
    _rnode->close_node();
    _snode->close_node();
}

void
m_client::m_exit()
{
    if(_cl != nullptr)
        cmdline_quit(_cl);
    printf("\n-------------------------------------"
           "\nNetwork error: disconnect from server"
           "\n-------------------------------------\n");
    exit(1);
}

bool 
m_client::m_login()
{
    m_noecho noec;

    std::string name, passwd;
    printf("login as: ");
    std::cin >> name;
    for(int i = 0; i < 3; ++i)
    {
        //接收
        printf("%s's password: ", name.c_str());
        noec.noecho();
        std::cin >> passwd;
        printf("\n");
        noec.recover();
        passwd = m_md5::md5sum(passwd);
       
        //新增send task
        _snode->send_login_data(name, passwd);
        
        //阻塞等结果
        _login_ret = -1;
        _sem.wait();
        
        //ret
        if(_login_ret == 0)
        {
            return true;
        }
        printf("Access denied\n");
    }
    return false;
}

void 
m_client::m_login_wake(int ret)
{
    _login_ret = ret;
    _sem.wakeup();
}

void 
m_client::m_add_plan()
{
    std::string content = {}, remark = {}, stime = {};
    int nowtime, time = 0;
    m_input i;

    //当前时间
    std::time_t t = std::time(nullptr);
    std::tm *const pt = std::localtime(&t);
    nowtime = (1900 + pt->tm_year) * 10000 + (1 + pt->tm_mon) * 100 + pt->tm_mday;
    
    //接收输入
    i.input();
    printf("计划内容\n> ");
    getline(std::cin, content);
    while(content.size() == 0)
    {
        printf("error - 内容不能为空\n> ");
        getline(std::cin, content);
    }
    printf("计划备注\n> ");
    getline(std::cin, remark);
    printf("预计完成时间(>=%d)\n> ", nowtime);
    while(1) 
    {
        try 
        {
            getline(std::cin, stime);
            if(stime.size() != 0)
                time = std::stoi(stime);
            if(time < nowtime)
            {
                printf("WARN - 预计完成时间未设置\n");
                time = 0;
            }
            break;
        }
        catch(std::exception& invalid_argument) 
        {
            printf("error - 输入有误\n> ");
            continue;
        }
	}
    i.recover();

    //新增 send task
    _snode->send_add_plan_data(content, remark, nowtime, time);
    printf("add plan...");
    fflush(stdin);

    //阻塞等结果
    _operate_ret = -1;
    _sem.wait();
    
    //ret
    if(_operate_ret == 0)
    {
        printf(" success\n");
        return;
    }
    printf(" failed\n");
}

void 
m_client::m_del_plan(int id)
{
    std::string input;
    m_input i;

    i.input();
    printf("确认删除id为%d的计划吗? (y/n): ", id);
    getline(std::cin, input);
    i.recover();
    if(input == "y" || input == "yes" || input == "Y")
    {
        //新增send task
        _snode->send_delete_plan_data(id);
        printf("delete plan...");
        fflush(stdin);
        
        //阻塞等结果
        _operate_ret = -1;
        _sem.wait();
        
        //ret
        if(_operate_ret == 1)
            printf(" success\n");
        else if(_operate_ret == -1)
            printf(" failed -- 计划不存在\n");
        else if(_operate_ret == -2)
            printf(" failed -- 权限不足\n");
        else
            printf(" failed -- 未知错误\n");
    }
}

void 
m_client::m_upd_plan(int mode, int id)
{
    std::string content = {}, remark = {};
    int time = 0, status = 0;
    m_input i;
    
    i.input();
    if(mode == 0)//时间
    {
        _upd_plan_t(time);
        _snode->send_update_time_data(id, time);
    }
    else if(mode == 1)//状态
    {
        _upd_plan_s(status);
        _snode->send_update_status_data(id, status);
    }
    else//内容
    {
        _upd_plan_c(content, remark);
        _snode->send_update_content_data(id, content, remark);
    }
    i.recover();

    //阻塞等结果
    printf("update plan...");
    fflush(stdin);
    _operate_ret = -1;
    _sem.wait();
    
    //ret
    if(_operate_ret == 1)
        printf(" success\n");
    else if(_operate_ret == -1)
        printf(" failed -- 计划不存在\n");
    else if(_operate_ret == -2)
        printf(" failed -- 权限不足\n");
    else
        printf(" failed -- 未知错误\n");
}

void 
m_client::_upd_plan_t(int& time)
{
    std::string stime = {};
    std::time_t t = std::time(nullptr);
    std::tm *const pt = std::localtime(&t);
    int nowtime = (1900 + pt->tm_year) * 10000 + (1 + pt->tm_mon) * 100 + pt->tm_mday;
    
    printf("更新预计完成时间(>=%d)\n> ", nowtime);
    while(1) 
    {
        try 
        {
            getline(std::cin, stime);
            if(stime.size() != 0)
                time = std::stoi(stime);
            if(time < nowtime)
            {
                printf("INFO - 更新为不设置\n");
                time = 0;
            }
            break;
        }
        catch(std::exception& invalid_argument) 
        {
            printf("error - 输入有误\n> ");                
            continue;
        }
    } 
}

void 
m_client::_upd_plan_s(int& status)
{
    std::string input = {};
    printf("更新计划状态\n1-搁置 2-进行中 3-已完成\n> ");
    while(1) 
    {
        try 
        {
            getline(std::cin, input);
            status = std::stoi(input) - 1;
            if(status < 0 || status > 2)
            {
                printf("error - 输入有误\n> ");
                continue;
            }
            break;
        }
        catch(std::exception& invalid_argument) 
        {
            printf("error - 输入有误\n> ");
            continue;
        }
    } 
}

void 
m_client::_upd_plan_c(std::string& content, std::string& remark)
{
    printf("更新计划内容\n> ");
    getline(std::cin, content);
    while(content.size() == 0)
    {
        printf("error - 内容不能为空\n> ");
        getline(std::cin, content);
    }
    printf("更新计划备注\n> ");
    getline(std::cin, remark);    
}

void
m_client::m_operate_wake(int ret)
{
    _operate_ret = ret;
    _sem.wakeup();
}



