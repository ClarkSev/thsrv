/****************************************************************************
* @File:	线程安全队列模板类
* @Date:	2020-2-12
* @Author:	T.H.
* @Note:	功能说明：该队列使用有锁结构与queue库实现线程安全
	1.既可实现有限大小，也可固定大小队列
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_QUEUETHREADSAFE_H
#define THSRV_BASE_QUEUETHREADSAFE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
// #include "thsrv/base/CurrentThread.h"
#include "thsrv/base/Condition.h"
#include "thsrv/base/Mutex.h"

#include <limits.h>  // for INT_MAX

#include <deque>   // for deque<>
#include <iterator>  // for iterator

namespace thsrv
{
	
namespace base
{
	
#define __MAXSIZEQUEUE__        INT_MAX
	
template<typename T>
class QueueThreadSafe
{
public:
	QueueThreadSafe();
	QueueThreadSafe(const int tsz);
	T try_pop();
	void try_push(const T& tval);
	
	void push(const T& tval);
	void pop();
	T front();
	void resize(const int tsz);
	void swap(QueueThreadSafe<T>& others);   // only swap the container
	bool full()const;
	bool empty()const;
	size_t max_size()const;
	size_t size()const;
	std::deque<T>& container()const;
	
	typename std::deque<T>::iterator begin(){ return que_.begin(); }
	typename std::deque<T>::iterator end(){ return que_.end(); }
private:
	size_t maxsize_;
	mutable MutexLock mx_;
	Condition notEmpty_;
	Condition notFull_;
	std::deque<T> que_;

};  //END QueueThreadSafe CLASS

/// non-member function
template<typename T>
void swap(QueueThreadSafe<T>& lhs, QueueThreadSafe<T>& rhs)
{
	lhs.swap(rhs);
}

/// START QueueThreadSafe CLASS
// ctor.
template<typename T>
QueueThreadSafe<T>::QueueThreadSafe():maxsize_(__MAXSIZEQUEUE__),
notEmpty_(mx_),
notFull_(mx_)
{
	// que_.resize(maxsize_);
}
template<typename T>
QueueThreadSafe<T>::QueueThreadSafe(const int tsz):\
maxsize_(static_cast<size_t>(tsz)),
notEmpty_(mx_),
notFull_(mx_)
{
	// que_.resize(maxsize_);
}
// method
template<typename T>
T QueueThreadSafe<T>::try_pop()
{
	MutexLockGuard _l(mx_);
	while(empty()){
		notEmpty_.wait();
	}
	T _val = que_.front();
	que_.pop_front();
	notFull_.notifyOne();
	return _val;
}
template<typename T>
void QueueThreadSafe<T>::try_push(const T& tval)
{
	MutexLockGuard _l(mx_);
	while(full()){
		notFull_.wait();
	}
	que_.push_back(tval);
	notEmpty_.notifyOne();
}
template<typename T>
void QueueThreadSafe<T>::push(const T& tval)
{
	MutexLockGuard _l(mx_);
	// FIXME: add log tag
	if(full()) return;
	que_.push_back(tval);
	notEmpty_.notifyOne();
}
template<typename T>
void QueueThreadSafe<T>::pop()
{
	MutexLockGuard _l(mx_);
	// FIXME: add log tag
	if(empty()) return;
	que_.pop_front();
	notFull_.notifyOne();
}
template<typename T>
T QueueThreadSafe<T>::front()
{
	T tmp;
	MutexLockGuard _l(mx_);
	if(empty())  return tmp;
	tmp = que_.front();
	return tmp;
}
template<typename T>
bool QueueThreadSafe<T>::full()const
{
	return que_.size() == maxsize_;
}
template<typename T>
bool QueueThreadSafe<T>::empty()const
{
	return que_.size() == 0;
}
template<typename T>
void QueueThreadSafe<T>::resize(int tsz)
{
	MutexLockGuard _l(mx_);
	que_.resize(static_cast<size_t>(tsz));
}
template<typename T>
void QueueThreadSafe<T>::swap(QueueThreadSafe<T>& others)
{
	// std::swap(maxsize_, others.maxsize_);
	std::swap(que_, others.que_);  // only swap the container.
}
template<typename T>
size_t QueueThreadSafe<T>::max_size()const
{
	MutexLockGuard _l(mx_);
	return maxsize_;
}
template<typename T>
size_t QueueThreadSafe<T>::size()const
{
	MutexLockGuard _l(mx_);
	return que_.size();
}
template<typename T>
std::deque<T>& QueueThreadSafe<T>::container()const
{
	MutexLockGuard _l(mx_);
	return que_;
}
/// END QueueThreadSafe CLASS
	
}  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_BASE_QUEUETHREADSAFE_H

