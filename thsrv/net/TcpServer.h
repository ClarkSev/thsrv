/****************************************************************************
* @File:	TcpServer 服务器封装类
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TCPSERVER_H
#define THSRV_NET_TCPSERVER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"
#include "thsrv/net/InetAddress.h"
#include "thsrv/net/Acceptor.h"
#include "thsrv/net/Poller.h"
#include "thsrv/net/Callback.h"

#include <functional>
#include <string>
#include <memory>
#include <map>

namespace thsrv
{
	
namespace net
{

class EventLoop;
	
class TcpServer : public noncopyable
{
public: // public properity
	typedef std::map<std::string,TcpConnectionPtr>ConnectMap;
	
public:  // public method
	TcpServer(EventLoop* loop, const std::string& tnm,const InetAddress& peeraddr);
	void start();
	void setConnectionCallback(const ConnectionCallback& cb){ conncb_ = cb; }
	void setOnMessageCallback(const MessageCallback& cb){ msgcb_ = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompletecb_ = cb; }
	
private:  // private method
	void newConnect(int tfd,const InetAddress& peeraddr);
	void removeConnection(const TcpConnectionPtr& conn);
	void removeConnectionInLoop(const TcpConnectionPtr& conn);
	
private:  // private properity
	EventLoop* loop_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	std::unique_ptr<Poller> poller_;
	ConnectMap connects_;
	
	// callback function
	ConnectionCallback conncb_;
	MessageCallback msgcb_;
	WriteCompleteCallback writeCompletecb_;
	
};  //END TcpServer CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TCPSERVER_H

