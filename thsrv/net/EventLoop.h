/****************************************************************************
* @File:	事件循环封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	功能说明：one loop per thread
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_EVENTLOOP_H
#define THSRV_NET_EVENTLOOP_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"
#include "thsrv/base/QueueThreadSafe.h"

#include <functional>   // for function
#include <vector>
#include <deque>
#include <memory>

namespace thsrv
{
	
namespace net
{

class Channel;
class Poller;

class EventLoop : public noncopyable
{
public:  // properity
	typedef std::function<void()> TASK;
	typedef std::vector<Channel*> ChannelList;
public: // method
	EventLoop();
	~EventLoop();
	
	void start();  // just only start in itself thread
	void stop();   // can stop in any thread
	bool started()const;  // any thread
	bool stoped()const;   // any thread
	void runInLoop(TASK t_task);
	void runAfter(double delay_ms, TASK t_task);
	void runEvery(TASK t_task);
	void queueInLoop(TASK t_task);
	
	void removeChannel(Channel* channel);
	void updateChannel(Channel* channel);	
// private: // method
	void assertInLoopThread()const;
	bool isInLoopThread() const;
private: // properity
	bool running_;
	const pid_t currentId_;
	std::unique_ptr<Poller> poller_;
	mutable base::MutexLock mx_;
	base::Condition readyCond_;
	ChannelList activeChannels_;
	base::QueueThreadSafe<TASK> taskQue_;
	
};  //END EventLoop CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_EVENTLOOP_H

