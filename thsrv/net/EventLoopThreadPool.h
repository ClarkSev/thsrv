/****************************************************************************
* @File:	事件循环封装类
* @Date:	2020-2-24
* @Author:	T.H.
* @Note:	主要功能较为单一，只需要新建EventLoopThread与分配EventLoop即可
        numThread >= 0，默认为主线程，即使用一个base EventLoop；
        当numThread > 0,则baseLoop不参与 getNextLoop 的调配，此时baseLoop主要
        用于接收请求与分配任务
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_EVENTLOOPTHREADPOLL_H
#define THSRV_NET_EVENTLOOPTHREADPOLL_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"
#include "thsrv/net/EventLoopThread.h"

#include <vector>
#include <string>
#include <memory>

namespace thsrv
{
	
namespace net
{

class EventLoop;	

class EventLoopThreadPool
{
public:
    typedef std::unique_ptr<EventLoopThread> EventLoopThreadPtr;
//     const int kInitNumEventLoopThread = 5;
public:
    EventLoopThreadPool(EventLoop* loop, const int tnum = 0,\
                        const std::string& nameArg = "defaultThreadPoolName"
    );
    // void setNumThread(const int tnum);
    EventLoop* getNextLoop();
    void startPool();
    void stopPool();

private:
    EventLoop* baseLoop_;
    bool start_;
    const int numThread_;
    int idx_;
    const std::string name_;
    mutable base::MutexLock mx_;
    base::Condition cond_;
    // threads_ 中的线程数目始终比loops_中的EventLoop少一个，即threads_是新开辟的thread
    // 而loops_包含从用户传递来的 主EventLoop（因threads_没有将主线程计算进去）
    std::vector<EventLoopThreadPtr> threads_;
    std::vector<EventLoop*> loops_;
};  //END EventLoopThreadPool CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_EVENTLOOP_H

