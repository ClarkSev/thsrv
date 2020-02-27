/****************************************************************************
* @File:	Timer封装类
* @Date:	2020-2-26
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Timer.h"
#include "thsrv/net/EventLoop.h"


#include <sys/timerfd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <iostream>

/*
    struct timespec {
        time_t tv_sec;                // Seconds 
        long   tv_nsec;               // Nanoseconds 十亿分之一秒
    };
    struct itimerspec {
        struct timespec it_interval;  // Interval for periodic timer 周期
        struct timespec it_value;     // Initial expiration 初始过期时间
    };

	int timerfd_create(int clockid, int flags);
    int timerfd_settime(int fd, int flags,
                            const struct itimerspec *new_value,
                            struct itimerspec *old_value);
    int timerfd_gettime(int fd, struct itimerspec *curr_value);
*/

namespace thsrv
{

namespace net
{

// using namespace std::placeholders;

/// START CLASS
// static various initialization
Timer::AtomicInt64 Timer::s_numTimer_;

Timer::Timer(const TimeStamp& when, const TimerCallback& cb, double interval):
interval_(interval),
expiration_(when),
timercb_(cb)
{
    s_numTimer_.fetchAndAdd(1);
}
void Timer::start()
{
    timercb_();
}
void Timer::reset(const TimeStamp& now)
{
    if(interval_){
        expiration_ = addTime(now, interval_);
    }else{
        expiration_ = TimeStamp::invaild();
    }
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


