/*
	测试文件
*/
#include "thsrv/base/TimeStamp.h"
#include "thsrv/base/Logger.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/net/TcpServer.h"
#include "thsrv/net/Buffer.h"
#include "thsrv/net/TcpConnection.h"
#include "thsrv/net/Socket.h"

#include <iostream>
#include <string>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;
using namespace std::placeholders;

MutexLock g_mx;

EventLoop *g_loop = NULL;

void func3();
void func4();

void func1()
{
	// lock_guard<mutex> _l(mx);
	// MutexLockGuard _l(mx);
	cout<<"calling func1"<<endl;
	g_loop->runInLoop(func4);
	cout<<"func1 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func1 pid = "<<::getpid()<<endl;
	int a=6;
	while(a--){
		cout<<"a = "<<a<<endl;
		sleep(1);
	}
}
void func2()
{
	// lock_guard<mutex> _l(mx);
	// MutexLockGuard _l(mx);
	cout<<"calling func2"<<endl;
	// sleep(1);
	g_loop->runInLoop(func3);
	cout<<"func2 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func2 pid = "<<::getpid()<<endl;
}
void func3()
{
	cout<<"calling func3"<<endl;
	cout<<"func3 current tid = "<<CurrentThread::tid()<<endl;
}
void func4()
{
	cout<<"calling func4"<<endl;
	cout<<"func4 current tid = "<<CurrentThread::tid()<<endl;
}
class EchoServer
{
public:
	EchoServer(EventLoop* loop,const InetAddress& addr):\
	loop_(loop), echoserver_(loop, "echo_server", addr)
	{
		// echoserver_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
		echoserver_.setOnMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
		echoserver_.setWriteCompleteCallback(std::bind(&EchoServer::writeComplete, this, _1));
	}
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO<<"connection.";
	}
	void onMessage(const TcpConnectionPtr& conn,Buffer& buf,TimeStamp receiveTime)
	{
		std::string data = buf.retrieveAllToString();
		LOG_INFO<<"recv data:"<<data;
		conn->send(data);
	}
	void writeComplete(const TcpConnectionPtr& conn)
	{		
		LOG_INFO<<"writeComplete.";
	}
	void start()
	{
		echoserver_.start();
	}
private:
	EventLoop* loop_;
	TcpServer echoserver_;
};

#if 1
int main()
{
	EventLoop main_loop;
	g_loop = &main_loop;
	// Thread th1(func1);
	// Thread th2(func2);

	// th1.start();
	// th2.start();
	// th1.join();
	// th2.join();
	
	InetAddress addr(1234);
	EchoServer lecho(g_loop, addr);
	
	lecho.start();
	
	g_loop->start();
	return 0;
}
#endif //0

#if 0
int main()
{
	uint16_t tport = 1234;
	std::string tip = "1.2.3.4";
	InetAddress addr(tport,1);
	LOG_INFO<<"addr = "<<addr.toIpAndPort();
	return 0;
}
#endif //0

