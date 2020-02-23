/****************************************************************************
* @Date:	2020-2-5
* @Author:	T.H.
* @Note:	不可复制对象基类
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_NONCOPYABLE_H
#define THSRV_BASE_NONCOPYABLE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

namespace thsrv
{
	
class noncopyable
{
public:
	noncopyable(const noncopyable&)=delete;     // 禁止对象拷贝构造，其目的禁止浅层拷贝
	void operator=(const noncopyable&)=delete;  // 禁止对象赋值运算符
public:
	noncopyable()=default;
	~noncopyable()=default;
	
};  //END NONCOPYABLE CLASS	
	
}  //END THSRV NAMESPACE

#endif  //END THSRV_BASE_NONCOPYABLE_H
