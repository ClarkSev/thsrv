/****************************************************************************
* @File:	socket文件描述符封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_SOCKET_H
#define THSRV_NET_SOCKET_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/InetAddress.h"

#include <sys/uio.h>
#include <sys/socket.h>



namespace thsrv
{
	
namespace net
{
	
	
namespace socketops
{
	
/// 类型转换器
struct sockaddr* sockaddr_cast(struct sockaddr_in* taddr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* taddr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* taddr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* taddr);
struct sockaddr_in* sockaddr_in_cast(struct sockaddr* taddr);
struct sockaddr_in6* sockaddr_in6_cast(struct sockaddr* taddr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* taddr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* taddr);

/// 基本操作函数
int createSocketNonBlock(sa_family_t taf);
void setReuseSocket(const int tfd, const bool on);
void setTcpNoDelay(const int tfd, const bool on);
void setKeepAlive(const int tfd, const bool on);

void bind(int tsockfd,const struct sockaddr* taddr);
void listen(int tsockfd);
ssize_t read(int tfd, void* tbuf, size_t tcnt);
ssize_t readv(int tfd, const struct iovec* tiov,int tiovcnt);
ssize_t send(int tsockfd, const void* tbuf,size_t tlen);
int accept(int tsockfd, struct sockaddr* taddr, socklen_t *taddrlen, int& retErrno);
int connect(int tsockfd, const struct sockaddr* taddr);   // return errno
void close(int tsockfd);
void shutdownWrite(int tsockfd);

InetAddress getLocalAddr(const int tfd);
InetAddress getPeerAddr(const int tfd);

}  // END SOCKETOPS NAMESPACE

/// Socket类会自动关闭 sockfd ，无需用户关闭；
class Socket
{
public:
	Socket(const InetAddress& taddr);
	explicit Socket(int fd):sockfd_(fd){}

	~Socket();

	void setReuse(const bool on){ socketops::setReuseSocket(sockfd_, on); }
	void setTcpNoDelay(const bool on){ socketops::setTcpNoDelay(sockfd_, on); }
	void setKeepAlive(const bool on){ socketops::setKeepAlive(sockfd_, on); }

	void bind(const InetAddress &taddr);
	void listen();
	int accept(InetAddress* peeraddr, int& retErrno);
	void connect(const InetAddress &taddr);

	ssize_t recv(void* tbuf, size_t tcnt);
	ssize_t send(const void* tbuf,size_t tlen);

	int fd()const{ return sockfd_; }
private:
	const int sockfd_;
};  //END Socket CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_SOCKET_H

