/****************************************************************************
* @File:	TcpServer封装类
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TcpServer.h"

#include "thsrv/base/Logger.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/net/Socket.h"
#include "thsrv/net/TcpConnection.h"

#include <iostream>  // placeholders;

namespace thsrv
{

namespace net
{

using namespace std::placeholders;

// typedef std::shared_ptr<TcpConnection>TcpConnectionPtr;
// typedef std::function<void(const TcpConnectionPtr&)>ConnectionCallback;
// typedef std::function<void(const TcpConnectionPtr&, Buffer&,const size_t)>MessageCallback;
// typedef ConnectionCallback  CloseCallback;
// typedef ConnectionCallback  WriteCompleteCallback;


/// START CLASS

TcpServer::TcpServer(EventLoop* loop,const std::string& tnm,const InetAddress& taddr):\
loop_(loop),
name_(tnm),
acceptor_(new Acceptor(loop, taddr)),
poller_(new Poller(loop))
{
	acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnect, this, _1, _2));
}

void TcpServer::start()
{
	loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get() ));
}

/// private method and interface
void TcpServer::newConnect(int tfd,const InetAddress& peeraddr)
{
	// create new TcpConnection
	loop_->assertInLoopThread();
	
	char buf[32]={0};
	snprintf(buf, sizeof(buf), "#%u", tfd);
	std::string newConnName = name_ + std::string(buf);
	LOG_INFO<<"newconn: "<<peeraddr.toIpAndPort()<<"["<<newConnName<<"]";
	
	InetAddress localaddr(socketops::getLocalAddr(tfd));
	// LOG_DEBUG<<" localaddr = "<<localaddr.toIpAndPort()<<" -- peeraddr = "<<peeraddr.toIpAndPort();
	
	// create TcpConnection guard by shared_ptr
	TcpConnectionPtr conn(new TcpConnection(loop_, newConnName, tfd, localaddr, peeraddr));  
	connects_[newConnName] = conn;
	conn->setConnectionCallback(conncb_);
	conn->setOnMessageCallback(msgcb_);
	conn->setWriteCompleteCallback(writeCompletecb_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
	loop_->runInLoop(std::bind(&TcpConnection::onConnection, conn.get()));   // FIXME: set loop_
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();

	LOG_INFO<<"From "<<conn->getPeerAddr().toIpAndPort()<<"["<<name_<<"] remove connection "\
			<<conn->getLocalAddr().toIpAndPort()<<"["<<conn->getName()<<"]";

	EventLoop* ioLoop = conn->getThisLoop();
	connects_.erase(conn->getName());
	// conn->closeConnection();
	ioLoop->runInLoop(std::bind(&TcpConnection::closeConnection, conn.get()));
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


