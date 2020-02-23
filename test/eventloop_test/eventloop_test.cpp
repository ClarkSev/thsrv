/*
	²âÊÔÎÄ¼þ
*/
#include "thsrv/base/Thread.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/net/EventLoop.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <iostream>
#include <functional>
#include <mutex>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;

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
int main()
{
	EventLoop main_loop;
	g_loop = &main_loop;
	Thread th1(func1);
	Thread th2(func2);

	th1.start();
	th2.start();
	th1.join();
	th2.join();
	g_loop->start();
	return 0;
}



