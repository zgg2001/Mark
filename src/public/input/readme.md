# m_input
&emsp;&emsp;此类是为了确保输入模式正常而创建的。未来的更新方向是可以规定输入的类型(纯数字、纯字母等)，目前先凑合用。</br>
&emsp;&emsp;此类主要通过 `struct termios` 的相关设置实现，从而实现输入正常。

**使用说明：**
```
/*
* 设置为输入模式
*/
void input();

/*
* 恢复
*/
void recover();
```
使用如上方法来开启/关闭输入模式。
```
m_input i;
std::string name;

printf("name: ");
i.input();
std::cin >> name;
printf("\n");
i.recover();
```
一个简单应用场景。
