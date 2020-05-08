/****************************************************************************
* @File:	Buffer封装类
* @Date:	2020-2-17
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/Buffer.h"
#include "thsrv/net/Socket.h"

#include <assert.h>

#include <algorithm>

namespace thsrv
{

namespace net
{

/// START CLASS

const char Buffer::kCRLF[] = "\r\n";

/// 异步写入socket
void Buffer::appendInWtBuf(const char* buf, size_t tlen)
{
	if(tlen > appendable_wtbuf()){
		makeSpaceWtBuf();
	}
	std::copy(buf, buf + tlen, beginWriteInBuf());
	bufWritebytes_ += tlen;
}
void Buffer::appendInWtBuf(const std::string& tstr)
{
	appendInWtBuf(tstr.data(), tstr.size());
}
// 终端向缓存器中索取数据
void Buffer::retrieveFromWtBuf(size_t tlen)
{
	shWritebytes_ += tlen;
	if(shWritebytes_ == bufWritebytes_){
		bufWritebytes_ = 0;
		shWritebytes_ = 0;
	}
}
void Buffer::retrieveAllFromWtBuf()
{
	bufWritebytes_ = 0;
	shWritebytes_ = 0;
}
std::string Buffer::retrieveToStringFromWtBuf(size_t tlen)
{
	std::string lstr(beginWriteOutBuf(), tlen);
	retrieveFromWtBuf(tlen);
	return lstr;
}
std::string Buffer::retrieveAllToStringFromWtBuf()
{
	std::string lstr(beginWriteOutBuf(), writable());
	retrieveAllFromWtBuf();
	return lstr;
}
/// 异步读取数据
ssize_t Buffer::readFd(const int tfd, int* saveErrno)
{
	const size_t kMaxSzExBuf = 65536;
	char extrabuf[kMaxSzExBuf];
	// 使用分散内存一次性读取fd中的所有数据
	struct iovec iobuf[2];
	iobuf[0].iov_base = beginReadInBuf();
	iobuf[0].iov_len = appendable_rdbuf();
	iobuf[1].iov_base = extrabuf;
	iobuf[1].iov_len = kMaxSzExBuf;
	
	ssize_t ret = socketops::readv(tfd, iobuf, 2);
	if(ret<0){
		if(saveErrno)  *saveErrno = errno;
		return ret;
	}else if(static_cast<size_t>(ret) > appendable_rdbuf() ){
		size_t oldBufReadbytes = bufReadbytes_;
		bufReadbytes_ = rdbuf_.size();  // appendInRdBuf内部需要再次读取最新bufReadbytes_，故需要实时更新
		appendInRdBuf(extrabuf, ret - bufReadbytes_); 
		bufReadbytes_ = oldBufReadbytes + ret;
	}else{
		bufReadbytes_ += ret;
	}
	return ret;
}
// 读取从socket接收到的数据
size_t Buffer::getReadBuffer(char* buf, size_t tlen)
{
	size_t retlen = tlen;
	if(tlen > readable() || tlen == 0)  retlen = readable();
	std::copy(buf, buf + retlen, beginReadOutBuf());
	shReadbytes_ += tlen;
	if(shReadbytes_ == bufReadbytes_){
		bufReadbytes_ = 0;
		shReadbytes_ = 0;
	}
	return retlen;
}
std::string Buffer::getReadBufferToString()
{
	std::string tstr(beginReadOutBuf(), readable());
	bufReadbytes_ = 0;
	shReadbytes_ = 0;
	return tstr;
}

// 从rdbuf中查找CRLF，并返回相应的地址----- for httpParse
const char* Buffer::findCRLFReadBuf()
{
	return findCRLFReadBuf(beginReadOutBuf());
}
const char* Buffer::findCRLFReadBuf(const char* start)
{
	const char* end = beginReadInBuf();
	assert(start<=end);
	const char* crlf = std::search(start, end, kCRLF, kCRLF+2);
	return crlf==end ? nullptr : crlf;
}
// 该函数包含 更新shReadBytes_
std::string Buffer::getlineReadBuf()
{
	return getlineReadBuf(beginReadOutBuf());
}
std::string Buffer::getlineReadBuf(const char* start)
{
	const char* end = beginReadInBuf();
	assert(start<=end);
	if(start==end) return "";
	const char* crlf = findCRLFReadBuf(start);

	if(!crlf) {
		return "";
	}else {
		// update the shReadBytes_
		size_t len = crlf - beginReadOutBuf();
		shReadbytes_ += len + 2;
		return std::string(start, crlf);
	}
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


