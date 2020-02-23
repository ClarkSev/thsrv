/****************************************************************************
* @File:	线程封装类
* @Date:	2020-2-10
* @Author:	T.H.
* @Note:	功能说明：对线程使用pthread_create等进行封装，含thread_id标识

			注：创建的线程不能传递参数与返回值
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_THREAD_H
#define THSRV_BASE_THREAD_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Atomic.h"

#include <pthread.h>

#include <string>
#include <functional>  // for function

namespace thsrv
{
	
namespace base
{
	
// 线程函数指针
typedef std::function<void()>ThreadFunc;

class Thread
{	
public:  // public method
	Thread();
	Thread(ThreadFunc thFunc);
	~Thread();
	
	void start();
	void join();
	void exit();
	bool started()const { return started_; }
	bool joined()const { return joined_; }
	void setThreadFunc(ThreadFunc tFunc);
	// 用于记录线程数量，以便权衡性能，只有在线程start之后，才会记录
	static int numThread() { return numThread_.load(); }
	pthread_t tid() const{ return tid_; }

private: // private method and property
	bool started_;
	bool joined_;
	pthread_t tid_;
	ThreadFunc func_;
	static Atomic<int> numThread_;
};  //END THREAD CLASS	

}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_THREAD_H

