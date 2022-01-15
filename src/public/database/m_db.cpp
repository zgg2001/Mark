/*************************************************************************
	> File Name: m_db.cpp
	> Author: ZHJ
	> Remarks: 连接MySql数据库
	> Created Time: Sat 15 Jan 2022 04:40:28 PM CST
 ************************************************************************/

#include"m_db.h"

m_db::m_db():
    _mysql(nullptr)
{
    _mysql = mysql_init(nullptr);
    if(_mysql == nullptr)
    {
        ERROR("MySql init error");
        exit(1);
    }
    DEBUG("MySql init success");
}

m_db::~m_db()
{
    if(_mysql != nullptr)
    {
        mysql_close(_mysql);
    }
}

bool 
m_db::connect(const char* host, const char* user, 
              const char* passwd, const char* db_name)
{
    //lock
    std::lock_guard<std::mutex>lock(_lock);

    _mysql = mysql_real_connect(_mysql, host, user, passwd, db_name, 0, NULL, 0);
    if(_mysql == nullptr)
    {
        ERROR("MySql connect error");
        return false;
    }
    DEBUG("MySql connect success");
    return true;
}

int 
m_db::exec(const char* sql, std::vector<char**>* res, int* num_rows, int* num_fields)
{
    MYSQL_RES* result;
    MYSQL_ROW row;
    
    if(mysql_query(_mysql, sql))
    {
        ERROR("MySql exec error: %s", mysql_error(_mysql));
        return -1;
    }
    
    DEBUG("MySql exec success");
    
    //获取结果并储存
    result = mysql_store_result(_mysql);
    if(result && res && num_rows && num_fields)
    {
        res->clear();
        //行列
        *num_rows = mysql_num_rows(result);
        *num_fields = mysql_num_fields(result);
        //结果
        for(int i = 0; i < *num_rows; ++i)
        {
            row = mysql_fetch_row(result);
            if(row < 0)
                break;
            res->push_back(row);
        }
        DEBUG("MySql exec have result return");
        return 0;
    }

    DEBUG("MySql exec no result return");
    return 1;
}


