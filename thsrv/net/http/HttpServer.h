/****************************************************************************
* @File:	Http服务器
* @Date:	2020-5-9
* @Author:	T.H.
* @Note:	功能说明：使用HttpParse解析Http请求报文，并返回响应报文
    用户可设置回调函数实现需要返回的内容
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef  THSRV_NET_HTTPSERVER_H
#define  THSRV_NET_HTTPSERVER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TcpServer.h"

#include <functional>

namespace thsrv
{

namespace net
{

class HttpParse;
class HttpResponse;

class HttpServer
{
public:
    typedef std::function<void(const HttpParse&, HttpResponse&)>HttpCallback;

public:
    HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr);

    void setHttpCallback(const HttpCallback& cb) { httpcb_ = cb; }
    void start(){ server_.start(); }

private:
    // void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer& buf, TimeStamp receiveTime);
    void onRequest(const TcpConnectionPtr& conn, HttpParse& req);

private:
    TcpServer server_;
    HttpCallback httpcb_;
};  // END CLASS

}  // END NAMESPACE NET

}  // END NAMESPACE THSRV

#endif  // THSRV_NET_HTTPSERVER_H
