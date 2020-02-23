/****************************************************************************
* @File:	mutex封装类
* @Date:	2020-2-10
* @Author:	T.H.
* @Note:	pthread_t 获得的id号并不是唯一的，只是说同一时刻是唯一的，
	而pthread_t是一个结构体指针，内存是动态分配的，所以如果内存被重复使用，
	那么pthread_t 的 tid号就会重复。
	因此可以借助内核调用使用的id进行标识tid	，犹如 pid_t 一样，
	不过需要自己封装 gettid() 函数。
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_MUTEX_H
#define THSRV_BASE_MUTEX_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Logger.h"

// Shouldn't include this header,or lead to cross-include, and compiling wrong.
// 'friend class Condition' is ok, which is pre-declare.
// #include "thsrv/base/Condition.h"   

#include <assert.h>

namespace thsrv
{

namespace base
{
	
class MutexLock : public noncopyable
{
	
public:
	MutexLock():holder_(0)
	{
		pthread_mutex_init(&mx_, NULL);  // must initializate the mx_ before using
	}
	~MutexLock()
	{
		assert(holder_==0);
		if(pthread_mutex_destroy(&mx_)!=0) LOG_FATAL<<"~MutexLock ERROR";
	}
	bool isHoldByCurrentThread()const
	{
		return holder_ == CurrentThread::tid();
	}
	inline void lock()
	{
		if(pthread_mutex_lock(&mx_)!=0) LOG_FATAL<<"MutexLock::lock ERROR";
		assignHolder();
	}
	inline void unlock()
	{
		unassignHolder();
		if(pthread_mutex_unlock(&mx_)!=0)  LOG_FATAL<<"MutexLock::unlock ERROR";
	}
	inline pthread_mutex_t* getPthreadMutex(){ return &mx_; }
private:  //method
	friend class Condition;   // This class is only efficient to Condition class
	class UnassignHold
	{
		public:
			UnassignHold(MutexLock& tmx):owner_(&tmx){
				owner_->unassignHolder();
			}
			~UnassignHold(){
				owner_->assignHolder();
			}
		private:
			MutexLock* owner_;
	};  // END UnassignHold CLASS
	void assignHolder()
	{
		assert(holder_==0);
		holder_ = CurrentThread::tid();
	}
	void unassignHolder()
	{
		holder_ = 0;   // zero is the invaild value to pthread_t structure
	}
private:
	pthread_mutex_t mx_;
	pid_t holder_;
};  //END MUTEX CLASS
class MutexLockGuard : public noncopyable
{

public:
	MutexLockGuard(MutexLock& tmx):mx_(&tmx)
	{
		mx_->lock();
	}
	~MutexLockGuard()
	{
		mx_->unlock();
	}
private:
	MutexLock* mx_;
	
};	//END MutexLockGuard CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_MUTEX_H