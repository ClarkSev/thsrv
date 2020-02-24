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
#include "thsrv/net/EventLoopThread.h"
#include "thsrv/net/EventLoop.h"

#include <assert.h>

namespace thsrv
{

namespace net
{

/// START CLASS
EventLoopThread::EventLoopThread():
start_(false),
cond_(mx_),
loop_(NULL)
{}

EventLoopThread::~EventLoopThread()
{
    stopLoop();
}
EventLoop* EventLoopThread::startLoop()
{
    EventLoop *loop = NULL;
    thread_.setThreadFunc(std::bind(
        &EventLoopThread::threadFunc, this
    ));
    {
        base::MutexLockGuard _l(mx_);
        thread_.start();  // will call threadFunc
        while(loop_ == NULL){
            cond_.wait();
        }
        assert(loop_ != NULL);
        start_ = true;
        loop = loop_;
    }
    return loop;
}
void EventLoopThread::stopLoop()
{
    if(!start_)  return;
    loop_->stop();  // FIXME: using mutex guard
    thread_.join();
}
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        base::MutexLockGuard _l(mx_);
        assert(loop_ == NULL);
        loop_ = &loop;
        cond_.notifyOne();
    }
    loop.start();
    base::MutexLockGuard _l(mx_);
    loop_ = NULL;
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


