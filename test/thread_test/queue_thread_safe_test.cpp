/*
	²âÊÔÎÄ¼þ
*/

// #include "thsrv/base/StringPiece.h"
//#include "base/LogStream.h"
// #include "thsrv/base/Logger.h"
// #include "thsrv/base/CurrentThread.h"

#include "thsrv/base/QueueThreadSafe.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <iostream>
#include <functional>
#include <mutex>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;

QueueThreadSafe<int>g_que(20);
MutexLock g_mx;
void func1()
{
	// lock_guard<mutex> _l(mx);
	
	cout<<"calling func1"<<endl;
	// sleep(1);
	QueueThreadSafe<int>l_que;
	{
		MutexLockGuard _l(g_mx);
		g_que.swap(l_que);
	}
	for(auto elem : l_que){
		cout<<" func2 l_que = "<<elem;
	}
	cout<<endl;
	cout<<"func1 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func1 pid = "<<::getpid()<<endl;
	int a=6;
	while(a--){
		cout<<"pop = "<<g_que.try_pop()<<endl;
		sleep(2);
	}
}
void func2()
{
	// lock_guard<mutex> _l(mx);
	cout<<"calling func2"<<endl;
	cout<<"func2 current tid = "<<CurrentThread::tid()<<endl;
	cout<<"func2 pid = "<<::getpid()<<endl;
	int b=20;
	while(b--){
		{
			MutexLockGuard _l(g_mx);
			cout<<"b = "<<b<<endl;
		}
		g_que.try_push(b);
		sleep(1);
	}
}

int main()
{
	Thread th1(func1), th2(func2);
	th2.start();
	sleep(3);
	th1.start();
	th1.join();
	th2.join();
	
	cout<<"g_que = ";
	for(auto elem:g_que){
		cout<<elem<<" ";
	}
	cout<<endl;
	return 0;
}



