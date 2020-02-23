/****************************************************************************
* @File:	TcpConnection封装类
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：该类主要负责Tcp连接的三个半事件，并通过回调函数进行处理
	一个TcpConnection含有一个Channel通信通道。
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TCPCONNECTION_H
#define THSRV_NET_TCPCONNECTION_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/noncopyable.h"
#include "thsrv/base/StringPiece.h"
#include "thsrv/net/Buffer.h"
#include "thsrv/net/InetAddress.h"
#include "thsrv/net/Callback.h"

#include <string>
#include <functional>
#include <memory>

namespace thsrv
{
	
namespace net
{

class Socket;
class Channel;
class EventLoop;	

class TcpConnection : public noncopyable,\
					  public std::enable_shared_from_this<TcpConnection>
{
public:  // public properity
	enum conn_state_t{
		kConnecting,
		kConnected,
		kDisconnecting,
		kDisconnected
	};
	
public:  // public method
	TcpConnection(EventLoop* loop,std::string tnm,int tfd,\
				   const InetAddress& localaddr, const InetAddress& peeraddr);
	~TcpConnection();
	
	// called by TcpServer. It's hiden to user.
	void setOnMessageCallback(const MessageCallback& cb){ msgcb_ = cb; }  // msg is coming.
	void setConnectionCallback(const ConnectionCallback& cb){ conncb_ = cb; }  // called by connecting and disconnecting socket
	void setCloseCallback(const CloseCallback& cb){ closecb_ = cb; }
	void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompletecb_ = cb; }
	
	EventLoop* getThisLoop() const{ return loop_; }
	const std::string& getName()const{ return name_; }
	InetAddress getLocalAddr() const{ return localaddr_; }
	InetAddress getPeerAddr() const{ return peeraddr_; }
	const Buffer& buffer() const{ return msgbuf_; }
	bool connected() const{ return state_ == kConnected; }
	bool disconnected() const{ return state_ == kDisconnected; }
	
	void send(const char* buf, const size_t len = 0);
	void send(const StringPiece& tstr);
	void send(const std::string& tstr);
	void send(const Buffer& buf);
	
	void enableRead();
	void disableRead();
	
	void onConnection();
	void closeConnection();

	void set_conn_state(conn_state_t state){ state_ = state; }  // FIXME: not thread-safe
	
private:  // private method
	/// handle events
	void handleRead();
	void handleWrite();
	void handleClose();
	void handleError();

	void shutdownInLoop();
	void sendInLoop(const void* data,const size_t tlen);
	void enableReadInLoop();
	void disableReadInLoop();
	
private:  // private properity
	
	EventLoop *loop_;
	const std::string name_;
	const InetAddress localaddr_;
	const InetAddress peeraddr_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	conn_state_t state_;   // FIXME: use Atomic Class
	Buffer msgbuf_;
	
	// callback function
	ConnectionCallback conncb_;
	MessageCallback msgcb_;
	CloseCallback closecb_;
	WriteCompleteCallback writeCompletecb_;
	
};  //END TcpConnection CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TCPCONNECTION_H

