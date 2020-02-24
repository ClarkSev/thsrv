/****************************************************************************
* @File:	socket请求接收器，专供TcpSever使用
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：该类使用accept4进行接收，不考虑兼容问题。
	同时，该类内部含Socket、Channel成员，用于处理新连接请求
	，并将回调请求处理函数。
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_ACCEPTOR_H
#define THSRV_NET_ACCEPTOR_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"
#include "thsrv/net/InetAddress.h"
#include "thsrv/net/Socket.h"
#include "thsrv/net/Channel.h"

#include <functional>

namespace thsrv
{
	
namespace net
{
	
class EventLoop;
	
class Acceptor : public noncopyable
{
public:  // public properity
	typedef std::function<void(int,const InetAddress&)>NewConnectCallback;

public:  // public method
	//ctor.
	Acceptor(EventLoop* tloop,const InetAddress& taddr);
	~Acceptor();
	void listen();
	bool listening()const { return listening_ == true; }
	// called by tcpserver
	void setNewConnectionCallback(NewConnectCallback cb){ newConncb_ = cb; }
	int fd()const{ return sockfd_.fd(); }
private:  // private method
	void handleRead();
private:  // private properity	
	EventLoop* loop_;
	bool listening_;
	Socket sockfd_;
	Channel acceptChannel_;
	NewConnectCallback newConncb_;
	
};  //END Acceptor CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_ACCEPTOR_H

