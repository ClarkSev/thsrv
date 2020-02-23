/****************************************************************************
* @File:	无锁原子操作封装类
* @Date:	2020-2-11
* @Author:	T.H.
* @Note:	功能说明：使用无锁结构实现，自带库atomic类内部可能使用自旋锁实现，
	效率较低，故不使用自带的atomic模板。
	__sync_fetch_and_add等函数是linux系统下实现无锁结构的相关函数，和c++库中
	atomic<T>aInt 中的 aInt.compare_exchange_weak(...)作用相似，使用memory barrier
	实现无锁
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_ATOMIC_H
#define THSRV_BASE_ATOMIC_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"

#include <stdio.h>

namespace thsrv
{
	
namespace base
{

template<typename T>	
class Atomic : public noncopyable
{
public:
	explicit Atomic():val_(0){}
	T fetchAndAdd(T tval) volatile noexcept
	{
		return __sync_fetch_and_add(&val_, tval);
	}
	T fetchAndSub(T tval) volatile noexcept
	{
		return __sync_fetch_and_sub(&val_, tval);
	}
	T store(T tval) volatile noexcept  // return old value
	{
		return __sync_lock_test_and_set(&val_, tval);
	}
	T load() volatile noexcept
	{
		return __sync_fetch_and_add(&val_, 0);
	}
	// Atomic operator=(T val)
	// {
		
	// }
private:
	volatile T val_;
};  //END Atomic CLASS

}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_ATOMIC_H

