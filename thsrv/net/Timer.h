/****************************************************************************
* @File:	Timer封装类
* @Date:	2020-2-26
* @Author:	T.H.
* @Note:	该类是Timer的抽象，并不是真实的Timer
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TIMER_H
#define THSRV_NET_TIMER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/TimeStamp.h"
#include "thsrv/net/TimerId.h"
#include "thsrv/base/Atomic.h"

#include <functional>
#include <memory>

namespace thsrv
{
	
namespace net
{

class EventLoop;

class Timer
{
public:
    typedef std::function<void()>TimerCallback;
    typedef base::Atomic<uint64_t>AtomicInt64;
public:
    Timer(const TimeStamp& when,const TimerCallback& cb,double interval = 0);
    // ~Timer();
    void start();
    void reset(const TimeStamp& now);
    TimeStamp expiration(){ return expiration_; }
    static uint64_t numTimerCreated(){ return s_numTimer_.load(); }
    bool repeated()const{ return interval_ != 0; }
private:
    double interval_;
    TimeStamp expiration_;
    TimerCallback timercb_;
    static AtomicInt64 s_numTimer_;
};  //END Timer CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TIMER_H

