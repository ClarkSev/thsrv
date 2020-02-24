/****************************************************************************
* @File:	socket文件描述符封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	功能说明：将socket进行封装，外部使用时不需要进行错误检查
	注：没有考虑兼容问题，默认库支持 accept4
	目前只支持 IPv4
	accept 传递参数时，一定要初始化参数，否则可能出现 Invalid argument，是否
	正常运行全凭运气。
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Socket.h"
#include "thsrv/base/Logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>   // for strerror()

namespace thsrv
{

namespace net
{
	
namespace socketops
{

/// 类型转换器
struct sockaddr* sockaddr_cast(struct sockaddr_in* taddr)
{
	void* lptr = static_cast<void*>(taddr);
	return static_cast<struct sockaddr*>(lptr);
}
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* taddr)
{
	const void* lptr = static_cast<const void*>(taddr);
	return static_cast<const struct sockaddr*>(lptr);
}
struct sockaddr* sockaddr_cast(struct sockaddr_in6* taddr)
{
	void* lptr = static_cast<void*>(taddr);
	return static_cast<struct sockaddr*>(lptr);
}
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* taddr)
{
	const void* lptr = static_cast<const void*>(taddr);
	return static_cast<const struct sockaddr*>(lptr);
}
struct sockaddr_in* sockaddr_in_cast(struct sockaddr* taddr)
{
	void* lptr = static_cast<void*>(taddr);
	return static_cast<struct sockaddr_in*>(lptr);
}
struct sockaddr_in6* sockaddr_in6_cast(struct sockaddr* taddr)
{
	void* lptr = static_cast<void*>(taddr);
	return static_cast<struct sockaddr_in6*>(lptr);
}
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* taddr)
{
	const void* lptr = static_cast<const void*>(taddr);
	return static_cast<const struct sockaddr_in*>(lptr);
}
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* taddr)
{
	const void* lptr = static_cast<const void*>(taddr);
	return static_cast<const struct sockaddr_in6*>(lptr);
}

/// 基本操作函数
int createSocketNonBlock(sa_family_t taf)
{
	int fd = ::socket(taf, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if(fd < 0){
		LOG_FATAL<<"ERROR:"<<strerror(errno);
	}
	return fd;
}
// void setKeepAlive(const int tfd)  // 设置心跳包
// {
// }
void setReuseSocket(const int tfd)
{
	int opt;
	if(::setsockopt(tfd, SOL_SOCKET, SO_REUSEADDR,&opt,sizeof(opt))<0){
		LOG_FATAL<<"ERROR:"<<strerror(errno);
	}
}
void bind(int tsockfd,const struct sockaddr* taddr)
{
	if(::bind(tsockfd,taddr,static_cast<socklen_t>(sizeof(struct sockaddr_in6)) )!=0){
		LOG_FATAL<<"ERROR:"<<strerror(errno);
	}
}
void listen(int tsockfd)
{
	if(::listen(tsockfd,SOMAXCONN)!=0){
		LOG_FATAL<<"ERROR:"<<strerror(errno);
	}
}
ssize_t read(int tfd, void* tbuf, size_t tcnt)
{
	ssize_t rdlen = ::read(tfd ,tbuf ,tcnt);
	// handle the error by TcpConnection
	// if(rdlen<0){
	// 	LOG_FATAL<<"ERROR:"<<strerror(errno);
	// }
	return rdlen;
}
ssize_t readv(int tfd, const struct iovec* tiov,int tiovcnt)
{
	ssize_t rdlen = ::readv(tfd, tiov,tiovcnt);
	// handle the error by TcpConnection
	// if(rdlen<0){
	// 	LOG_FATAL<<"ERROR:"<<strerror(errno);
	// }
	return rdlen;
}
ssize_t send(int tsockfd, const void* tbuf,size_t tlen)
{
	ssize_t sdlen = ::send(tsockfd, tbuf, tlen, 0);
	if(sdlen<0){
		LOG_FATAL<<"ERROR:"<<strerror(errno);
	}
	return sdlen;
}
// 仿照 muduo 库实现
int accept(int tsockfd, struct sockaddr* taddr, socklen_t *taddrlen)
{
	// int ret = ::accept(tsockfd, taddr, taddrlen);
	int ret = ::accept4(tsockfd, taddr, taddrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	// struct sockaddr_in *addr4 = sockaddr_in_cast(taddr);
	// LOG_DEBUG<<" *addr4 = "<<inet_ntoa(addr4->sin_addr);
	if(ret<0){
		int savedErr = errno;
		switch(savedErr){
		  case EAGAIN:
		  case ECONNABORTED:
		  case EINTR:
		  case EPROTO: // ???
		  case EPERM:
		  case EMFILE: // per-process lmit of open file desctiptor ???
			// expected errors
			errno = savedErr;
			LOG_WARN << " ignore error of ::accept " << strerror(savedErr);
			break;
		  case EBADF:
		  case EFAULT:
		  case EINVAL:
		  case ENFILE:
		  case ENOBUFS:
		  case ENOMEM:
		  case ENOTSOCK:
		  case EOPNOTSUPP:
			// unexpected errors
			LOG_FATAL << "unexpected error of ::accept " << strerror(savedErr);
			break;
		  default:
			LOG_FATAL << "unknown error of ::accept " << strerror(savedErr);
			break;
		}
	}
	return ret;
}
int connect(int tsockfd, const struct sockaddr* taddr)
{
	// be careful the value of length
	return ::connect(tsockfd, taddr, static_cast<socklen_t>(sizeof(struct sockaddr)));
}
void close(int tsockfd)
{
	if(::close(tsockfd)!=0){
		LOG_ERR<<"ERROR:"<<strerror(errno);
	}
}
void shutdownWrite(int tsockfd)
{
	if(::shutdown(tsockfd, SHUT_WR)!=0){
		LOG_ERR<<"ERROR:"<<strerror(errno);
	}
}
InetAddress getLocalAddr(const int tfd)
{
	struct sockaddr_in localaddr;   // FIXME: ipv6
	bzero(&localaddr, sizeof(localaddr));  // must be initialized
	socklen_t tlen = static_cast<socklen_t>(sizeof(localaddr)); // be careful the value
	int ret = ::getsockname(tfd, socketops::sockaddr_cast(&localaddr), &tlen);
	if(ret<0){
		LOG_ERR<<" ERROR:"<<strerror(errno); return InetAddress(0);
	}
	return InetAddress(localaddr);
}
InetAddress getPeerAddr(const int tfd)
{
	struct sockaddr_in peeraddr;  // FIXME: ipv6
	bzero(&peeraddr, sizeof(peeraddr));  // must be initialized
	socklen_t tlen = static_cast<socklen_t>(sizeof(peeraddr));  // must be careful the value
	int ret = ::getpeername(tfd, socketops::sockaddr_cast(&peeraddr), &tlen);
	if(ret<0){
		LOG_ERR<<" ERROR: "<<strerror(errno); return InetAddress(0);
	}
	return InetAddress(peeraddr);
}


}  // END SOCKETOPS NAMESPACE

/// START Socket CLASS
Socket::Socket(const InetAddress& taddr):\
sockfd_( socketops::createSocketNonBlock(taddr.getFamily()) )
{
	assert(sockfd_>=0);
	socketops::setReuseSocket(sockfd_);
}
Socket::~Socket()
{
	socketops::close(sockfd_);
}
void Socket::bind(const InetAddress &taddr)
{
	socketops::bind(sockfd_, taddr.getSockAddr());
}
void Socket::listen()
{
	socketops::listen(sockfd_);
}
ssize_t Socket::recv(void* tbuf, size_t tcnt)
{
	return socketops::read(sockfd_, tbuf, tcnt);
}
ssize_t Socket::send(const void* tbuf,size_t tlen)
{
	return socketops::send(sockfd_, tbuf, tlen);
}
int Socket::accept(InetAddress *peeraddr)
{
	struct sockaddr_in laddr;  // FIXME: ipv6
	bzero(&laddr, sizeof(laddr));
	socklen_t tlen = static_cast<socklen_t>(sizeof(laddr));  // be careful the value
	int lfd = socketops::accept(sockfd_, socketops::sockaddr_cast(&laddr), &tlen);
	if(lfd>0){
		peeraddr->setSockAddr4(laddr); 
	}
	return lfd;
}
void Socket::connect(const InetAddress &taddr)
{
	int ret = socketops::connect(sockfd_, taddr.getSockAddr());
	if(ret<0){
		LOG_ERR<<"ERROR:"<<strerror(ret);
	}
}

/// END Socket CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


