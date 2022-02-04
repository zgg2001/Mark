/*************************************************************************
	> File Name: m_db_mgr.cpp
	> Author: ZHJ
	> Remarks: 基于m_db类进行数据库管理
	> Created Time: Thu 03 Feb 2022 08:33:13 PM CST
 ************************************************************************/

#include"m_db_mgr.h"
#include"../server/m_server.h"

m_db_mgr::m_db_mgr(m_server* server):
    _server(server)
{

}

m_db_mgr::~m_db_mgr()
{

}

int 
m_db_mgr::init(const char* host, const char* user,
               const char* passwd)
{
    int ret;
    int num;

    //储存sql命令执行结果
    std::vector<char**>res;
    int row;
    int field;
    
    //无所属server
    if(_server == nullptr)
    {
        ERROR("m_db_mgr init error -- server is nullptr");
        return -4;
    }

    //连接数据库
    ret = _db.connect(host, user, passwd, "mark");
    if(ret == -1)
    {
        ERROR("m_db_mgr init error -- mysql_real_connect() error");
        return -1; 
    }
    else if(ret == -2)
    {  
        ERROR("m_db_mgr init error -- mysql_init() error");
        return -2;  
    }

    //查看数据库mark中表数量是否正常(正常为5)
    ret = _db.exec(SQL_GET_TABLE_NUM, &res, &row, &field);
    num = atoi(res[0][0]);
    DEBUG("m_db_mgr init 查表数量: %d", num);
    if(num == 0)//第一次使用 建表初始化
    {
        INFO("m_db_mgr init 建表初始化 start");
        //建表
        _db.exec(SQL_CREATE_PLAN_TBL, &res, &row, &field);
        _db.exec(SQL_CREATE_PLAN_INFO_TBL, &res, &row, &field);
        _db.exec(SQL_CREATE_GROUP_TBL, &res, &row, &field);
        _db.exec(SQL_CREATE_USER_GROUP_TBL, &res, &row, &field);
        _db.exec(SQL_CREATE_USER_TBL, &res, &row, &field);
        
        //root用户 初始密码123456
        _db.exec("insert into mark_user values (1, 'root', 'e10adc3949ba59abbe56e057f20f883e', '')", &res, &row, &field); 
        return 1;
    }
    else if(num == 5)//数量正常 读表反序列化
    {
        INFO("m_db_mgr init 数据反序列化 start");
        //读组
        _db.exec(SQL_SELECT_GROUP_TBL, &res, &row, &field);
        for(int i = 0; i < row; ++i)
        {
            int gid = atoi(res[i][0]);
            int aid = atoi(res[i][2]);
            _server->new_group_node(gid, res[i][1], aid);
            DEBUG("new_group_node: gid:%d gname:%s aid:%d", gid, res[i][1], aid);
        }
        
        //读用户
        _db.exec(SQL_SELECT_USER_GROUP_TBL, &res, &row, &field);
        for(int i = 0; i < row; ++i)
        {
            int uid = atoi(res[i][0]);
            int gid = atoi(res[i][1]);
            _server->new_group_user(gid, uid, res[i][2], res[i][3]);
            DEBUG("new_group_user: gid:%d uid:%d name:%s", gid, uid, res[i][2]);
        }
        
        //读计划
        _db.exec(SQL_SELECT_PLAN_INFO_TBL, &res, &row, &field);
        for(int i = 0; i < row; ++i)
        {
            int gid = atoi(res[i][2]);//计划的所属组id
            int pid = atoi(res[i][3]);//组内id
            int uid = atoi(res[i][1]);//所属用户id
            int status = atoi(res[i][5]);//计划状态
            int id = atoi(res[i][0]);//计划的主id
            int cid = atoi(res[i][4]);//计划的创建者id
            int ctime = atoi(res[i][6]);//创建计划的时间
            int ptime = atoi(res[i][7]);//计划完成的时间
            _server->new_group_plan(gid, pid, uid, status,
                                    id, cid,
                                    ctime, ptime,
                                    res[i][8], res[i][9]);
            DEBUG("new_group_plan: gid:%d pid:%d uid:%d", gid, pid, uid);
        }
    }
    else//表数量异常
    {
        ERROR("m_db_mgr init error -- table count(%d) error", num);
        return -3;
    }

    return 0;
}
