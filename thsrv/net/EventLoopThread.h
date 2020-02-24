/****************************************************************************
* @File:	事件循环线程封装类
* @Date:	2020-2-24
* @Author:	T.H.
* @Note:	该类不考虑线程安全问题
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_EVENTLOOPTHREAD_H
#define THSRV_NET_EVENTLOOPTHREAD_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Thread.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"

namespace thsrv
{
	
namespace net
{

class EventLoop;

class EventLoopThread
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
    void stopLoop();
    bool started()const{ return start_ == true; }
private:
    void threadFunc();
private:
    bool start_;
    base::Thread thread_;
    base::MutexLock mx_;
    base::Condition cond_;
    EventLoop* loop_;

};  //END EventLoopThread CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_EVENTLOOP_H

