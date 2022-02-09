/*************************************************************************
	> File Name: m_group.h
	> Author: ZHJ
	> Remarks: 数据结构 -- 组
	> Created Time: Tue 25 Jan 2022 09:30:25 PM CST
 ************************************************************************/

#ifndef _M_GROUP_H_
#define _M_GROUP_H_

#include<map>
#include"m_user.h"

/*
* 数据结构 -- 组
* 每一个实例就是一个组
* 组中储存了所属于此组的计划和用户
*/
class m_group
{
public:
    m_group(int g_id, int a_id);
    m_group() = default;
    m_group(const m_group&) = default;
    m_group& operator= (const m_group&) = default;

    /*
    * 计划 增加
    * 第一种实现为无 组内id 的 新节点 添加
    * 第二种实现为有 组内id 的 老节点 添加
    * 二者最终都是通过调用 _add_plan() 实现
    *
    *    pid: 计划组内id
    *    uid: 计划所属id
    * status: 状态
    * 后6个传参为class plan构造传参
    * 
    * 返回值:
    *  -1 : 所属用户不存在
    *  -2 : 计划组内id重复
    * 其余: 计划的组内id
    */
    int addplan(int u_id, int status,
                int plan_id, int user_id,
                int create_t, int plan_t,
                const char* content_str, const char* remark_str);
    int addplan(int p_id, int u_id, int status,
                int plan_id, int user_id,
                int create_t, int plan_t,
                const char* content_str, const char* remark_str);

    /*
    * 计划 删除
    * u_id: 执行删除的uid
    * p_id: 计划的组内id
    *
    * 返回值:
    *  -1 : 用户不存在/计划不存在
    *  -2 : 无权限删除
    *   0 : 未知错误
    *   1 : 成功
    */
    int delplan(int u_id, int p_id);

    /*
    * 计划 改动
    * u_id: 执行用户id
    * p_id: 计划的组内id
    *
    * 返回值:
    *  -1 : 用户/计划不存在
    *  -2 : 无权限删除
    *  -3 : 状态数值超限
    *   1 : 成功
    */
    int updplan_t(int u_id, int p_id, int time);//完成时间
    int updplan_s(int u_id, int p_id, int status);//状态
    int updplan_c(int u_id, int p_id, 
                  const char* content_str, const char* remark_str);//内容

    /*
    * 用户 增加
    *  u_id: 用户id
    *  name: 用户名
    * email: 用户邮箱
    * 返回true为添加成功
    */
    bool adduser(int u_id, std::string name, std::string email);

    /*
    * 用户 删除
    * u_id: 用户id
    * 返回true为删除成功
    */
    bool deluser(int u_id);

    int get_plan_max_id() const { return plan_max_id; }

private:
    int _add_plan(int p_id, int u_id, int status,
                  int plan_id, int user_id,
                  int create_t, int plan_t,
                  const char* content_str, const char* remark_str);

public:
    /*
    *    group_id: 此组的id
    * plan_max_id: 当前组内计划最大id号 用于分配组内id
    *    admin_id: 管理员id
    */
    int group_id;
    int plan_max_id;
    int admin_id;

    /*
    * plans: 储存了此组中的计划 key为组内id
    * users: 储存了此组中的用户 key为用户id
    */
    std::unordered_map<int, m_plan> plans;
    std::unordered_map<int, m_user> users;
};

#endif
