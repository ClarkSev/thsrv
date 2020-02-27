/****************************************************************************
* @File:	Connector封装类
* @Date:	2020-2-20
* @Author:	T.H.
* @Note:	专用于TcpClient连接请求管理

* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_CONNECTOR_H
#define THSRV_NET_CONNECTOR_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/InetAddress.h"
#include "thsrv/net/Channel.h"

#include <functional>
#include <memory>

namespace thsrv
{
	
namespace net
{
	
class EventLoop;

class Connector
{
public:  // public properity
   typedef std::function<void(int, const InetAddress&)>NewConnectCallback;
    enum conn_state_t{ kDisconnect, kConnecting, kConnected };

public:
    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();
	void start();
    void restart();
    void stop();
    InetAddress getServerAddr()const{ return serverAddr_; }
    // called by TcpClient
    void setNewConnectionCallback(const NewConnectCallback& cb){ newConncb_ = cb; }

private:   // private method and interface
    void startInLoop();
    void restartInLoop();
    void stopInLoop();
    void try_connect(const int tfd);
    void connect(const int tfd);
    void retry(const int tfd);
    void set_state(conn_state_t state){ state_ = state; }
    void handleWrite();
    // 因为一个fd只有有一个channel,那么使用connector连接到之后，
    // 就将channel删除，并传递给TcpConnection掌管   
    int deliveryPowerInLoop();   
    void resetConnectChannelInLoop();
private:   // private properity
    const double kMaxDelaySecond  = 30;  // 最长重试间隔时间 30s
    const double kInitDelaySecond = 0.5;   // 重试的间隔  500ms

    EventLoop* loop_;
    bool start_;
    conn_state_t state_;
    std::unique_ptr<Channel> connectChannel_;
    const InetAddress serverAddr_;
    NewConnectCallback newConncb_;

};  //END Connector CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_CONNECTOR_H

