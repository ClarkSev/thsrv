/****************************************************************************
* @File:	socket请求接收器，专供TcpSever使用
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Acceptor.h"
#include "thsrv/net/EventLoop.h"

#include <assert.h>

namespace thsrv
{

namespace net
{

/// START CLASS
Acceptor::Acceptor(EventLoop* tloop,const InetAddress& taddr):\
loop_(tloop),
listening_(false),
sockfd_(taddr),
acceptChannel_(tloop, sockfd_.fd())
{
	assert(sockfd_.fd()>=0);
	sockfd_.bind(taddr);
	// LOG_INFO<<" Acceptor ctor. addr = "<<taddr.toIpAndPort();
	acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	listening_ = false;
}
void Acceptor::listen()
{
	if(listening_)  return;
	sockfd_.listen();
	listening_ = true;
	acceptChannel_.enableRead();
}
void Acceptor::handleRead()
{
	loop_->assertInLoopThread();
	assert(listening_);
	InetAddress peeraddr;
	int connfd = sockfd_.accept(&peeraddr);
	if(newConncb_){
		newConncb_(connfd, peeraddr);
	}else{ 
		socketops::close(connfd);
	}
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


