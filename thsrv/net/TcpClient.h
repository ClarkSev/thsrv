/****************************************************************************
* @File:    TcpClient封装类
* @Date:	2020-2-21
* @Author:	T.H.
* @Note:	
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TCPCLIENT_H
#define THSRV_NET_TCPCLIENT_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Callback.h"
#include "thsrv/net/Connector.h"

#include <functional>

namespace thsrv
{
	
namespace net
{

class EventLoop;	

class TcpClient
{
public:
    TcpClient(EventLoop* loop,const InetAddress& serverAddr);

    void connect();     // thread-safe, it can be called in any thread.
    void disconnect();  // thread-safe
    void reconnect();   // thread-safe
    bool started()const { return start_ == true; }
    void send(const char* tchr, size_t len = 0);  // thread safe
    void send(std::string& tstr);  // thread safe

    // set-up function isn't thread-safe,
    // 但是在实际使用时，TcpClient通常会封装成一个应用的client且回调函数不会暴露给实际应用中
    void setConnectionCallback(const ConnectionCallback& cb){ conncb_ = cb; }
    void setOnMessageCallback(const MessageCallback& cb){ msgcb_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb){ writeCompletecb_ = cb; }

private: // private method and interface
    void newConnection(int tfd,const InetAddress& serverAddr);
    void disconnectInLoop(const TcpConnectionPtr& conn);
    void reconnectInLoop(const TcpConnectionPtr& conn);

private: // private properity
	EventLoop* loop_;
    bool start_;
    std::unique_ptr<Connector> connector_;
    TcpConnectionPtr conn_;

    ConnectionCallback conncb_;
    MessageCallback msgcb_;
    WriteCompleteCallback writeCompletecb_;
};  //END TcpClient CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TCPCLIENT_H

