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

#include <sys/types.h>  // for pid_t ...

namespace thsrv
{

namespace CurrentThread
{

extern __thread pid_t cacheTid_;   // 使用变量将每个进程的 tid 存储起来， 以防止每次调用tid时陷入内核

void cacheTid();

inline pid_t tid()
{
	if(__builtin_expect(cacheTid_==0, 0)){
		cacheTid();
	}
	return cacheTid_;
}

	
} //END CurrentThread NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_CURRENTTHREAD_H

