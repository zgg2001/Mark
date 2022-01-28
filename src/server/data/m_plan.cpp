/*************************************************************************
	> File Name: plan.cpp
	> Author: ZHJ
	> Remarks: 数据结构 -- 计划
	> Created Time: Mon 24 Jan 2022 07:50:07 PM CST
 ************************************************************************/

#include<cstdio>
#include<cstring>
#include"m_plan.h"

m_plan::m_plan(int plan_id, int user_id,
               int create_t, int plan_t,
               const char* content_str, const char* remark_str):
    id(plan_id),
    create_user(user_id),
    status(plan_status::shelve),
    create_time(create_t),
    plan_time(plan_t)
{
    //内容与备注
    memset(content, 0, sizeof(content));
    memset(remark, 0, sizeof(remark));
    if(content_str != nullptr)
        snprintf(content, m_plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(remark, m_plan::SIZE, "%s", remark_str);
}

void 
m_plan::change(const char* content_str, const char* remark_str)
{
    if(content_str == nullptr)
        return;
    snprintf(content, m_plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(remark, m_plan::SIZE, "%s", remark_str);
}

