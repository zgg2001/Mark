/*************************************************************************
	> File Name: m_cmd.h
	> Author: ZHJ
	> Remarks: 交互式命令行 命令声明定义
	> Created Time: Sun 06 Feb 2022 09:43:07 AM CST
 ************************************************************************/

#ifndef _M_CMD_H
#define _M_CMD_H

#include<nice_cmd/parse_string.h>
#include<nice_cmd/parse_num.h>

/*
* 命令 exit 退出
*/
struct cmd_exit_result
{
    fixed_string_t exit;
};

static void
cmd_exit_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    cmdline_quit(cl);
    printf("Bye...\n");
}

parse_token_string_t cmd_exit_tok =
    TOKEN_STRING_INITIALIZER(struct cmd_exit_result, exit, "exit");

parse_inst_t cmd_exit = {
    .f = cmd_exit_parsed,
    .data = NULL,
    .help_str = (char*)"退出",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_exit_tok),
        NULL,
    },
};

/*
* 命令 新增plan
*/
struct cmd_add_plan_result
{
    fixed_string_t str1;
    fixed_string_t str2;
};

static void
cmd_add_plan_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    m_client::ins()->m_add_plan();
}

parse_token_string_t cmd_add_plan_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_add_plan_result, str1, "add");
parse_token_string_t cmd_add_plan_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_add_plan_result, str2, "plan");

parse_inst_t cmd_add_plan = {
    .f = cmd_add_plan_parsed,
    .data = NULL,
    .help_str = (char*)"添加计划 add plan",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_add_plan_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_add_plan_tok2),
        NULL,
    },
};

/*
* 命令 删除plan
*/
struct cmd_del_plan_result
{
    fixed_string_t str1;
    fixed_string_t str2;
    int num1;
};

static void
cmd_del_plan_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    struct cmd_del_plan_result* result = (cmd_del_plan_result*)parsed_result;
    int id = result->num1;

    if(id <= 0)
    {
        printf("error - id应为正数\n");
        return;
    }
    m_client::ins()->m_del_plan(id);
}

parse_token_string_t cmd_del_plan_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_del_plan_result, str1, "delete");
parse_token_string_t cmd_del_plan_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_del_plan_result, str2, "plan");
parse_token_num_t cmd_del_plan_tok3 =
    TOKEN_NUM_INITIALIZER(struct cmd_del_plan_result, num1, INT32);

parse_inst_t cmd_del_plan = {
    .f = cmd_del_plan_parsed,
    .data = NULL,
    .help_str = (char*)"删除计划 delete plan X",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_del_plan_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_del_plan_tok2),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_del_plan_tok3),
        NULL,
    },
};

/*
* 命令 更改plan
*/
struct cmd_upd_plan_result
{
    fixed_string_t str1;
    fixed_string_t str2;
    fixed_string_t str3;
    int num1;
};

static void
cmd_upd_plan_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    struct cmd_upd_plan_result* result = (cmd_upd_plan_result*)parsed_result;
    int id = result->num1, mode = 0;

    if(strcmp("time", result->str3) == 0)
        mode = 0;
    else if(strcmp("status", result->str3) == 0)
        mode = 1;
    else
        mode = 2;

    if(id <= 0)
    {
        printf("error - id应为正数\n");
        return;
    }
    m_client::ins()->m_upd_plan(mode, id);
}

parse_token_string_t cmd_upd_plan_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_upd_plan_result, str1, "update");
parse_token_string_t cmd_upd_plan_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_upd_plan_result, str2, "plan");
parse_token_string_t cmd_upd_plan_tok3 =
    TOKEN_STRING_INITIALIZER(struct cmd_upd_plan_result, str3, "time#status#content");
parse_token_num_t cmd_upd_plan_tok4 =
    TOKEN_NUM_INITIALIZER(struct cmd_upd_plan_result, num1, INT32);

parse_inst_t cmd_upd_plan = {
    .f = cmd_upd_plan_parsed,
    .data = NULL,
    .help_str = (char*)"更新计划 update plan time|status|content X",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_upd_plan_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_upd_plan_tok2),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_upd_plan_tok3),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_upd_plan_tok4),
        NULL,
    },
};

/*
* 命令 show plan
*/
struct cmd_show_plan_result
{
    fixed_string_t str1;
    fixed_string_t str2;
    int num1;
};

static void
cmd_show_plan_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    struct cmd_show_plan_result* result = (cmd_show_plan_result*)parsed_result;
    int id = result->num1;

    if(id <= 0)
    {
        printf("error - id应为正数\n");
        return;
    }
    m_client::ins()->m_show_plan(id);
}

parse_token_string_t cmd_show_plan_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_result, str1, "show");
parse_token_string_t cmd_show_plan_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_result, str2, "plan");
parse_token_num_t cmd_show_plan_tok3 =
    TOKEN_NUM_INITIALIZER(struct cmd_show_plan_result, num1, INT32);

parse_inst_t cmd_show_plan = {
    .f = cmd_show_plan_parsed,
    .data = NULL,
    .help_str = (char*)"展示计划 show plan X",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_tok2),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_tok3),
        NULL,
    },
};

/*
* 命令 show plan user
*/
struct cmd_show_plan_u_result
{
    fixed_string_t str1;
    fixed_string_t str2;
    fixed_string_t str3;
};

static void
cmd_show_plan_u_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    struct cmd_show_plan_u_result* result = (cmd_show_plan_u_result*)parsed_result;
    int mode = 1;

    if(strcmp("shelve", result->str3) == 0)
        mode = 0;
    else if(strcmp("progressing", result->str3) == 0)
        mode = 1;
    else if(strcmp("complete", result->str3) == 0)
        mode = 2;
    else 
        mode = 3;

    m_client::ins()->m_show_plan_u(mode);
}

parse_token_string_t cmd_show_plan_u_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_u_result, str1, "show");
parse_token_string_t cmd_show_plan_u_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_u_result, str2, "plan");
parse_token_string_t cmd_show_plan_u_tok3 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_u_result, str3, "shelve#progressing#complete#all");

parse_inst_t cmd_show_plan_u = {
    .f = cmd_show_plan_u_parsed,
    .data = NULL,
    .help_str = (char*)"展示计划 show plan shelve|progressing|complete|all",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_u_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_u_tok2),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_u_tok3),
        NULL,
    },
};

/*
* 命令 show plan group
*/
struct cmd_show_plan_g_result
{
    fixed_string_t str1;
    fixed_string_t str2;
    fixed_string_t str3;
    fixed_string_t str4;
};

static void
cmd_show_plan_g_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    struct cmd_show_plan_g_result* result = (cmd_show_plan_g_result*)parsed_result;
    int mode = 1;

    if(strcmp("shelve", result->str4) == 0)
        mode = 0;
    else if(strcmp("progressing", result->str4) == 0)
        mode = 1;
    else if(strcmp("complete", result->str4) == 0)
        mode = 2;
    else 
        mode = 3;

    m_client::ins()->m_show_plan_g(mode);
}

parse_token_string_t cmd_show_plan_g_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_g_result, str1, "show");
parse_token_string_t cmd_show_plan_g_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_g_result, str2, "group");
parse_token_string_t cmd_show_plan_g_tok3 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_g_result, str3, "plan");
parse_token_string_t cmd_show_plan_g_tok4 =
    TOKEN_STRING_INITIALIZER(struct cmd_show_plan_g_result, str4, "shelve#progressing#complete#all");

parse_inst_t cmd_show_plan_g = {
    .f = cmd_show_plan_g_parsed,
    .data = NULL,
    .help_str = (char*)"展示组计划 show group plan shelve|progressing|complete|all",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_g_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_g_tok2),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_g_tok3),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_show_plan_g_tok4),
        NULL,
    },
};

/*
* 更改密码
*/
struct cmd_reset_password_result
{
    fixed_string_t str1;
    fixed_string_t str2;
};

static void
cmd_reset_password_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    m_client::ins()->m_reset_password();
}

parse_token_string_t cmd_reset_password_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_reset_password_result, str1, "reset");
parse_token_string_t cmd_reset_password_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_reset_password_result, str2, "password");

parse_inst_t cmd_reset_password = {
    .f = cmd_reset_password_parsed,
    .data = NULL,
    .help_str = (char*)"重置密码 reset password",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_reset_password_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_reset_password_tok2),
        NULL,
    },
};

/*
* 命令 新增组/用户
*/
struct cmd_add_group_user_result
{
    fixed_string_t str1;
    fixed_string_t str2;
};

static void
cmd_add_group_user_parsed(struct cmdline* cl, void* parsed_result, void* data)
{
    cmd_add_group_user_result* result = (cmd_add_group_user_result*)parsed_result;
    
    if(strcmp("group", result->str2) == 0)
        m_client::ins()->m_add_group();
    else
        m_client::ins()->m_add_user();
}

parse_token_string_t cmd_add_group_user_tok1 =
    TOKEN_STRING_INITIALIZER(struct cmd_add_group_user_result, str1, "add");
parse_token_string_t cmd_add_group_user_tok2 =
    TOKEN_STRING_INITIALIZER(struct cmd_add_group_user_result, str2, "group#user");

parse_inst_t cmd_add_group_user = {
    .f = cmd_add_group_user_parsed,
    .data = NULL,
    .help_str = (char*)"添加组/用户 add group|user",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_add_group_user_tok1),
        reinterpret_cast<parse_token_hdr_t*>(&cmd_add_group_user_tok2),
        NULL,
    },
};

/*
* 命令组
*/
parse_ctx_t main_ctx[] = {//常规
    reinterpret_cast<parse_inst_t*>(&cmd_exit),
    reinterpret_cast<parse_inst_t*>(&cmd_add_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_del_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_upd_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan_u),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan_g),
    reinterpret_cast<parse_inst_t*>(&cmd_reset_password),
    NULL,
};
parse_ctx_t main_ctx_root[] = {//root
    reinterpret_cast<parse_inst_t*>(&cmd_exit),
    reinterpret_cast<parse_inst_t*>(&cmd_add_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_del_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_upd_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan_u),
    reinterpret_cast<parse_inst_t*>(&cmd_show_plan_g),
    reinterpret_cast<parse_inst_t*>(&cmd_reset_password),
    reinterpret_cast<parse_inst_t*>(&cmd_add_group_user),
    NULL,
};

#endif
