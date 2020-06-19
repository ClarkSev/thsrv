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
#include "thsrv/net/Socket.h"
// #include "thsrv/base/TimeStamp.h"

#include <sys/eventfd.h>
#include <assert.h>  // for assert
#include <signal.h>  // for signal

namespace thsrv
{

// 当客户端异常退出时，如果服务器仍然往客户端写数据
// 会导致服务器接收到 RST 信号，并产生 SIGPIPE 信号，然后关闭服务器
// 因此需要忽略该错误，只需忽略该信号即可解决
#pragma GCC diagnostic ignored "-Wold-style-cast"
class IgnoreSigPipe
{
public:
	IgnoreSigPipe(){
		::signal(SIGPIPE, SIG_IGN);
	}
};  // END CLASS IgnorePipe
#pragma GCC diagnostic error "-Wold-style-cast"

IgnoreSigPipe g_ignoreSigPipeObj;

namespace net
{

/// START CLASS
__thread EventLoop* l_eventLoopInThread = NULL;

namespace detail
{

// 
int createEventFd()
{
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(evtfd<0){
		LOG_FATAL<<"createEventFd ERROR";
	}
	return evtfd;
}

}  // end namespace detail

//ctor.
EventLoop::EventLoop():\
running_(false),
wakeupFd_(detail::createEventFd()),
currentId_(CurrentThread::tid()),
wakeupChannel_(new Channel(this, wakeupFd_)),
poller_(new Poller(this)),
readyCond_(mx_),
timerQue_(new TimerQueue(this))
{
	if(l_eventLoopInThread){
		LOG_FATAL<<" anthor EventLoop in thread.";
	}
	l_eventLoopInThread = this;
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableRead();
}
EventLoop::~EventLoop()
{
	assert(running_==false);
	close(wakeupFd_);
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
		TimeStamp pollReturnTime = poller_->poll(timeout, &activeChannels_);
		for(auto lchannel : activeChannels_){
			lchannel->handleEvent(pollReturnTime);
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
	wakeup();
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

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = socketops::send(wakeupFd_, &one, sizeof(one));
	if(n != sizeof(one)){
		LOG_ERR<<"EventLoop::wakeup send: "<<n<<" bytes instead of 8";
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = socketops::read(wakeupFd_, &one, sizeof(one));
	if(n != sizeof(one)){
		LOG_ERR<<"EventLoop::wakeup read: "<<n<<" bytes instead of 8";
	}

}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


