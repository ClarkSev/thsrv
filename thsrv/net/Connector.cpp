/****************************************************************************
* @File:	客户端连接器
* @Date:	2020-2-20
* @Author:	T.H.
* @Note:	专用于TcpClient连接请求管理
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Connector.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/Socket.h"
#include "thsrv/net/EventLoop.h"

#include <errno.h>
#include <string.h>

namespace thsrv
{

namespace net
{

/// START CLASS
Connector::Connector(EventLoop* loop, const InetAddress& serverAddr):\
loop_(loop),
start_(false),
serverAddr_(serverAddr)
{
    state_ = kDisconnect;
}

Connector::~Connector()
{
    state_ = kDisconnect;
}
void Connector::start()
{
    start_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}
void Connector::restart()
{
    loop_->runInLoop(std::bind(&Connector::restartInLoop, this));
}
void Connector::stop()
{
    start_ = false;
    loop_->runInLoop(std::bind(&Connector::stopInLoop, this));
}
void Connector::handleWrite()
{
    loop_->assertInLoopThread();
    assert(start_);
    int fd = deliveryPowerInLoop();  // 将fd控制权交由newConncb_控制
    assert(state_ == kConnected);
    if(newConncb_)  newConncb_(fd, serverAddr_);
}
int Connector::deliveryPowerInLoop()
{
    loop_->assertInLoopThread();
    state_ = kConnecting;
    int fd = connectChannel_->fd();
    resetConnectChannelInLoop();
    state_ = kConnected;
    return fd;
}
void Connector::resetConnectChannelInLoop()
{
    loop_->assertInLoopThread();
    connectChannel_->disableAll();
    connectChannel_->remove();
}
void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    int lfd = socketops::createSocketNonBlock(serverAddr_.getFamily());
    if(lfd < 0){
        LOG_FATAL<<"ERROR:"<<strerror(errno);
    }
    try_connect(lfd);
}
void Connector::restartInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    start_ = false;
    state_ = kConnecting;
    // sockfd 由 TcpConnection中的 Socket 接管
    // 而Socket会自动关闭 sockfd，无需用户控制
    // 同时 fd 已经交由newConncb_（TcpConenction设置）处理，
    // 所以connectChannel实际上已经无效了
    startInLoop();
    state_ = kConnected;
    start_ = true;
}
void Connector::stopInLoop()
{
    loop_->assertInLoopThread();
    //FIXME:
}
void Connector::try_connect(const int tfd)
{
    loop_->assertInLoopThread();
    state_ = kConnecting;
    int ret = socketops::connect(tfd, serverAddr_.getSockAddr());
    int saveErr = ret == 0? 0 : errno;
    // LOG_INFO<<"Connector::try_connect saveErr = "<<strerror(saveErr);
    switch(saveErr){
        case 0:
        case EISCONN:
        case EINTR:
        case EINPROGRESS:
            connect(tfd);break;
        case EADDRINUSE:
        case EACCES:
        case EALREADY:
        case EADDRNOTAVAIL:
        case EAGAIN:       
        case ECONNREFUSED:
            retry(tfd);break;
        case EAFNOSUPPORT:        
        case EBADF:
        case EFAULT:     
        case ENOTSOCK:
        case EPROTOTYPE:
        case ETIMEDOUT:
            LOG_FATAL<<"unexcepted error of ::connect "<<strerror(saveErr);
            socketops::close(tfd); break;
        default:
            LOG_FATAL<<"unknown error of ::connect "<<strerror(saveErr);
            socketops::close(tfd);
    }
}
void Connector::connect(const int tfd)
{
    loop_->assertInLoopThread();
    state_ = kConnected;
    connectChannel_.reset(new Channel(loop_, tfd));
    connectChannel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    connectChannel_->enableWrite(); 
}
void Connector::retry(const int tfd)
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    static int numRetry = 1;
    LOG_INFO<<"Retry to connect the "<<tfd<<" "<<numRetry<<"th";
    double delay_sec = std::min(kInitDelaySecond * 2, kMaxDelaySecond);
    numRetry++;
    if(delay_sec >= static_cast<double>(kMaxDelaySecond)){
        socketops::close(tfd);
        LOG_INFO<<"Close the socket["<<tfd<<"]";
        loop_->queueInLoop(
            std::bind(&Connector::stop, this)
        );
        numRetry = 1;
    }else{
        loop_->runAfter(
            delay_sec,
            std::bind(&Connector::try_connect, this, tfd)
        );
    }
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


