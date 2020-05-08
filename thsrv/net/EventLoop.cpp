/****************************************************************************
* @File:	事件循环封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/EventLoop.h"
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/Poller.h"
#include "thsrv/net/Channel.h"
// #include "thsrv/base/TimeStamp.h"

#include <assert.h>  // for assert

namespace thsrv
{

namespace net
{

/// START CLASS
__thread EventLoop* l_eventLoopInThread = NULL;

//ctor.
EventLoop::EventLoop():\
running_(false),
currentId_(CurrentThread::tid()),
poller_(new Poller(this)),
readyCond_(mx_),
timerQue_(new TimerQueue(this))
{
	if(l_eventLoopInThread){
		LOG_FATAL<<" anthor EventLoop in thread.";
	}
	l_eventLoopInThread = this;
}
EventLoop::~EventLoop()
{
	assert(running_==false);
	l_eventLoopInThread = NULL;
}
void EventLoop::start()
{
	assertInLoopThread();
	running_ = true;
	int timeout = 10;
	// TimeStamp tm(TimeStamp::now());
	while(running_){
		// handle i/o thread
		activeChannels_.clear();
		// LOG_INFO<<"After clear EventLoop::start activeChannels_.size"<<activeChannels_.size();
		poller_->poll(timeout, &activeChannels_);
		for(auto lchannel : activeChannels_){
			lchannel->handleEvent();
		}
		// handle others task
		base::QueueThreadSafe<TASK> l_task;
		taskQue_.swap(l_task);
		for(auto task : l_task){
			task();
		}
	}
	running_ = false;
}
void EventLoop::stop()
{
	base::MutexLockGuard _l(mx_);
	running_ = false;
}
bool EventLoop::started()const
{
	base::MutexLockGuard _l(mx_);
	return running_;
}
bool EventLoop::stoped()const
{
	base::MutexLockGuard _l(mx_);
	return (!running_);
}
void EventLoop::runInLoop(TASK t_task)
{
	if(isInLoopThread()){
		t_task();
	}else{
		queueInLoop(t_task);
	}
}
void EventLoop::runAfter(double delay_sec,const TimerCallback& t_task)
{
	TimeStamp when = addTime(TimeStamp::now(), delay_sec);
	runAt(when, t_task);
	// timerQue_->addTimer(when, t_task);
	// LOG_INFO<<"quit EventLoop::runAfter.";
}
void EventLoop::runEvery(double delay_sec, const TimerCallback& t_task)
{
	TimeStamp when = addTime(TimeStamp::now(), delay_sec);
	// runAt(when, t_task, delay_sec);
	timerQue_->addTimer(when, t_task, delay_sec);
	// LOG_INFO<<"quit EventLoop::runEvery.";
}
void EventLoop::runAt(const TimeStamp& when,const TimerCallback& t_task)
{
	timerQue_->addTimer(when, t_task);
}
void EventLoop::queueInLoop(TASK t_task)
{
	taskQue_.try_push(t_task);
	// wake-up
}
void EventLoop::removeChannel(Channel* channel)
{
	LOG_INFO<<"EventLoop::removeChannel";
	assertInLoopThread();
	poller_->removeChannel(channel);
}
void EventLoop::updateChannel(Channel* channel)
{
	assertInLoopThread();
	poller_->updateChannel(channel);
}

// method implement
inline bool EventLoop::isInLoopThread() const
{
	return currentId_ == CurrentThread::tid();
}
inline void EventLoop::assertInLoopThread()const
{
	assert(isInLoopThread());
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


