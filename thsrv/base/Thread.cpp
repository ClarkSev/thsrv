/****************************************************************************
* @File:	线程封装类
* @Date:	2020-2-10
* @Author:	T.H.
* @Note:	功能说明：对线程使用pthread_create等进行封装，含thread_id标识

* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Logger.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/CurrentThread.h"

#include <assert.h>
#include <errno.h>  // for errno_t
#include <sys/syscall.h>  // for syscall()
#include <unistd.h>     // for getpid

namespace thsrv
{

namespace detail
{

pid_t gettid()
{
	return static_cast<pid_t>(syscall(SYS_gettid));  // get the only sign of tid
}

class ThreadData
{
public:
	explicit ThreadData(base::ThreadFunc& tfunc):func_(tfunc){}
	void* runInThreadData(void* targ)
	{
		if(func_)  func_();
		return NULL;
	}
private:
	base::ThreadFunc func_;
};  //END THREADDATA CLASS

void* _buildin_start_thread(void* targ)
{
	ThreadData* data = static_cast<ThreadData*>(targ);
	data->runInThreadData(NULL);
	delete data;
	return NULL;  // non-return
}

}  //END DETAIL NAMESPACE

namespace CurrentThread
{

void cacheTid()
{
	if(cacheTid_ == 0)
		cacheTid_ = detail::gettid();
}

inline bool isMainThread()
{
	return tid()==::getpid();
}

}  // END CURRENTTHREAD NAMESPACE

namespace base
{

/// 类的具体实现
//START THREAD CLASS
//static various
Atomic<int> Thread::numThread_;

//ctor.
Thread::Thread():started_(false),joined_(false),tid_(0),func_(NULL)
{}
Thread::Thread(ThreadFunc thFunc):started_(false),joined_(false),tid_(0),func_(thFunc)
{}
Thread::~Thread()
{
	if(started_ && !joined_){
		pthread_join(tid_, NULL);
		joined_ = true;
	}
	numThread_.fetchAndSub(1);  /// numThread_ -= 1;
}

///public interface
void Thread::setThreadFunc(ThreadFunc tFunc)
{
	assert(!started_);
	assert(!joined_);
	func_ = tFunc;
	
}

void Thread::start()
{
	assert(!joined_);  // started_ must be set-up before joined_
	if(started_)  return;	
	// 使用share_ptr等会导致内存重叠，两次连续使用只能保存最后的数据
	// std::shared_ptr<detail::ThreadData>data(new detail::ThreadData(func_)); 
	detail::ThreadData* data = new detail::ThreadData(func_);
	int retErr = pthread_create(&tid_, NULL, \
				 detail::_buildin_start_thread, \
				 static_cast<void*>(data));
	//FIXME: Throw an exception
	if(retErr){
		LOG_ERR<<"Thread::start ERROR:"<<strerror(retErr);
		delete data;
		return;
	}
	numThread_.fetchAndAdd(1);  /// numThread_ += 1;
	started_ = true;
}

void Thread::join()
{
	assert(started_);  // started_ must be set-up before calling this function;
	if(joined_)  return;
	int retErr = pthread_join(tid_, NULL);
	if(retErr){
		LOG_ERR<<"Thread::join ERROR:"<<strerror(retErr);
		return;
	}
	joined_ = true;
}

void Thread::exit()
{
	if(!started_) return;   // non-op
	pthread_exit(NULL);
}

//END THREAD CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


