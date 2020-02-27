/****************************************************************************
* @File:	回调封装类
* @Date:	2020-2-17
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_CALLBACK_H
#define THSRV_NET_CALLBACK_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include <functional>
#include <memory>

namespace thsrv
{
	
namespace net
{

class Buffer;
class TcpConnection;	

typedef std::shared_ptr<TcpConnection>TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)>ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer&,const size_t)>MessageCallback;
typedef ConnectionCallback  CloseCallback;
typedef ConnectionCallback  WriteCompleteCallback;

typedef std::function<void()>TASK;
typedef TASK  TimerCallback;

}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_CALLBACK_H

