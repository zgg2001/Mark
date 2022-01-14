# m_semaphore
&emsp;&emsp;此类主要是为了线程安全退出而创建的。</br>
&emsp;&emsp;类中声明了 `wait()` 和 `wakeup()` 方法。在 `wait()` 方法调用后调用线程将会阻塞，直至其他线程调用 `wakeup()` 进行唤醒。</br>
&emsp;&emsp;当子线程需要退出时，首先在主线程中调用 `wait()`，并在子线程执行内容末尾调用 `wakeup()`，即可确保子线程安全退出后主线程继续执行。

# m_thread
&emsp;&emsp;此类主要是为了方便创建子线程而创建的。</br>
&emsp;&emsp;在创建该类对象后，调用其 `start(event, event, event)` 方法，传入三个事件，即可完成线程的创建和启动。</br>
&emsp;&emsp;三个事件分别为创建事件、运行事件和销毁事件，运行顺序由左至右。</br>
&emsp;&emsp;事件类型实际为 `std::function<void(m_thread*)>`，所以可以传入普通函数、lambda表达式等。</br>
&emsp;&emsp;线程的退出通过 `close()`(安全退出) 和 `exit()`(不安全退出) 来进行。

