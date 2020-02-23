/****************************************************************************
* @File:	线程池封装类
* @Date:	2020-2-11
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/ThreadPool.h"

#include <assert.h>   // for assert

namespace thsrv
{

namespace base
{

/// START CLASS
//ctor.
ThreadPool::ThreadPool(int tNumThread):maxThreads_(tNumThread),
maxSizeQue_(-1),
running_(false),
notEmpty_(mx_),
notFull_(mx_)
{
	// threadVec_.resize(maxThreads_);  // initialize the vector
	for(int i = 0; i< maxThreads_;i++){
		threadVec_.emplace_back(new Thread());
	}
}
ThreadPool::~ThreadPool()
{
	if(running_) stop();
}
// thread-safe
void ThreadPool::setMaxSizeQue(const int tsz)
{
	taskQue_.resize(tsz);
	MutexLockGuard _l(mx_);
	maxSizeQue_ = static_cast<size_t>(tsz);
}
// void ThreadPool::setThreadInitTask(const TASK& cb)
// {
	// MutexLockGuard _l(mx_);
	// threadInitCallback_ = cb;
// }

// no thread-safe
Thread* ThreadPool::getNextThread()
{
	// MutexLockGuard _l(mx_);
	int curIdx = 0, szTaskQue = static_cast<int>(taskQue_.size());
	// queueSize() is the numbers of current tasks.
	// if(szTaskQue != 0)
	curIdx = szTaskQue % maxThreads_;
	return threadVec_[curIdx].get();
}
// thread-safe , 'cause of the queue_thread_safe structure
void ThreadPool::pushTask(TASK task)
{
	if(taskFull()){
		notFull_.wait();
	}
	MutexLockGuard _l(mx_);
	taskQue_.push_back(task);
}
void ThreadPool::start()
{
	MutexLockGuard _l(mx_);
	if(running_)  return;  // if it started, will be no-op
	running_ = true;
	for(auto& threadPtr : threadVec_){
		threadPtr->setThreadFunc(std::bind(&ThreadPool::runInThread,this));
		threadPtr->start();
	}
}
void ThreadPool::stop()
{
	{//lock
		MutexLockGuard _l(mx_);
		if(!running_) return;
		running_ = false;
		notEmpty_.notifyAll();
	}
	for(auto& threadPtr : threadVec_){
		threadPtr->join();
	}
}
bool ThreadPool::started()const
{
	MutexLockGuard _l(mx_);
	return running_;
}
bool ThreadPool::stoped()const
{
	MutexLockGuard _l(mx_);
	return (!running_);
}
// thread-safe
size_t ThreadPool::queueSize()const
{
	MutexLockGuard _l(mx_);
	return taskQue_.size();
}
// thread-safe
bool ThreadPool::taskFull()const
{
	MutexLockGuard _l(mx_);
	return maxSizeQue_ == taskQue_.size();
}
// thread-safe
bool ThreadPool::taskEmpty()const
{
	MutexLockGuard _l(mx_);
	return taskQue_.size() == 0;
}

///private method
// thread-safe, just only this function to pop
ThreadPool::TASK ThreadPool::getTask()
{
	TASK tmpTask;
	MutexLockGuard _l(mx_);
	while(taskQue_.empty() && running_){
		notEmpty_.wait();
	}
	if(!taskQue_.empty()){
		tmpTask = taskQue_.front();
		taskQue_.pop_front();
		notFull_.notifyOne();
	}
	return tmpTask;
}
// thread-safe, just be called at the beginning
void ThreadPool::runInThread()
{
	while(running_){
		TASK _task(getTask());
		if(_task) _task();
	}
}

/// END CLASS

} //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


