/****************************************************************************
* @File:	TcpConnection封装类
* @Date:	2020-2-16
* @Author:	T.H.
* @Note:	功能说明：
	1.关闭读功能---> 当 ::readv 返回为 0 时，表示对方已经关闭写功能，
		则该channel关闭读功能；
	2.关闭写功能---> 当写完成时，将该channel写功能关闭；用户或是服务器准备
		发送数据的同时，将写功能打开即可。
	注：当 this 指针向用户暴露且其对象的生存期可由用户控制时，需要使用
		shared_from_this保护
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TcpConnection.h"
#include "thsrv/net/Socket.h"
#include "thsrv/net/Channel.h"
#include "thsrv/net/EventLoop.h"

#include <unistd.h>
#include <string.h>  // for strerror()

namespace thsrv
{

namespace net
{

namespace
{

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
	LOG_INFO<<conn->getPeerAddr().toIpAndPort()<<" -> "\
			<<conn->getLocalAddr().toIpAndPort()<<"["<<conn->getName()<<"] : "\
			<<(conn->connected() ? "UP" : "DOWN");
}
void defaultOnMessageCallback(const TcpConnectionPtr& conn, Buffer& buf, TimeStamp receiveTime)
{
	LOG_INFO<<"Get "<< buf.getReadBufferToString().size() \
			<<" bytes data from : "<<conn->getLocalAddr().toIpAndPort();
}

}   // END NAMESPACE

using namespace std::placeholders;

/// START CLASS
//ctor.
TcpConnection::TcpConnection(EventLoop* loop, std::string tnm,int tfd,
							 const InetAddress& tlocaladdr,
							 const InetAddress& tpeeraddr):\
loop_(loop),
name_(tnm),
localaddr_(tlocaladdr),
peeraddr_(tpeeraddr),
socket_(new Socket(tfd)),
channel_(new Channel(loop_, tfd)),
state_(kConnecting),
conncb_(defaultConnectionCallback),
msgcb_(defaultOnMessageCallback)
{
	// register the callback function
	channel_->setReadCallback( std::bind(&TcpConnection::handleRead,  this, _1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

	socket_->setKeepAlive(true);
	socket_->setTcpNoDelay(true);
}
TcpConnection::~TcpConnection()
{
	// shutdownConnection();
	assert(state_ == kDisconnected);
}

void TcpConnection::setTcpNoDelay(const bool on)
{
	socket_->setTcpNoDelay(on);
}

void TcpConnection::enableRead()
{
	loop_->runInLoop(std::bind(
		&TcpConnection::enableReadInLoop,
		this   // 此处在 TcpConnection 内部使用，不需要使用 shared_from_this
	));
}
void TcpConnection::disableRead()
{
	loop_->runInLoop(std::bind(
		&TcpConnection::disableReadInLoop,
		this   // 此处在 TcpConnection 内部使用，不需要使用 shared_from_this
	));
}
void TcpConnection::send(const char* buf, const size_t len)
{
	// push the task into queue
	size_t l_len = len==0 ? strlen(buf) : len;
	loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,
		this,   // 此处在 TcpConnection 内部使用，不需要使用 shared_from_this
		buf,
		l_len
	));
}
void TcpConnection::send(const std::string& tstr)
{
	send(tstr.data(), tstr.size());
}
void TcpConnection::send(const StringPiece& tstr)
{
	send(tstr.data(), tstr.size());
}
void TcpConnection::send(const Buffer& buf)
{
	Buffer lbuf = buf;
	std::string lstr = lbuf.retrieveAllToStringFromWtBuf();
	send(lstr);
}
void TcpConnection::onConnection()
{
	loop_->assertInLoopThread();
	state_ = kConnected;
	channel_->enableRead();
	if(conncb_)  conncb_(shared_from_this()); 
}
void TcpConnection::closeConnection()
{
	if(state_ == kDisconnecting || state_ == kConnected){
		set_conn_state(kDisconnected);
		loop_->queueInLoop(std::bind(&TcpConnection::closeConnectionInLoop, shared_from_this()));
	}

}
void TcpConnection::shutdown()
{
	if(state_ == kConnected){
		set_conn_state(kDisconnecting);
		loop_->queueInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
	}
}
/// private method implementation
void TcpConnection::handleRead(TimeStamp receiveTime)
{
	// read fd and put the msg into buffer.
	loop_->assertInLoopThread();
	int saveErr = 0;
	int fd = channel_->fd();
	ssize_t nrd = msgbuf_.readFd(fd, &saveErr);
	if(nrd < 0){
		LOG_WARN<<"sockfd = "<<fd<<" handleRead:ERROR "<<strerror(saveErr);
	}else if(nrd == 0){
		// LOG_INFO<<"TcpConnection::handleRead nrd==0";
		handleClose();   // close TcpConnection
	}else{
		msgcb_(shared_from_this(), msgbuf_, receiveTime );   
	}
}
void TcpConnection::handleWrite()   // shell handle it
{
	loop_->assertInLoopThread(); 
	size_t remindBytes = msgbuf_.writable();
	ssize_t nwt = socket_->send(static_cast<void*>(msgbuf_.beginWriteOutBuf()), remindBytes);
	if(nwt<0){
		LOG_WARN<<"sockfd = "<<channel_->fd()<<" handleWrite:ERROR "<<strerror(errno);
		return;
	}else if(nwt < static_cast<ssize_t>(remindBytes) ){
		remindBytes -= nwt;
		msgbuf_.retrieveFromWtBuf(nwt);  // it has already send nwt-bytes to remote socket
	}
	if(msgbuf_.writable()==0){
		if(channel_->isWritting())  channel_->disableWrite();
		LOG_INFO<<"TcpConnection::handleWrite events = "<<channel_->eventToString(channel_->events());
		if(writeCompletecb_){
			loop_->queueInLoop(std::bind(
				&TcpConnection::writeCompletecb_, 
				shared_from_this()   // 将this指针向用户暴露时，需要使用shared_from_this保护
			));
		}
		if(state_ == kDisconnecting){   // FIXME:
			LOG_INFO<<"TcpConnection::handleWrite->state_ = kDisconnecting.";
			shutdownInLoop();
		}
	}
}

void TcpConnection::closeConnectionInLoop()
{
	loop_->assertInLoopThread();
	if(state_ == kDisconnecting || state_ == kConnected){
		handleClose();
	}
}
void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	channel_->disableAll();
	state_ = kDisconnected;
	if(conncb_) conncb_(shared_from_this());
	closecb_(shared_from_this());  // 将this指针向用户暴露时，需要使用shared_from_this
}
void TcpConnection::handleError()
{
	loop_->assertInLoopThread();
	//FIXME
	LOG_INFO<<"TcpConenction:handleError FIXME";
}

/// run in loop
void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	//shutdownWrite
	if(channel_->isWritting())
		socketops::shutdownWrite(channel_->fd());
}
void TcpConnection::sendInLoop(const void* data,const size_t tlen)
{
	loop_->assertInLoopThread();
	size_t remainBytes = tlen;
	if(!channel_->isWritting() && msgbuf_.writable() == 0){
		// directly send to peer-socket
		// LOG_INFO<<"TcpConnection::sendInLoop directly send to peer-socket.";
		ssize_t len = socket_->send(data, tlen);
		remainBytes = tlen - len;
		if(remainBytes == 0) return;
	}
	assert(remainBytes > 0);
	// push the data into outbuffer.
	// LOG_INFO<<"TcpConnection::sendInLoop push data into buffer.";
	msgbuf_.appendInWtBuf(static_cast<const char*>(data), remainBytes);
	if(!channel_->isWritting()){  
		channel_->enableWrite();
	}
}
void TcpConnection::enableReadInLoop()
{
	loop_->assertInLoopThread();
	channel_->enableRead();
}
void TcpConnection::disableReadInLoop()
{
	loop_->assertInLoopThread();
	channel_->disableRead();
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


