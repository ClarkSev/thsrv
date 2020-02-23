/****************************************************************************
* @File:	I/O线程轮询封装类
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Poller.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/Channel.h"
#include "thsrv/net/EventLoop.h"

#include <sys/epoll.h>  //for epoll
#include <errno.h>
#include <string.h>  // for strerror

#include <string>

namespace thsrv
{

namespace net
{

/// fd的状态
const int kNewFd = -1;
const int kAddedFd = 1;
const int kDeletedFd = 2;

int Poller::kInitSzEvents = 16;

/// START CLASS
Poller::Poller(EventLoop* tloop):\
loop_(tloop),
epollfd_( (epoll_create1(EPOLL_CLOEXEC)) ),
events_(kInitSzEvents)
{
	assert(epollfd_>=0);
}

TimeStamp Poller::poll(int timeout, ChannelList* activeChannel)
{
	TimeStamp tm(TimeStamp::now());
	int nfds = ::epoll_wait(epollfd_, \
	&*events_.begin(),\
	static_cast<int>(events_.size()),
	timeout);
	if(nfds<0){
		LOG_ERR<<" ERROR:"<<strerror(errno);
	}else if(nfds==0){
		// LOG_WARN<<"no events happen.";
		return tm;
	}
	fullActiveChannel(nfds, activeChannel);  // activeChannel将由EventLoop使用
	if(static_cast<size_t>(nfds)==events_.size()){
		events_.resize(2*nfds);
	}
	return tm;
}
// 该函数不负责 Channel 的真实删除
void Poller::updateChannel(Channel* channel)
{
	int state = channel->fd_state();
	int lfd = channel->fd();
	switch(state){
		case kNewFd:
			assert(channels_.find(lfd)==channels_.end());  //channel-map didn't exist the channel
			channels_[lfd] = channel;
			updateEpollEvent(EPOLL_CTL_ADD, channel);
			channel->set_fd_state(kAddedFd);break;
		case kAddedFd:
			assert(channels_[lfd] == channel);
			if(channel->isNoneEvents()){
				updateEpollEvent(EPOLL_CTL_DEL, channel);
				channel->set_fd_state(kDeletedFd);
			}else{
				updateEpollEvent(EPOLL_CTL_MOD, channel);
			}break;
		case kDeletedFd:
			updateEpollEvent(EPOLL_CTL_ADD, channel);
			channel->set_fd_state(kAddedFd); break;   // for re-use it
		default:
			LOG_WARN<<" Channel(fd = "<<channel->fd()<< ") has wrong state parameter.";
	}
}
void Poller::removeChannel(Channel* channel)
{
	int lfd = channel->fd();
	// LOG_INFO<<"Poller::removeChannel: fd = "<<lfd;
	assert(channel->isNoneEvents());
	assert(channels_[lfd] == channel);
	channels_.erase(lfd);   // earse by key, remove from channel-map
	if(channel->fd_state() == kAddedFd){
		updateEpollEvent(EPOLL_CTL_DEL, channel);  // remove from listen-list
	}
	channel->set_fd_state(kNewFd);  // set-operate for re-use
}
// private method
void Poller::fullActiveChannel(int numEvent, ChannelList* tchannels)
{
	// LOG_INFO<<"Poller::fullActiveChanne numEvent = "<<numEvent;
	for(int i=0; i<numEvent; i++){
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revent(events_[i].events);
		tchannels->push_back(channel);
	}
}
void Poller::updateEpollEvent(int ops, Channel* tchannel)
{
	int lfd = tchannel->fd();
	struct epoll_event levt;
	bzero(&levt, sizeof(levt));
	levt.data.ptr = static_cast<void*>(tchannel);
	levt.events = tchannel->events();
	LOG_INFO<<"Poller::updateEpollEvent fd = "<<lfd\
			<<", ops = "<<operateToStr(ops)\
			<<", evts = "<<tchannel->eventToString(levt.events);
	if(::epoll_ctl(epollfd_, ops, lfd, &levt)<0){
		LOG_WARN<<"::epoll_ctl ERROR: "<<strerror(errno)<<" fd = "<<lfd<<" ops = "<<operateToStr(ops);
	}
}
std::string Poller::operateToStr(int ops)
{
	switch(ops){
		case EPOLL_CTL_MOD:
			return "MOD";
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		default:
			return "NULL";
	}
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


