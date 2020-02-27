/****************************************************************************
* @File:	Timer封装类
* @Date:	2020-2-25
* @Author:	T.H.
* @Note:	管理一个timerid，并将其封装便于使用
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
class Channel;

class Timer
{
public:
    typedef std::function<void()>TimerCallback;
    typedef base::Atomic<uint64_t>AtomicInt64;
public:
    Timer(EventLoop* loop,const TimeStamp& when,const TimerCallback& cb,double interval = 0);
    ~Timer();
    TimerId timerid()const{ return timerid_; }
    void cancelAndDie();  // once the timer is canceled, it will die;
    void start();
    void restart(const TimeStamp& when, const TimerCallback& cb, double interval = 0);
    bool canceled()const{ return start_ == false; }
    bool started()const{ return start_ == true; }
    bool repeated()const{ return interval_ == 0; }

    static uint64_t numTimerCreated(){ return s_numTimer_.load(); }

private:
    void handleRead();
    void settime(const TimeStamp& when);
    void startInLoop();
    void cancelAndDieInLoop();
    void restartInLoop(const TimeStamp& when,const TimerCallback& cb, double interval);
    // TimerStamp gettime();
private:
    EventLoop* loop_;
	const TimerId timerid_;
    bool start_;
    double interval_;
    TimeStamp when_;
    TimerCallback timercb_;
    std::unique_ptr<Channel> timerChannel_;

    static AtomicInt64 s_numTimer_;
};  //END Timer CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TIMER_H

