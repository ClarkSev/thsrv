/****************************************************************************
* @File:	线程池封装类
* @Date:	2020-2-11
* @Author:	T.H.
* @Note:	只有非线程安全的函数进行了标记
	其加锁的粒度较大，考虑后续完善
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_THREADPOOL_H
#define THSRV_BASE_THREADPOOL_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Thread.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"
// #include "thsrv/base/QueueThreadSafe.h"

#include <deque>
#include <vector>
#include <memory>   // for unique_ptr<>
#include <functional>  // for function

namespace thsrv
{
	
namespace base
{
	
class ThreadPool
{
public:  // public properity
	typedef std::function<void()> TASK;
	const static int kNumInitThread = 5;
	
public:  // public method and interface
	explicit ThreadPool(int tNumThread = kNumInitThread);
	~ThreadPool();
	void setMaxSizeQue(const int tsz);
	// void setThreadInitTask(const TASK& cb);
	Thread* getNextThread();   // no thread-safe
	void pushTask(TASK task);
	void start();
	void stop();
	bool started() const;
	bool stoped() const;
	size_t queueSize() const;
	bool taskFull() const;
	bool taskEmpty() const;
	
private: // private method and interface
	// thread-safe
	TASK getTask();
	void runInThread();
	
private: // private properity
	int maxThreads_;
	size_t maxSizeQue_;
	bool running_;
	
	mutable MutexLock mx_;
	Condition notEmpty_;
	Condition notFull_;
	
	// TASK threadInitCallback_;
	std::vector< std::unique_ptr<Thread> > threadVec_;
	std::deque<TASK> taskQue_;
	
};  //END ThreadPool CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_THREADPOOL_H

