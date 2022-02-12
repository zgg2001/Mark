/*************************************************************************
	> File Name: m_datagram.h
	> Author: ZHJ
	> Remarks: 数据报文
	> Created Time: Sun 06 Feb 2022 12:00:35 PM CST
 ************************************************************************/

#ifndef _M_DATAGRAM_H_
#define _M_DATAGRAM_H_

/*
* 命令(报文)类型
*/
enum cmd
{
    CMD_C2S_HEART,      //心跳 client to server
    CMD_S2C_HEART,      //心跳 server to client
    CMD_LOGIN,          //登录 
    CMD_LOGIN_RESULT,   //登录结果
    CMD_ADD_PLAN,       //新增计划
    CMD_DEL_PLAN,       //删除计划
    CMD_UPD_PLAN_T,     //改动计划 - time
    CMD_UPD_PLAN_S,     //改动计划 - status
    CMD_UPD_PLAN_C,     //改动计划 - content
    CMD_OPERATE_RESULT, //增/删/改结果
    CMD_SHOW_PLAN_USER, //show个人计划
    CMD_SHOW_RESULT_U,  //获取个人计划结果
    CMD_SHOW_PLAN_GROUP,//show组计划
    CMD_SHOW_RESULT_G   //获取组计划结果
};

/*
* 报头
*    cmd: 报文命令
* length: 报文长度
*/
struct header
{
    short cmd;
    short length;
};

/*
* 心跳报文 C2S
*/
struct c2s_heart : public header
{
    c2s_heart()
    {
        this->cmd = CMD_C2S_HEART;
        this->length = sizeof(c2s_heart);
    }
};

/*
* 心跳报文 S2C
*/
struct s2c_heart : public header
{
    s2c_heart()
    {
        this->cmd = CMD_S2C_HEART;
        this->length = sizeof(s2c_heart);
    }
};

/*
* 登录报文
*/
struct login : public header
{
    login()
    {
        this->cmd = CMD_LOGIN;
        this->length = sizeof(login);
    }
    char username[34];//用户名
    char password[34];//密码
};

/*
* 登录结果
*/
struct login_result : public header
{
    login_result()
    {
        this->cmd = CMD_LOGIN_RESULT;
        this->length = sizeof(login_result);
    }
    int result;
};

/*
* 新增计划
*/
struct add_plan : public header
{
    add_plan()
    {
        this->cmd = CMD_ADD_PLAN;
        this->length = sizeof(add_plan);
    }
    int create_time;    //计划创建时间
    int plan_time;      //计划完成时间
    char content[102];  //内容
    char remark[102];   //备注
};

/*
* 删除计划
*/
struct del_plan : public header
{
    del_plan()
    {
        this->cmd = CMD_DEL_PLAN;
        this->length = sizeof(del_plan);
    }
    int plan_id;
};

/*
* 改动计划 - 计划时间
*/
struct upd_plan_t : public header
{
    upd_plan_t()
    {
        this->cmd = CMD_UPD_PLAN_T;
        this->length = sizeof(upd_plan_t);
    }
    int plan_id;
    int plan_time;
};

/*
* 改动计划 - 计划状态
*/
struct upd_plan_s : public header
{
    upd_plan_s()
    {
        this->cmd = CMD_UPD_PLAN_S;
        this->length = sizeof(upd_plan_s);
    }
    int plan_id;
    int status;
};

/*
* 改动计划 - 计划内容
*/
struct upd_plan_c : public header
{
    upd_plan_c()
    {
        this->cmd = CMD_UPD_PLAN_C;
        this->length = sizeof(upd_plan_c);
    }
    int plan_id;
    char content[102];  //内容
    char remark[102];   //备注
};

/*
* 增/删/改结果
*/
struct operate_result : public header
{
    operate_result()
    {
        this->cmd = CMD_OPERATE_RESULT;
        this->length = sizeof(operate_result);
    }
    int result;
};

/*
* 获取计划 -- 个人计划
*/
struct show_plan_u : public header
{
    show_plan_u()
    {
        this->cmd = CMD_SHOW_PLAN_USER;
        this->length = sizeof(show_plan_u);
    }
    int mode;
};

/*
* 获取计划结果 -- 个人计划
*/
struct show_ret_u
{
    int plan_id;
    int status;
    int create_time;
    int plan_time;
    char content[102];
};
struct show_result_u : public header
{
    show_result_u()
    {
        this->cmd = CMD_SHOW_RESULT_U;
        this->length = sizeof(show_result_u);
    }
    int sn;//状态数 为0时说明为尾包 客户端停止阻塞
    int noap;//有效计划数 <=10
    show_ret_u plans[10];
};

/*
* 获取计划 -- 组计划
*/
struct show_plan_g : public header
{
    show_plan_g()
    {
        this->cmd = CMD_SHOW_PLAN_GROUP;
        this->length = sizeof(show_plan_g);
    }
    int mode;
};

/*
* 获取计划结果 -- 组计划
*/
struct show_ret_g
{
    int plan_id;
    int user_id;
    char username[12];
    int status;
    int create_time;
    int plan_time;
    char content[102];
};
struct show_result_g : public header
{
    show_result_g()
    {
        this->cmd = CMD_SHOW_RESULT_G;
        this->length = sizeof(show_result_g);
    }
    int sn;//状态数 为0时说明为尾包 客户端停止阻塞
    int noap;//有效计划数 <=10
    show_ret_g plans[10];
};

#endif
