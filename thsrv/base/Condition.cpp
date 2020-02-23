/****************************************************************************
* @File:	条件变量封装类
* @Date:	2020-2-12
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Condition.h"
#include "thsrv/base/Logger.h"

#include <errno.h>
#include "sys/time.h"

namespace thsrv
{
	
namespace base
{
	
///STAET Condition CLASS

Condition::Condition(MutexLock& tmx):mx_(&tmx),cond_(PTHREAD_COND_INITIALIZER)
{
	// pthread_cond_init(&cond_, NULL);  // must initialize before using.
}
Condition::~Condition()
{
	if(pthread_cond_destroy(&cond_)!=0)  LOG_FATAL<<"~Conditon:ERROR";
}

void Condition::wait()
{
	// pthread_cond_wait() 内部会使用 pthread_mutex_unlock() 将mx_进行解锁
	// 并挂起线程，等待唤醒，但是Mutex中实现会分配 holder，所以实现Condition时
	// 需要将 holer 进行释放
	MutexLock::UnassignHold _l(*mx_);
	if(pthread_cond_wait(&cond_, mx_->getPthreadMutex())!=0) 
		LOG_FATAL<<"Conditon::wait():ERROR";
}
bool Condition::waitForSecond(double tsec)
{
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);  // get abstime
	const int64_t kNumNanoSecondPerSecond = 1000000000;
	int64_t addtimeNanoSecond = static_cast<int64_t>(tsec * kNumNanoSecondPerSecond);
	abstime.tv_sec += static_cast<time_t>((addtimeNanoSecond + abstime.tv_nsec) / kNumNanoSecondPerSecond);
	abstime.tv_nsec = static_cast<long>((addtimeNanoSecond + abstime.tv_nsec) % kNumNanoSecondPerSecond);
	return ETIMEDOUT == pthread_cond_timedwait(&cond_, mx_->getPthreadMutex(), &abstime);
}
void Condition::notifyOne()
{
	if(pthread_cond_signal(&cond_)!=0) 
		LOG_FATAL<<"Conditon::notifyOne():ERROR";
}
void Condition::notifyAll()
{
	if(pthread_cond_broadcast(&cond_)!=0) 
		LOG_FATAL<<"Conditon::notifyAll():ERROR";
}

///END Condition CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


