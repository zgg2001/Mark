/*************************************************************************
	> File Name: plan.cpp
	> Author: ZHJ
	> Remarks: 数据结构 -- 计划
	> Created Time: Mon 24 Jan 2022 07:50:07 PM CST
 ************************************************************************/

#include<cstdio>
#include<cstring>
#include"m_plan.h"

m_plan::m_plan(int plan_id, int now_id, int user_id,
               int create_t, int plan_t,
               const char* content_str, const char* remark_str):
    id(plan_id),
    now_user(now_id),
    create_user(user_id),
    status(plan_status::shelve),
    create_time(create_t),
    plan_time(plan_t)
{
    memset(_content, 0, sizeof(_content));
    memset(_remark, 0, sizeof(_remark));
    if(content_str != nullptr)
        snprintf(_content, m_plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(_remark, m_plan::SIZE, "%s", remark_str);
}

m_plan::m_plan(const m_plan& p)
{
    id = p.id.load();
    now_user = p.now_user.load();
    create_user = p.create_user.load();
    status = p.status.load();
    create_time = p.create_time.load();
    plan_time = p.plan_time.load();
    memset(_content, 0, sizeof(_content));
    memset(_remark, 0, sizeof(_remark));
    snprintf(_content, m_plan::SIZE, "%s", p._content);
    snprintf(_remark, m_plan::SIZE, "%s", p._remark);
}

void 
m_plan::change(const char* content_str, const char* remark_str)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(content_str == nullptr)
        return;
    snprintf(_content, m_plan::SIZE, "%s", content_str);
    if(remark_str != nullptr)
        snprintf(_remark, m_plan::SIZE, "%s", remark_str);
}

const char* 
m_plan::get_content()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return (const char*)_content;
}

const char* 
m_plan::get_remark()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return (const char*)_remark;
}






