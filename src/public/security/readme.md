# m_md5
&emsp;&emsp;此类是为了计算字符串的md5值而创建的。</br>
&emsp;&emsp;此类主要通过 `#include<openssl/md5.h>` 内的函数计算md5值，仅进行一个简单封装。

**使用说明：**
```
/*
* 计算md5值
* src: 需要计算md5值的字符串
* 返回计算后的结果
*/
static std::string md5sum(const std::string& src);
```
使用如上静态方法来计算md5值。
```
string a = "123456";
string ret = m_md5::md5sum(a);
cout << ret << endl;

编译:
g++ test.cpp -o test -lcrypto

输出:
e10adc3949ba59abbe56e057f20f883e
```
一个简单例程。

# m_noecho
&emsp;&emsp;此类是为了不明文显示输入而创建的。</br>
&emsp;&emsp;此类主要通过 `struct termios` 的相关设置实现，从而禁止输入回显。

**使用说明：**
```
/*
* 设置无回显输入模式
*/
void noecho();

/*
* 恢复
*/
void recover();
```
使用如上方法来开启/关闭 `NO_ECHO` 模式。
```
m_noecho noec;
std::string passwd;

printf("password: ");
noec.noecho();
std::cin >> passwd;
printf("\n");
noec.recover();
```
一个简单应用场景。

