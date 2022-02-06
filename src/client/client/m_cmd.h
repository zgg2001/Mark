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
    .help_str = "exit",
    .tokens = {
        reinterpret_cast<parse_token_hdr_t*>(&cmd_exit_tok),
        NULL,
    },
};

/*
* 命令组
*/
parse_ctx_t main_ctx[] = {
    reinterpret_cast<parse_inst_t*>(&cmd_exit),
    NULL,
};

#endif
