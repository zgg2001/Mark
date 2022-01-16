# m_mail
&emsp;&emsp;此类是为了方便邮件的发送而创建的。</br>
&emsp;&emsp;此类是通过Linux系统调用使用 `mailx`进行邮件发送的，所以使用本类前必须确保环境下已安装程序 `mailx` 并按其要求配置完毕。

* 具体配置方法可以看这个链接:</br>
https://blog.csdn.net/qq_42859864/article/details/84862977

&emsp;&emsp;本类中仅对用户输入进行"缝合"，使其成为一条
`echo -e '内容' | mail -s '标题' 地址` 格式的命令，随后执行该命令以实现邮件发送的功能。

**使用说明：**
```
SET_MAIL_STATE(false);
SET_MAIL_STATE(true);
```
如上宏函数来设置邮件系统的状态，当状态为false时，系统不会进行邮件的发送。(初始状态为true)
```
SEND_MAIL("xxx@qq.com", "test", "hello world");
SEND_MAIL("xxx@qq.com", "test", "hello world\n测试\n%d", 2022);
```
如上宏函数来发送邮件，第一个参数为目标地址，第二个参数为标题，随后为邮件内容。
