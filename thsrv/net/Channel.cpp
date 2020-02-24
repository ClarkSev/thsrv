/****************************************************************************
* @File:	通信通道封装类
* @Date:	2020-2-15
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Channel.h"
#include "thsrv/base/TimeStamp.h"
#include "thsrv/net/EventLoop.h"

#include <sys/epoll.h>  // for EPOLLIN
#include <assert.h>

namespace thsrv
{

namespace net
{

/// START CLASS
const int Channel::kNoneEvent = 0;
const int Channel::kEventPollIn = EPOLLIN | EPOLLPRI;
const int Channel::kEventPollOut = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd):
loop_(loop),
fd_(fd),
ev_(kNoneEvent),
rev_(kNoneEvent),
fd_state_(-1),
removed_(0)   // create it and set vaild state
{}

Channel::~Channel()
{
	assert(ev_ == kNoneEvent);
	if(!removed_)   loop_->removeChannel(this);
}
void Channel::remove()
{
	assert(removed_ == 0);
	assert(ev_ == kNoneEvent);
	loop_->removeChannel(this);
	removed_ = 1;
}
void Channel::handleEvent()
{
	LOG_INFO<<"Channel::handleEvent revents = "<< eventToString(rev_);
	if(rev_& kEventPollIn){
		if(readcb_) readcb_();
	}else if(rev_ & kEventPollOut){
		if(writecb_)  writecb_();
	}else{
		if(errorcb_)  errorcb_();
	}
	// if(rev_ &)
}
std::string Channel::eventToString(int evts)const
{
	if(evts & kNoneEvent){
		return "NoneEvent";
	}else if(evts & kEventPollIn){
		return "ReadEvent";
	}else if(evts & kEventPollOut){
		return "WriteEvent";
	}else{
		return "NULL";
	}
}
/// private method
void Channel::update()
{
	loop_->updateChannel(this);
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


