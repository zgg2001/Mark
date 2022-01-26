/*************************************************************************
	> File Name: user.h
	> Author: ZHJ
	> Remarks: 数据结构 -- 用户
	> Created Time: Tue 25 Jan 2022 09:30:54 PM CST
 ************************************************************************/

#ifndef _USER_H_
#define _USER_H_

#include<string>
#include<vector>
#include<list>
#include<algorithm>
#include"plan.h"

/*
* 数据结构 -- 用户
* 每一个实例就是一个用户
*
*  user_id: 用户id
* username: 用户名
*     mail: 用户邮箱
*    plans: 储存此用户计划的组内id
*           plans[0] = 搁置计划组
*           plans[1] = 进行中计划组
*           plans[2] = 已完成计划组
*/
class user
{
public:
    user() = default;
    user(int u_id, std::string name, std::string email);
    user(const user&) = default;
    user& operator= (const user&) = default;

    /*
    * 添加新计划
    * 依据计划的状态加入对应的链表
    * 两种实现 按强枚举状态加入 和 按int状态加入
    * 
    * status: 计划状态
    *         0 - 搁置
    *         1 - 进行中 
    *         2 - 已完成
    *     id: 计划的组内id
    */
    void addplan(plan_status status, int id);
    void addplan(int status, int id);

    /*
    * 删除指定计划
    *
    * id: 计划的组内id
    *
    * 返回true为删除成功
    */
    bool delplan(int id);

    /*
    * 更新计划状态
    * 内部调用 _update_plan() 实现
    *
    *     id: 要更新的计划id(组内id)
    * newsta: 新的计划状态 0/1/2
    *
    * 返回true为变更成功
    */
    bool updplan(int id, plan_status newsta);
    bool updplan(int id, int newsta);

    /*
    * 搜索计划
    * id: 要搜索的计划id(组内id)
    * 返回true为找到
    */
    bool search(int id);

private:
    bool _update_plan(int id, int newsta);

public:
    int user_id;
    std::string username;
    std::string mail;
    std::vector<std::list<int>> plans;
};

#endif
