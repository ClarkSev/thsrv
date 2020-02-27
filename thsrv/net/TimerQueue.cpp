/****************************************************************************
* @File:	定时器队列封装类
* @Date:	2020-2-26
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TimerQueue.h"
#include "thsrv/net/EventLoop.h"

#include <sys/timerfd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>  // UINTPTR_MAX

#include <vector>
#include <iterator>

namespace thsrv
{

namespace detail
{

int createTimerFd()
{
    int fd = ::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC );
    assert(fd >= 0);
    return fd;
}
void setTimerFd(const int fd, const TimeStamp& when)
{
    struct timespec tv_new_val;
    bzero(&tv_new_val, sizeof(timespec));
    // LOG_INFO<<"Timer::settime when = "<<when.toStringFmt();
    tv_new_val = timeToSpec(when);
    // set-up the parameter
    struct itimerspec it_new_val;
    bzero(&it_new_val, sizeof(itimerspec));
    double interval = 1;   // one second 
    if(when.vaild()){  // periodic running
        struct timespec tv_interval;  bzero(&tv_interval, sizeof(tv_interval));
        tv_interval.tv_sec = static_cast<time_t>(interval);
        const int64_t kNumNanSecondPerSecond = 1000 * 1000 * 1000;

#pragma GCC diagnostic ignored "-Wconversion"
        double interval_ns = (interval - tv_interval.tv_sec) * kNumNanSecondPerSecond;
#pragma GCC diagnostic error   "-Wconversion"

        tv_interval.tv_nsec = static_cast<long>(interval_ns);
        it_new_val.it_interval = tv_interval;
        it_new_val.it_value = tv_new_val;
    }else{
        bzero(&it_new_val.it_interval, sizeof(timespec));
        bzero(&it_new_val.it_value, sizeof(timespec));
        // it_new_val.it_value = tv_new_val;
    }
    // load the parameter
    int ret = ::timerfd_settime(fd, TFD_TIMER_ABSTIME, &it_new_val, NULL);
    if(ret<0){
        LOG_ERR<<"ERROR:"<<strerror(ret);
    }
}

}   // END DETAIL NAMESPACE

namespace net
{

/// START CLASS
TimerQueue::TimerQueue(EventLoop* loop):
loop_(loop),
timerfd_( detail::createTimerFd() ),
timerChannel_(new Channel(loop_, timerfd_))
{
    timerChannel_->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerChannel_->enableRead();
}
TimerQueue::~TimerQueue()
{
    timerChannel_->disableAll();
    timerChannel_->remove();
    ::close(timerfd_);  // close the fd;
    for(auto& elem : activeTimers_){
        delete elem.second;
    }
}
TimerId TimerQueue::addTimer(const TimeStamp& when, const TimerCallback& cb, double interval)
{
    TimerPtr timer(new Timer(when, cb, interval));
    loop_->runInLoop(std::bind(
        &TimerQueue::addTimerInLoop, this, timer
    ));
    return TimerId(timer, Timer::numTimerCreated());
}
void TimerQueue::cancelTimer(TimerId timerid)
{
    loop_->runInLoop(std::bind(
        &TimerQueue::cancelTimerInLoop, this, timerid.timer()
    ));
}
void TimerQueue::addTimerInLoop(TimerPtr timer)
{
    loop_->assertInLoopThread();
    bool changed = insert(timer);
    if(changed){
        detail::setTimerFd(timerfd_, timer->expiration());
    }
}
void TimerQueue::cancelTimerInLoop(TimerPtr timer)
{
    loop_->assertInLoopThread();
    TimeStamp when = timer->expiration();
    TimerQueue::Entry expiredId(when, timer);
    auto iter = activeTimers_.find(expiredId);
    if(iter !=  activeTimers_.end()){
        activeTimers_.erase(expiredId);
        delete expiredId.second;
    }
}
bool TimerQueue::insert(TimerPtr timer)
{
    loop_->assertInLoopThread();
    // insert the timer into set;
    bool changed = false;
    TimeStamp when = timer->expiration();
    auto iter = activeTimers_.begin();
    if(iter == activeTimers_.end() || when < iter->first){
        // 首次添加 或是 添加的过期时间比最开始的时间小
        changed = true;
    }
    std::pair<TimerSet::iterator, bool>ret 
        = activeTimers_.insert(Entry(when, timer));
    assert(ret.second);  // success
    return changed;
}
void TimerQueue::reset(const std::vector<Entry>& expiredVec, const TimeStamp& when)
{
    // it's not real timer
    for(auto& elem : expiredVec){
        if(elem.second->repeated()){
            elem.second->reset(when);
            insert(elem.second);
        }else{
            delete elem.second;  // FIXME
        }
    }
    if(!activeTimers_.empty()){
        TimeStamp nextExpire = activeTimers_.begin()->first;
        if(nextExpire.vaild())
            detail::setTimerFd(timerfd_, nextExpire);
    }
}
std::vector<TimerQueue::Entry> TimerQueue::getExpired(const TimeStamp& now)
{
    typedef std::pair<TimeStamp, TimerPtr>ExpiredId;
    ExpiredId nowId(now, reinterpret_cast<TimerPtr>(UINTPTR_MAX));  // FIXME:
    // the activeTimer_ sort by TimeStamp
    TimerSet::iterator pos = activeTimers_.lower_bound(nowId);
    std::vector<TimerQueue::Entry> expiredVec(activeTimers_.begin(), pos);
    activeTimers_.erase(activeTimers_.begin(), pos);
    return expiredVec;
}
void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    TimeStamp now = TimeStamp::now();
    std::vector<TimerQueue::Entry> expiredVec = getExpired(now);
    for(auto& timer : expiredVec){
        timer.second->start();  // handle events
    }
    reset(expiredVec, now);
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


