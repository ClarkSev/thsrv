/*
	²âÊÔÎÄ¼ş
*/

// #include "thsrv/base/StringPiece.h"
//#include "base/LogStream.h"
// #include "thsrv/base/Logger.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/Mutex.h"
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

int main()
{
	Thread th1, th2;
	MutexLock mx;
	// pthread_t tid;
	auto func1 = [&](void*)->void*{ 
		// lock_guard<mutex> _l(mx);
		MutexLockGuard _l(mx);
		cout<<"calling func1"<<endl;
		// sleep(1);
		cout<<"func1 current tid = "<<CurrentThread::tid()<<endl;
		cout<<"func1 pid = "<<::getpid()<<endl;
		int a=10;
		while(a--){
			cout<<"a = "<<a<<endl;
			sleep(1);
			if(a==5)
				th1.exit();
		}
		return NULL;
	};
	auto func2 = [&](void*)->void*{
		// lock_guard<mutex> _l(mx);
		MutexLockGuard _l(mx);
		cout<<"calling func2"<<endl;
		// sleep(1);
		cout<<"func2 current tid = "<<CurrentThread::tid()<<endl;
		cout<<"func2 pid = "<<::getpid()<<endl;
		return NULL;
	};
	th1.setThreadFunc(func1);
	cout<<"numThread = "<<Thread::numThread()<<endl;
	th2.setThreadFunc(func2);
	cout<<"numThread = "<<Thread::numThread()<<endl;
	th2.start();
	cout<<"numThread = "<<Thread::numThread()<<endl;
	th1.start();
	cout<<"numThread = "<<Thread::numThread()<<endl;
	{
		MutexLockGuard _l(mx);
		// lock_guard<mutex> _l(mx);
		cout<<"main tid = "<<CurrentThread::tid()<<"  pid = "<<::getpid()<<endl;
		cout<<"th1.started = "<<th1.started()<<"  th2.started = "<<th2.started()<<endl;
	}
	th1.join();  
	th2.join();
	return 0;
}



