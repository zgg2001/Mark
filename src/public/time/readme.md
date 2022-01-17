# m_timer
&emsp;&emsp;此类为计时器类，是为了方便在此项目中统计时间而创建的。</br>
&emsp;&emsp;此类通过C++11的 `std::chrono` 类实现，总体比较简单，但是分为普通实现和线程安全实现。即通过加锁来保证线程安全，但是也有不加锁的版本，来避免锁的开销。

**使用说明：**
```
/*
* 更新起始时间
* safe为线程安全版
*/
void update();
void update_safe();
```
使用如上方法来重置计时器。
```
/*
* 计算当前时间与起始时间的时间差
* safe为线程安全版
*
* 返回值为double类型 单位为秒
*/
double get_sec();
double get_sec_safe();

```
使用如上方法来获取计时时间。
```
m_timer t;
t.update_safe();
sleep(1);
cout << t.get_sec_safe() << endl;

可能的输出:
1.00008
```
一个简单例程。

# m_clock
&emsp;&emsp;此类是为了方便获取高精度时间而创建的，在我的规划中用于心跳检测相关内容。</br>
&emsp;&emsp;此类中仅有一个静态方法来获取当前的时间戳，精度为毫秒。

**使用方法:**
```
/*
* 获取当前时间戳 (毫秒)
* 返回值为 time_t 类型
*/
static time_t
get_now_millsec();
```
使用如上静态方法获取当前时间戳。
```
time_t t = m_clock::get_now_millsec();
sleep(1);
cout << t << endl;
cout << m_clock::get_now_millsec() << endl;

可能的输出:
1642428277056
1642428278056
```
一个简单例程。
