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

