/****************************************************************************
* @File:	Timer封装类
* @Date:	2020-2-25
* @Author:	T.H.
* @Note:	功能说明：在Timer运行完 runAfter() 之后，会自动销毁该对象
        精度为 us 级
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Timer.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/net/Channel.h"

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

Timer::Timer(EventLoop* loop,const TimeStamp& when, const TimerCallback& cb, double interval):
loop_(loop),
start_(false),
interval_(interval),
when_(when),
timercb_(cb),
timerChannel_(new Channel(loop_, timerid_.fd() ) )
{
    timerChannel_->setReadCallback(std::bind(&Timer::handleRead, this) );
    s_numTimer_.fetchAndAdd(1);
}
Timer::~Timer()
{
    cancelAndDie();
}
void Timer::start()
{
    loop_->runInLoop(std::bind(
        &Timer::startInLoop, this
    ));
}
void Timer::cancelAndDie()
{
    loop_->runInLoop(std::bind(
        &Timer::cancelAndDieInLoop, this
    ));
}
void Timer::restart(const TimeStamp& when,const TimerCallback& cb, double interval)
{
    loop_->runInLoop(std::bind(
        &Timer::restartInLoop, this, when, cb, interval
    ));
}
/// private method and interface
void Timer::startInLoop()
{
    loop_->assertInLoopThread();
    assert(start_ == false);
    start_ = true;
    settime(when_);
}
void Timer::cancelAndDieInLoop()
{
    loop_->assertInLoopThread();
    if(!start_)  return;
    settime( TimeStamp::invaild() );  // stop the timer
    s_numTimer_.fetchAndSub(1);
    start_ = false;
}
void Timer::restartInLoop(const TimeStamp& when,const TimerCallback& cb, double interval)
{
    loop_->assertInLoopThread();
    assert(start_ == true);
    start_ = false;
    when_ = when;
    timercb_ = cb;
    interval_ = interval;
    startInLoop();
}
void Timer::settime(const TimeStamp& when)
{
    loop_->assertInLoopThread();
    struct timespec tv_new_val;
    bzero(&tv_new_val, sizeof(timespec));
    // LOG_INFO<<"Timer::settime when = "<<when.toStringFmt();
    tv_new_val = timeToSpec(when);
    // set-up the parameter
    struct itimerspec it_new_val;
    bzero(&it_new_val, sizeof(itimerspec));

    if(interval_){  // periodic running
        struct timespec tv_interval;  bzero(&tv_interval, sizeof(tv_interval));
        tv_interval.tv_sec = static_cast<time_t>(interval_);
        const int64_t kNumNanSecondPerSecond = 1000 * 1000 * 1000;

#pragma GCC diagnostic ignored "-Wconversion"
        double interval_ns = (interval_ - tv_interval.tv_sec) * kNumNanSecondPerSecond;
#pragma GCC diagnostic error   "-Wconversion"

        tv_interval.tv_nsec = static_cast<long>(interval_ns);
        it_new_val.it_interval = tv_interval;
        it_new_val.it_value = tv_new_val;
    }else{
        bzero(&it_new_val.it_interval, sizeof(timespec));
        it_new_val.it_value = tv_new_val;
    }
    // load the parameter
    int ret = ::timerfd_settime(timerid_.fd(), TFD_TIMER_ABSTIME, &it_new_val, NULL);
    if(ret<0){
        LOG_ERR<<"ERROR:"<<strerror(ret);
    }
    timerChannel_->enableRead();
}
void Timer::handleRead()
{
    loop_->assertInLoopThread();
    // read the timerfd;
    uint64_t buf;
    int errnum = static_cast<int>(::read(timerid_.fd(), &buf, sizeof(uint64_t)));
    if(static_cast<size_t>(errnum) < sizeof(uint64_t)){
        LOG_ERR<<"ERROR:"<<strerror(errnum);
        return;
    }
    if(timercb_)  timercb_();  // run the callback
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


