/****************************************************************************
* @File:	I/O线程轮询封装类
* @Date:	2020-2-15
* @Author:	T.H.
* @Note:	功能说明：使用epoll进行接收scoket请求，并将请求放入Channel中，
	由内部Channel进行处理

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_POLLER_H
#define THSRV_NET_POLLER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include "thsrv/base/TimeStamp.h"

#include <sys/epoll.h>  //for epoll

#include <vector>
#include <map>

namespace thsrv
{
	
namespace net
{
/// pre-declare
class EventLoop;
class Channel;
class string;
// struct epoll_event;

class Poller
{
public:  // public properity
	typedef std::vector<Channel* >ChannelList;
	typedef std::map<int, Channel*>ChannelMap;
	typedef std::vector<struct epoll_event>PollEvts;
	
public:  // public method
	Poller(EventLoop* tloop);
	TimeStamp poll(int timeOut, ChannelList* activeChannel);
	// 该函数不负责 Channel 的真实删除
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
	
private:  // private method
	void fullActiveChannel(int numEvent, ChannelList* tchannels);
	void updateEpollEvent(int ops, Channel* tchannel);
	std::string operateToStr(int ops);
	
private:  // private properity
	static int kInitSzEvents;
	
	EventLoop* loop_;
	const int epollfd_;
	PollEvts events_;
	ChannelMap channels_;
	
};  //END Poller CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_POLLER_H

