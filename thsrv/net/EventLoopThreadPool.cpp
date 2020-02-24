/****************************************************************************
* @File:	事件循环封装类
* @Date:	2020-2-24
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/EventLoopThreadPool.h"
#include "thsrv/net/EventLoop.h"

#include <assert.h>

namespace thsrv
{

namespace net
{

/// START CLASS
EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, \
    const int tnum, const std::string& nameArg):\
baseLoop_(loop),
start_(false),
numThread_(tnum),
idx_(0),
name_(nameArg),
cond_(mx_)
{
    assert(baseLoop_ != NULL);
    assert(numThread_ >= 0);
    // threads_.resize(numThread_);
}
EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(start_);
    // dispatch loop using robin-round rule
    base::MutexLockGuard _l(mx_);
    if(numThread_ == 0){
        return baseLoop_;
    }else{
        return loops_[ (idx_++) % numThread_];
    }
}
void EventLoopThreadPool::startPool()
{
    assert(start_ == false);
    base::MutexLockGuard _l(mx_);
    start_ = true;
    //此处无权控制baseLoop的启动
    for(int i = 0; i < numThread_; i++){
        threads_.emplace_back(new EventLoopThread());
        loops_.push_back(threads_[i]->startLoop());
    }
}
void EventLoopThreadPool::stopPool()
{
    if(!start_)  return;
    base::MutexLockGuard _l(mx_);
    for(auto& threadPtr : threads_){
        threadPtr->stopLoop();
    }
    threads_.clear();
    loops_.clear();
    start_ = false;
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


