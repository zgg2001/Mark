/*************************************************************************
	> File Name: plan.cpp
	> Author: ZHJ
	> Remarks: 数据结构 -- 计划
	> Created Time: Mon 24 Jan 2022 07:50:07 PM CST
 ************************************************************************/

#include<cstdio>
#include<cstring>
#include"plan.h"

plan::plan(int plan_id, int user_id,
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
        snprintf(content, plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(remark, plan::SIZE, "%s", remark_str);
}

void 
plan::change(const char* content_str, const char* remark_str)
{
    if(content_str == nullptr)
        return;
    snprintf(content, plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(remark, plan::SIZE, "%s", remark_str);
}

