/*
	²âÊÔÎÄ¼þ
*/

// #include "thsrv/base/StringPiece.h"
//#include "base/LogStream.h"
// #include "thsrv/base/Logger.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/ThreadPool.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <iostream>
#include <functional>
#include <mutex>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;

MutexLock g_mx;

void func1()
{
	// lock_guard<mutex> _l(mx);
	// MutexLockGuard _l(mx);
	cout<<"calling func1"<<endl;
	// sleep(1);
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
class ptrClass
{
public:
	ptrClass(int tval):val_(tval){};
	void setVal(int tval)
	{
		val_ = tval;
	}
	void showVal() const
	{
		cout<<"ptrClass : val_ = "<<val_<<endl;
	}
private:
	int val_;
};
class testClass
{
public:
	testClass():ptr_(new ptrClass(1)){}
	ptrClass* getPtr()
	{
		return ptr_.get();
	}
	void setClassVal(int tval)
	{
		ptrClass* ptr = getPtr();
		ptr->setVal(tval);
		ptr->showVal();
	}
private:
	unique_ptr<ptrClass>ptr_;
};
void testFunc()
{
	testClass ts;
	ts.setClassVal(10);
}
int main()
{
	ThreadPool thpool(3);
	cout<<"start"<<endl;
	thpool.start();
	thpool.pushTask(func1);
	thpool.pushTask(func2);
	thpool.pushTask(func3);
	thpool.pushTask(func4);
	auto func5 = [](){
		cout<<"func5 tid = "<<CurrentThread::tid()<<"  pid = "<<::getpid()<<endl;
	};
	thpool.pushTask(func5);
	{
		MutexLockGuard _l(g_mx);
		cout<<"main tid = "<<CurrentThread::tid()<<"  pid = "<<::getpid()<<endl;
		cout<<"thpool start = "<<thpool.started()<<endl;
	}
	thpool.stop();
	return 0;
}



