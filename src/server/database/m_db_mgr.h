/*************************************************************************
	> File Name: m_db_mgr.h
	> Author: ZHJ
	> Remarks: 基于m_db类进行数据库管理
	> Created Time: Thu 03 Feb 2022 08:31:00 PM CST
 ************************************************************************/

#ifndef _M_DB_MGR_H
#define _M_DB_MGR_H

#include<string>
#include<unordered_map>
#include<public/log/log.h>
#include<public/database/m_db.h>

class m_server;

class m_db_mgr
{
public:
    m_db_mgr(m_server* server);
    virtual ~m_db_mgr();
    m_db_mgr() = delete;
    m_db_mgr(const m_db_mgr&) = delete;
    m_db_mgr& operator= (const m_db_mgr&) = delete;

    /*
    * 反序列化数据
    *
    *    host: host
    *    user: 登录用户名
    *  passwd: 登录密码
    * 
    * 返回值:
    * -4 : 失败 - 所属server不存在
    * -3 : 失败 - mark库内表数量异常
    * -2 : 失败 - 连接失败 - mysql_init() error
    * -1 : 失败 - 连接失败 - mysql_real_connect() error
    *  0 : 成功 - 读取成功
    *  1 : 成功 - 第一次使用, 初始化数据库
    */
    int init(const char* host, const char* user,
             const char* passwd);

    /*
    * 登录比对
    *
    * username: 用户名
    * password: 密码
    *
    * 返回值:
    * pair<uid, gid>
    * 登录失败时uid = gid = -1
    * 否则为用户的uid/gid
    */
    std::pair<int, int> login(std::string& name, std::string& passwd);

private:
    //所属server
    m_server* _server;

    //数据库连接
    m_db _db;
    
    //锁
    std::mutex _mutex;

private:
    //用户-密码结构体
    struct _info
    {
        int uid;
        int gid;
        std::string username;
        std::string password;
        _info() {}
        _info(int u, int g, std::string n, std::string w):
            uid(u), gid(g), username(n), password(w) {}
    };
    using _info = struct _info;
    
    /*
    * 此表内储存用户信息
    * 键为用户名
    * 值为结构体_info 其中储存uid、gid、用户名、密码
    */
    std::unordered_map<std::string, _info> _info_map;

private:
    //查询mark库中有几张表
    static constexpr char SQL_GET_TABLE_NUM[] = "           \
    SELECT                                                  \
        COUNT(TABLE_NAME)                                   \
    FROM                                                    \
        information_schema.TABLES                           \
    WHERE TABLE_SCHEMA = 'mark';";

    //计划表 建表语句
    static constexpr char SQL_CREATE_PLAN_TBL[] = "         \
    CREATE TABLE `mark_plan` (                              \
    `id` mediumint unsigned NOT NULL AUTO_INCREMENT,        \
    `create_user` smallint unsigned NOT NULL DEFAULT '0',   \
    `status` tinyint unsigned NOT NULL DEFAULT '0',         \
    `create_time` int unsigned DEFAULT '20220101',          \
    `plan_time` int unsigned DEFAULT '0',                   \
    `content` varchar(255) NOT NULL DEFAULT 'plan',         \
    `remark` varchar(255) NOT NULL DEFAULT '',              \
    PRIMARY KEY (`id`));";

    //计划-组 建表语句
    static constexpr char SQL_CREATE_PLAN_INFO_TBL[] = "    \
    CREATE TABLE `mark_plan_info` (                         \
    `id` mediumint unsigned NOT NULL DEFAULT '0',           \
    `user_id` smallint unsigned NOT NULL DEFAULT '0',       \
    `group_id` tinyint unsigned NOT NULL DEFAULT '0',       \
    `plan_id` mediumint unsigned NOT NULL DEFAULT '0',      \
    PRIMARY KEY (`id`));";

    //组表 建表语句
    static constexpr char SQL_CREATE_GROUP_TBL[] = "        \
    CREATE TABLE `mark_group` (                             \
    `group_id` tinyint unsigned NOT NULL AUTO_INCREMENT,    \
    `group_name` varchar(255) NOT NULL DEFAULT 'group',     \
    `admin_id` smallint unsigned NOT NULL DEFAULT '0',      \
    PRIMARY KEY (`group_id`));";
    
    //用户-组 建表语句
    static constexpr char SQL_CREATE_USER_GROUP_TBL[] = "   \
    CREATE TABLE `mark_user_group` (                        \
    `user_id` smallint unsigned NOT NULL DEFAULT '0',       \
    `group_id` tinyint unsigned NOT NULL DEFAULT '0',       \
    PRIMARY KEY (`group_id`,`user_id`));";

    //用户表 建表语句
    static constexpr char SQL_CREATE_USER_TBL[] = "         \
    CREATE TABLE `mark_user` (                              \
    `user_id` smallint unsigned NOT NULL AUTO_INCREMENT,    \
    `user_name` varchar(50) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL DEFAULT '',    \
    `password` varchar(255) DEFAULT NULL,                   \
    `mail_address` varchar(255) DEFAULT NULL,               \
    PRIMARY KEY (`user_id`),                                \
    UNIQUE KEY (`user_name`));";

    //查询用户-密码
    static constexpr char SQL_SELECT_USER_INFO[] = "        \
    SELECT                                                  \
        u.user_id, ug.group_id, u.user_name, u.password     \
    FROM                                                    \
        mark_user as u                                      \
    JOIN                                                    \
        mark_user_group as ug                               \
    ON                                                      \
        u.user_id = ug.user_id;";

    //查询组
    static constexpr char SQL_SELECT_GROUP_TBL[] = "        \
    SELECT                                                  \
        *                                                   \
    FROM                                                    \
        mark_group;";

    //查询用户-组
    static constexpr char SQL_SELECT_USER_GROUP_TBL[] = "   \
    SELECT                                                  \
        u.user_id, g.group_id, u.user_name, u.mail_address  \
    FROM                                                    \
        mark_user as u                                      \
    JOIN                                                    \
        mark_user_group as g                                \
    ON                                                      \
        u.user_id = g.user_id;";

    //查询计划信息
    static constexpr char SQL_SELECT_PLAN_INFO_TBL[] = "    \
    SELECT                                                  \
        i.id, i.user_id, i.group_id, i.plan_id, p.create_user, p.status, p.create_time, p.plan_time, p.content, p.remark \
    FROM                                                    \
        mark_plan as p                                      \
    JOIN                                                    \
        mark_plan_info as i                                 \
    ON                                                      \
        p.id = i.id;";
};

#endif
