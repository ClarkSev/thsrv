/*
	²âÊÔÎÄ¼þ
*/

// #include "thsrv/base/StringPiece.h"
//#include "base/LogStream.h"
// #include "thsrv/base/Logger.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"
#include "thsrv/base/Thread.h"
// #include "thsrv/base"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <iostream>
#include <functional>
#include <mutex>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;

bool g_flg = false;
MutexLock g_mx;
Condition g_cond(g_mx);

void func1()
{
	// g_mx.lock();
	MutexLockGuard _l(g_mx);
	cout<<"calling func1"<<endl;
	cout<<"func1 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func1 pid = "<<::getpid()<<endl;
	int a=10;
	while(a--){
		cout<<"a = "<<a<<endl;
		sleep(1);
		if(a==5){
			g_flg = true;
			g_cond.notifyOne();
		}
	}
	// g_mx.unlock();
}
void func2()
{
	MutexLockGuard _l(g_mx);
	// g_mx.lock();
	cout<<"calling func2"<<endl;
	while(!g_flg)
		g_cond.wait();
	cout<<"func2 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func2 pid = "<<::getpid()<<endl;
	// g_mx.unlock();
}

int main()
{
	Thread th1, th2;
	th1.setThreadFunc(func1);
	th2.setThreadFunc(func2);
	th2.start(); 
	sleep(1);
	th1.start();
	return 0;
}



