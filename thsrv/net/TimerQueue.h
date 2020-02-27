/****************************************************************************
* @File:	定时器队列封装类
* @Date:	2020-2-26
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TIMERQUEUE_H
#define THSRV_NET_TIMERQUEUE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/TimeStamp.h"
#include "thsrv/net/Callback.h"
#include "thsrv/net/Channel.h"
#include "thsrv/net/Timer.h"
#include "thsrv/net/TimerId.h"


#include <functional>
#include <memory>
#include <vector>
#include <set>

namespace thsrv
{
	
namespace net
{
	
class EventLoop;

class TimerQueue
{
public:
    typedef Timer* TimerPtr;
    typedef std::pair<TimeStamp, TimerPtr>Entry;
    // typedef std::pair<TimerPtr, uint64_t>ID;
    typedef std::set<Entry> TimerSet;
    // typedef std::set<ID> TimerIdSet;
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();
    TimerId addTimer(const TimeStamp& when, const TimerCallback& cb, double interval = 0);
    void cancelTimer(TimerId timerid);
private:
    void handleRead();
    bool insert(TimerPtr timer);
    std::vector<TimerQueue::Entry> getExpired(const TimeStamp& now);
    void reset(const std::vector<TimerQueue::Entry>& expiredVec,const TimeStamp& when);
    void addTimerInLoop(TimerPtr timer);
    void cancelTimerInLoop(TimerPtr timer);
private:
    EventLoop* loop_;
    // bool earliestChange_;
    const int timerfd_;
    TimerSet activeTimers_;
    std::unique_ptr<Channel>timerChannel_;

};  //END TimerQueue CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TIMERQUEUE_H

