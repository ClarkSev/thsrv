/****************************************************************************
* @File:	当前线程封装类
* @Date:	2020-2-10
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_CURRENTTHREAD_H
#define THSRV_BASE_CURRENTTHREAD_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include <sys/syscall.h>  // for syscall()
#include <sys/types.h>  // for pid_t ...
#include <unistd.h>     // for getpid

namespace thsrv
{

namespace CurrentThread
{
	
inline pid_t gettid()
{
	return static_cast<pid_t>(syscall(SYS_gettid));  // get the only sign of tid
}
inline pid_t tid()
{
	return gettid();
}
inline bool isMainThread()
{
	return tid()==::getpid();
}
	
} //END CurrentThread NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_CURRENTTHREAD_H

