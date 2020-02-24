/****************************************************************************
* @File:	TcpClient封装类
* @Date:	2020-2-21
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TcpClient.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/Socket.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/net/TcpConnection.h"


#include <iostream>

namespace thsrv
{

namespace net
{

using namespace std::placeholders;

/// START CLASS
TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr):\
loop_(loop),
start_(false),
connector_(new Connector(loop, serverAddr))
{
    // connector_ 的生命期肯定比TcpClient的短，所以调用时 this 一定存活
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1 , _2));
}
void TcpClient::connect()
{
    loop_->runInLoop(std::bind(
        &Connector::start, connector_.get()
    ));
}
void TcpClient::disconnect()
{
    if(!start_) return;
    loop_->runInLoop(std::bind(
        &TcpClient::disconnectInLoop, this, conn_
    ));
}
void TcpClient::reconnect()
{
    loop_->runInLoop(std::bind(
        &TcpClient::reconnectInLoop, this, conn_
    ));
}
void TcpClient::send(const char* tchr, size_t len)
{
    assert(start_);
    conn_->send(tchr, len);
}
void TcpClient::send(std::string& tstr)
{
    assert(start_);
    conn_->send(tstr);
}
// private method
void TcpClient::newConnection(int tfd,const InetAddress& serverAddr)
{
    loop_->assertInLoopThread();
    start_ = true;
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "Client#%u", tfd);
    LOG_INFO<<"Create newConnection:"<<buf;

    InetAddress localAddr(socketops::getLocalAddr(tfd));

    // LOG_INFO<<"TcpServer::newConnect localaddr = "<<localAddr.toIpAndPort()
	// 		<<" --> serveraddr = "<<serverAddr.toIpAndPort();

    conn_.reset(new TcpConnection(loop_, std::string(buf), tfd, localAddr, serverAddr));
    conn_->setConnectionCallback(conncb_);
    conn_->setOnMessageCallback(msgcb_);
    conn_->setWriteCompleteCallback(writeCompletecb_);
    // manual close the connection which differ from server
    conn_->setCloseCallback(std::bind(
            &TcpClient::disconnectInLoop, this, _1
    ));

    loop_->runInLoop(std::bind(
        &TcpConnection::onConnection, conn_.get()
    ));
}
void TcpClient::disconnectInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    start_ = false;
    connector_->stop();
    conn->closeConnection();
}
void TcpClient::reconnectInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    start_ = false;
    // 将当前TcpConnection状态设置为断开，然后进行重启
    conn_->set_conn_state(TcpConnection::kDisconnected);
    connector_->restart();
    start_ = true;
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


