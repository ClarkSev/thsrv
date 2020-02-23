/****************************************************************************
* @File:	网络字节序封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	只支持Ipv4，不支持IPv6，后续待完善
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_INETADDRESS_H
#define THSRV_NET_INETADDRESS_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/StringPiece.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>  // for sa_family_t
// #include <>

#include <string>

namespace thsrv
{
	
namespace net
{

class InetAddress //: public copyable
{
public:
	// InetAddress(const std::string& tip, const int tport);
	// InetAddress(const char* tip, const int tport);
	InetAddress(const StringPiece& tip, const uint16_t tport, bool enIpv6 = false);  // 兼容 string与char*
	InetAddress(uint16_t tport=0, bool enLoopTest = false, bool enIpv6 = false);
	
	explicit InetAddress(const struct sockaddr_in& t_sock_addr):sockaddr_(t_sock_addr){}
	explicit InetAddress(const struct sockaddr_in6& t_sock_addr6):sockaddr6_(t_sock_addr6){}
	
	std::string toPort()const;
	std::string toIp()const;
	std::string toIpAndPort()const;
	const struct sockaddr* getSockAddr()const{
		const void* lptr = static_cast<const void*>(&sockaddr6_);
		return static_cast<const struct sockaddr*>(lptr); 
	}
	void setSockAddr4(const struct sockaddr_in& taddr){ sockaddr_ = taddr; }
	void setSockAddr6(const struct sockaddr_in6& taddr){ sockaddr6_ = taddr; }
	// void setSockAddr(const struct sockaddr* taddr);
	sa_family_t getFamily() const{ return sockaddr6_.sin6_family; }
	uint32_t toNetPort()const { return sockaddr_.sin_port; }
	uint32_t toNetIp()const { return sockaddr_.sin_addr.s_addr; }
// private: // private method
	
private: // private properity
	union{
		struct sockaddr_in sockaddr_;   // IPv4
		struct sockaddr_in6 sockaddr6_;  // IPv6
	};
};  //END InetAddress CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_INETADDRESS_H

