/*************************************************************************
	> File Name: plan.h
	> Author: ZHJ
	> Remarks: 数据结构 -- 计划
	> Created Time: Mon 24 Jan 2022 07:05:49 PM CST
 ************************************************************************/

#ifndef _PLAN_H_
#define _PLAN_H_

/*
* 计划状态
*
*      shelve: 搁置
* progressing: 进行中
*    complete: 已完成
*/
enum class plan_status
{
	shelve,
	progressing,
	complete
};

/*
* 数据结构 -- 计划
* 最底层数据结构 
* 每一个实例储存一条计划
*
*           id: 计划id(总id)
*  create_user: 创建者用户id
*       status: 计划的状态
*  create_time: 创建的时间
*    plan_time: 计划完成的时间
*      content: 计划的内容
*       remark: 备注
*/
class plan
{
public:
    //储存大小
    static constexpr int SIZE = 100 + 2;//'\0'

public:
    //普通构造
    plan(int plan_id, int user_id,
         int create_t, int plan_t,
         const char* content_str, const char* remark_str);
    plan() = default;
    //copy构造
    plan(const plan& p) = default;
    plan& operator= (const plan& p) = default;
    //更改内容
    void change(const char* content_str, const char* remark_str);

public:
    int id;
    int create_user;
    plan_status status;
    int create_time;
    int plan_time;
    char content[SIZE];
    char remark[SIZE];
};

#endif



