/*************************************************************************
	> File Name: m_group.cpp
	> Author: ZHJ
	> Remarks: 数据结构 -- 组
	> Created Time: Tue 25 Jan 2022 09:30:37 PM CST
 ************************************************************************/

#include"m_group.h"

m_group::m_group(int g_id, int a_id):
    group_id(g_id),
    plan_max_id(0),
    admin_id(a_id)
{

}

int 
m_group::addplan(int u_id, int status,
               int plan_id, int user_id,
               int create_t, int plan_t,
               const char* content_str, const char* remark_str)
{
    //计算新结点的组内id
    int p_id = plan_max_id + 1;
    return _add_plan(p_id, u_id, status,
                     plan_id, user_id,
                     create_t, plan_t,
                     content_str, remark_str);
}

int 
m_group::addplan(int p_id, int u_id, int status,
               int plan_id, int user_id,
               int create_t, int plan_t,
               const char* content_str, const char* remark_str)
{
    return _add_plan(p_id, u_id, status,
                     plan_id, user_id,
                     create_t, plan_t,
                     content_str, remark_str);
}

int 
m_group::_add_plan(int p_id, int u_id, int status,
                 int plan_id, int user_id,
                 int create_t, int plan_t,
                 const char* content_str, const char* remark_str)
{
    //用户不存在
    if(users.count(u_id) == 0)
        return -1;
    //计划编号重复
    if(plans.count(p_id) != 0)
        return -2;
    //添加计划节点
    plans.insert({ p_id, 
                 { plan_id, user_id, 
                   create_t, plan_t,
                   content_str, remark_str } });
    plans[p_id].status = static_cast<plan_status>(status);
    //将此计划节点登记至所属用户下
    users[u_id].addplan(status, p_id);
    //更新最大组内id
    plan_max_id = std::max(plan_max_id, p_id);
    return p_id;
}

int
m_group::delplan(int u_id, int p_id)
{
    if(users.count(u_id) == 0 || plans.count(p_id) == 0)
        return -1;
    if(users[u_id].delplan(p_id) == false && u_id != 1 && u_id != admin_id)
        return -2;
    return plans.erase(p_id) ? 1 : 0;
}

int 
m_group::updplan_t(int u_id, int p_id, int time)
{
    if(users.count(u_id) == 0 || plans.count(p_id) == 0)
        return -1;
    if(users[u_id].search(p_id) == false && u_id != 1 && u_id != admin_id)
        return -2;
    //计划
    plans[p_id].plan_time = time;
    return 1;
}

int 
m_group::updplan_s(int u_id, int p_id, int status)
{
    if(users.count(u_id) == 0 || plans.count(p_id) == 0)
        return -1;
    if(users[u_id].search(p_id) == false && u_id != 1 && u_id != admin_id)
        return -2;
    //用户
    if(users[u_id].updplan(p_id, status) == false)
        return -3;
    //计划
    plans[p_id].status = static_cast<plan_status>(status);
    return 1;
}

int 
m_group::updplan_c(int u_id, int p_id,
                 const char* content_str, const char* remark_str)
{
    if(users.count(u_id) == 0 || plans.count(p_id) == 0)
        return -1;
    if(users[u_id].search(p_id) == false && u_id != 1 && u_id != admin_id)
        return -2;
    //计划
    plans[p_id].change(content_str, remark_str);
    return 1;
}

bool 
m_group::adduser(int u_id, std::string name, std::string email)
{
    if(users.count(u_id))
        return false;
    //int, user
    users.insert({ u_id, { u_id, name, email } });
    return true;
}

bool 
m_group::deluser(int u_id)
{
    return users.erase(u_id);
}



