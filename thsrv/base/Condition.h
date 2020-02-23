/****************************************************************************
* @File:	条件变量封装类
* @Date:	2020-2-12
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_CONDITION_H
#define THSRV_BASE_CONDITION_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/Mutex.h"

#include <stdio.h>

namespace thsrv
{
	
namespace base
{
	
class Condition
{
public:
	Condition(MutexLock& tmx);
	~Condition();
	
	void wait();
	bool waitForSecond(double tsec);
	void notifyOne();
	void notifyAll();
private:
	MutexLock* mx_;
	pthread_cond_t cond_;
};  //END Condition CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_CONDITION_H

