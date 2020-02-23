/****************************************************************************
* @File:	通信通道封装类
* @Date:	2020-2-15
* @Author:	T.H.
* @Note:	功能说明：该通道类可注册三个半事件，主要起到传输线路的作用
		将epoll中void* ptr进行广义化
	一个 fd 只能拥有一个 Channel
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_CHANNEL_H
#define THSRV_NET_CHANNEL_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/TimeStamp.h"

#include <functional>  // for function
#include <string>
namespace thsrv
{
	
namespace net
{

class EventLoop;

class Channel
{
public:  // public properity
	typedef std::function<void()> EventCallback;
	typedef std::function<void()> ReadCallback;
	
public:
	Channel(EventLoop* loop, int fd);
	~Channel();
	
	void enableRead(){ ev_ |= kEventPollIn; update();}
	void enableWrite(){ ev_ |= kEventPollOut; update();}
	void disableRead(){ ev_ &= ~kEventPollIn; update();}
	void disableWrite(){ ev_ &= ~kEventPollOut; update();}
	void disableAll(){ ev_ = kNoneEvent; update();}
	
	bool isWritting()const{ return ev_ & kEventPollOut; }
	bool isReading() const{ return ev_ & kEventPollIn; }
	bool isNoneEvents()const{ return ev_ == kNoneEvent; }
	
	void remove();
	
	// for epoll
	int fd()const{ return fd_; }
	int events() const{ return ev_; }
	int fd_state() const{ return fd_state_; }    // for epoll_ctl operate
	void set_fd_state(const int state){ fd_state_ = state; }
	void set_revent(const int tev){ rev_ = tev; }
	
	void handleEvent();
	void setReadCallback(ReadCallback cb){ readcb_ = cb; }
	void setWriteCallback(EventCallback cb){ writecb_ = cb; }
	void setCloseCallback(EventCallback cb){ closecb_ = cb;}
	void setErrorCallback(EventCallback cb){ errorcb_ = cb;}
	// void setWriteCompleteCallback();
	std::string eventToString(int evts)const;
private:  // private method
	void update();
	
private:  // private properity
	EventLoop* loop_;
	int fd_;
	int ev_;
	int rev_;  // return events
	int fd_state_;  // fd state
	int removed_;  // channel state: 1----vailed  0----invailed

	EventCallback writecb_;
	EventCallback closecb_;
	ReadCallback  readcb_;
	EventCallback errorcb_;
	
	static const int kNoneEvent;
	static const int kEventPollIn;
	static const int kEventPollOut;
	
};  //END Channel CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_CHANNEL_H

