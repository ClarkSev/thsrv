# thsrv
基于linux的高并发网络服务器框架（借鉴于muduo库）

项目说明：
1.Server端
	·server部分没有添加超时剔除客户的功能；
	·地址转换仍然有问题:
		使用getsockname 与 getpeername 时，指定的addrlen一定要对，可以指定为 sizeof(sockaddr_in)，否则返回的数据会是全0数据
	·经常出现accept连接失败：
		accept传递参数时，addr与addrlen一定要初始化 addrlen = sizeof(sockaddr_in)，否则是否成功全凭运气
	·解决文件描述符达上限问题（errno = EMFILE）：
		可以事先准备一个idlefd填坑，当出现 EMFILE 问题时，将idlefd关闭并重新接受请求，并将接收的 fd 关闭，最后再次填坑以备后续使用；若不处理该问题，会出现一直请求连接问题（busy loop）

2.Client端
	·retry时，runAfter存在问题，延时时间较短会出现无响应（出现死等现象），需要更改runAfter函数：
		使用Timer定时器设计
	·经常出现传输混乱——测试程序问题（FIXED）
	·reconnect存在问题

3.EventLoop 待添加 runEvery(FIXED)

4.线程安全待考证

5.项目性能测试未添加

6.不支持IPv6————后续完善

7.连接出现错误的相关处理程序考虑不周

8.原TimerQueue设计时，真实的Timer组成，这样比较浪费 timerfd；
	借鉴 muduo库的方法，设计一个Queue结构，该结构只有一个timerfd进行监控，这样虽然不能保证其及时处理，但是在权衡下是较优方案。

9.添加 HTTP 解析器，并实现了简易的 Web服务器， 目前仅支持 HTTP/1.0 与 HTTP/1.1 协议，以及只解析了 GET POST HEAD 方法
	目前浏览器能进行通信，但是似乎存在服务器断开连接后（可能没有真正的断开），浏览器仍然向服务器进行请求数据（一直在加载中）。

主要难点：
	1、Reactor + ThreadPool设计模型————使用Acceptor专用于接收请求，并使用ThreadPool分配任务；
	2、基于对象的设计思想，故使用了较多的函数对象实现回调，期间需要考虑对象的生存期问题；
	3、TcpConnection类的设计————整个库的核心部分；
	4、one loop per thread 设计
	5、使用 EventLoop 与 互斥量保证线程安全，合理使用 EventLoop->assertInLoopThread