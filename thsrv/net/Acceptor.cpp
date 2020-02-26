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
#include "thsrv/base/Logger.h"
#include "thsrv/net/EventLoop.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <error.h>
#include <string.h>

namespace thsrv
{

namespace net
{

/// START CLASS
Acceptor::Acceptor(EventLoop* tloop,const InetAddress& taddr):\
loop_(tloop),
listening_(false),
sockfd_(taddr),
idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
acceptChannel_(tloop, sockfd_.fd())
{
	assert(sockfd_.fd()>=0);
	sockfd_.bind(taddr);
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
	if(connfd >= 0){
		if(newConncb_){
			newConncb_(connfd, peeraddr);
		}else{ 
			socketops::close(connfd);
		}
	}else{
		LOG_ERR<<"ERROR:"<<strerror(connfd);
		if(connfd == EMFILE){
			::close(idlefd_);
			idlefd_ = sockfd_.accept(&peeraddr);
			::close(idlefd_);
			idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC); // reset idlefd_
		}else{
			LOG_FATAL<<"other ERROR";
		}
	}

}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


