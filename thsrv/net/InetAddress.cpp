/****************************************************************************
* @File:	网络字节序封装类
* @Date:	2020-2-13
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/InetAddress.h"
// #include "thsrv/net/Socket.h"

#include <assert.h>
#include <strings.h>   // for bzero

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInAddrAny = INADDR_ANY;
static const in_addr_t kInAddrLoopBack = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

namespace thsrv
{

namespace net
{

// 		/* Structure describing a generic socket address.  */
//		struct sockaddr
//		{
//			__SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
//			char sa_data[14];		/* Address data.  */
//		};

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

/// START CLASS

InetAddress::InetAddress(const StringPiece& tip, const uint16_t tport, bool enIpv6) // 兼容 string与char*
{
	if(enIpv6){
		// assert("IPv6 could not set-up ,using this ctor.");
		bzero(&sockaddr6_, sizeof(sockaddr6_));
		sockaddr6_.sin6_family = AF_INET6;
		sockaddr6_.sin6_port = htons(tport);
		assert(inet_pton(AF_INET6, tip.data(), &sockaddr6_.sin6_addr) == 1);
	}else{
		bzero(&sockaddr_, sizeof(sockaddr_));
		sockaddr_.sin_family = AF_INET;
		sockaddr_.sin_port = htons(tport);
		assert(inet_pton(AF_INET, tip.data(), &sockaddr_.sin_addr) == 1);
		// sockaddr_.sin_addr = inet_addr(tip.data());  // in_addr_t inet_addr(const char* cp)
	}
}
InetAddress::InetAddress(uint16_t tport, bool enLoopTest, bool enIpv6)
{
	if(enIpv6){  // ipv6
		bzero(&sockaddr6_, sizeof(sockaddr6_));
		sockaddr6_.sin6_family = AF_INET6;
		sockaddr6_.sin6_port = htons(tport);
		sockaddr6_.sin6_addr = enLoopTest ? in6addr_loopback : in6addr_any;
	}else{ // ipv4
		bzero(&sockaddr_, sizeof(sockaddr_));
		sockaddr_.sin_family = AF_INET;
		sockaddr_.sin_port = htons(tport);
		sockaddr_.sin_addr.s_addr = enLoopTest ? kInAddrLoopBack : kInAddrAny;
	}
}
// void InetAddress::setSockAddr(const struct sockaddr* taddr)
// {
	// if(taddr->sa_family == AF_INET){
		// sockaddr_ = *socketops::sockaddr_in_cast(taddr);
	// }else{
		// sockaddr6_ = *socketops::sockaddr_in6_cast(taddr);
	// }
// }
std::string InetAddress::toPort()const
{
	char buf[32] = {0};
	if(getFamily() == AF_INET){
		snprintf(buf, sizeof(buf),"%u", ntohs(sockaddr_.sin_port));
	}else{
		snprintf(buf, sizeof(buf),"%u", ntohs(sockaddr6_.sin6_port));
	}
	return buf;
}
std::string InetAddress::toIp()const
{
	//将网络二进制值转换为表达式
	if(getFamily() == AF_INET){
		char str[INET_ADDRSTRLEN];
		const char* ipv4 = inet_ntop(AF_INET, static_cast<const void*>(&sockaddr_.sin_addr),\
			str, static_cast<socklen_t>(sizeof str));
		return ipv4 == NULL ? "NULL" : ipv4;
	}else{
		char str[INET6_ADDRSTRLEN];
		const char* ipv6 = inet_ntop(AF_INET6, static_cast<const void*>(&sockaddr6_.sin6_addr),\
			str, static_cast<socklen_t>(sizeof str));
		return ipv6 == NULL ? "NULL" : ipv6;
	}
}
std::string InetAddress::toIpAndPort()const
{
	return "IP:" + toIp() + " PORT:" + toPort();
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


