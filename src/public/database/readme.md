# m_sql
&emsp;&emsp;此类主要是封装了一下C++连接操作MySql的过程，使得项目中对数据库的操作更加方便。</br>
&emsp;&emsp;使用本类的前提条件是需要确保已经安装MySql数据库系统，并且安装其开发包 `mysql-devel`。</br></br>
&emsp;&emsp;为了确保线程安全，需要在主线程的起始位置添加 `mysql_library_init(0, nullptr, nullptr)` 并在主线程的末尾位置添加 `mysql_library_end();`。</br>
* 参考地址:</br>
https://dev.mysql.com/doc/c-api/8.0/en/mysql-thread-init.html</br>
https://dev.mysql.com/doc/c-api/8.0/en/mysql-library-init.html</br>

使用说明:</br>
```
/*
* 连接MySql
*
*    host: host
*    user: 登录用户名
*  passwd: 登录密码
* db_name: 数据库名
*
* 连接成功返回true
*/
bool connect(const char* host, const char* user, const char* passwd, const char* db_name);
```
此函数进行数据库的连接操作。</br>
```
/*
* 执行指定sql语句
*
*        sql: 想要执行的sql语句
*        res: 储存查询结果
*   num_rows: 结果的行数
* num_fields: 结果的字段数
*
* 返回 -1 执行失败
* 返回  0 执行成功且有执行结果(select)
* 返回  1 执行成功但无执行结果
*/
int exec(const char* sql, std::vector<char**>* res, int* num_rows, int* num_fields);
```
此函数进行数据库语言的执行操作，并可以传入变量来储存执行结果。
