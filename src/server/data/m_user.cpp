/*************************************************************************
	> File Name: m_user.cpp
	> Author: ZHJ
	> Remarks: 数据结构 -- 用户 
	> Created Time: Tue 25 Jan 2022 09:31:03 PM CST
 ************************************************************************/

#include"m_user.h"

m_user::m_user(int u_id, std::string name, std::string email):
    user_id(u_id),
    username(name),
    mail(email)
{
    plans.assign(3, std::list<int>{});
}

void 
m_user::addplan(plan_status status, int id)
{
    int sta = static_cast<int>(status);
    if(sta < 0 || sta > 2)
        return;
    plans[sta].push_back(id);
}

void 
m_user::addplan(int status, int id)
{
    if(status < 0 || status > 2)
        return;
    plans[status].push_back(id);
}

bool 
m_user::delplan(int id)
{
    for(int i : {1, 0, 2})//进行中/搁置/已完成
    {
        auto iter = std::find(plans[i].begin(), plans[i].end(), id);
        if(iter != plans[i].end())
        {
            //删除
            plans[i].erase(iter);
            return true;
        }
    }
    return false;
}

bool 
m_user::updplan(int id, plan_status newsta)
{
    int sta = static_cast<int>(newsta);
    if(sta < 0 || sta > 2)
        return false;
    return _update_plan(id, sta); 
}

bool 
m_user::updplan(int id, int newsta)
{
    if(newsta < 0 || newsta > 2)
        return false;
    return _update_plan(id, newsta); 
}

bool 
m_user::_update_plan(int id, int newsta)
{
    for(int i : {1, 0, 2})//进行中/搁置/已完成
    {
        auto iter = std::find(plans[i].begin(), plans[i].end(), id);
        if(iter != plans[i].end())
        {
            //状态没有改变直接返回
            if(newsta == i)
                return true;
            //删除并重新加入
            plans[i].erase(iter);
            addplan(newsta, id);
            return true;
        }
    }
    return false;
}

bool 
m_user::search(int id)
{
    for(int i : {1, 0, 2})//进行中/搁置/已完成
    {
        auto iter = std::find(plans[i].begin(), plans[i].end(), id);
        if(iter != plans[i].end())
        {
            return true;
        }
    }
    return false;
}


