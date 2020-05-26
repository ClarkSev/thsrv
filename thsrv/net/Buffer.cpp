/****************************************************************************
* @File:	Buffer封装类
* @Date:	2020-5-26
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.2
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
const int Buffer::kPrependBytes = 8;
const char Buffer::kCRLF[] = "\r\n";

/// 异步写入socket
void Buffer::prepend(const void* data, size_t tlen)
{
	assert(tlen<=kPrependBytes);
	const char* d = static_cast<const char*>(data);
	readIdx_ -= tlen;
	std::copy(d, d+tlen, beginRead());
}

void Buffer::append(const std::string& tstr)
{
	append(tstr.data(), tstr.size());
}
void Buffer::append(const char* tbuf, size_t tlen)
{
	if(tlen>=writable()){
		makeSpaceBuffer(tlen - writable());
	}
	std::copy(tbuf, tbuf+tlen, beginWrite());
	hasWritten(tlen);
}

void Buffer::retrieve(size_t tlen)
{
	if(tlen>=readable()){
		retrieveAll();
	}else{
		readIdx_ += tlen;
	}
}
void Buffer::retrieveAll()
{
	readIdx_ = kPrependBytes;
	writeIdx_ = kPrependBytes;
}

std::string Buffer::retrieveToString(size_t tlen)
{
	if(readable()<tlen) return std::string();
	std::string res(peek(), tlen);
	retrieve(tlen);
	return res;
}
std::string Buffer::retrieveAllToString()
{
	return retrieveToString(readable());
}

void Buffer::hasWritten(size_t tlen)
{
	assert(tlen<=writable());
	writeIdx_ += tlen;
}

/// 异步读取数据
ssize_t Buffer::readFd(const int tfd, int* saveErrno)
{
	const size_t kMaxSzExBuf = 65536;
	char extrabuf[kMaxSzExBuf];
	// 使用分散内存一次性读取fd中的所有数据
	struct iovec iobuf[2];
	iobuf[0].iov_base = beginWrite();
	iobuf[0].iov_len = writable();
	iobuf[1].iov_base = extrabuf;
	iobuf[1].iov_len = kMaxSzExBuf;
	
	const size_t l_writable = writable();
	const int iov_cnt = (l_writable < kMaxSzExBuf) ? 2 : 1;

	ssize_t ret = socketops::readv(tfd, iobuf, iov_cnt);
	if(ret<0){
		if(saveErrno)  *saveErrno = errno;
	}else if(static_cast<size_t>(ret) > l_writable){
		append(extrabuf, ret - l_writable);
	}else{
		hasWritten(ret);
	}
	return ret;
}
// 读取从socket接收到的数据


// 从rdbuf中查找CRLF，并返回相应的地址----- for httpParse
const char* Buffer::findCRLF()
{
	return findCRLF(beginRead());
}
const char* Buffer::findCRLF(const char* start)
{
	const char* end = beginWrite();
	assert(start<=end);
	const char* crlf = std::search(start, end, kCRLF, kCRLF+2);
	return crlf==end ? nullptr : crlf;
}

std::string Buffer::getLineBuffer()
{
	return getLineBuffer(beginRead());
}
std::string Buffer::getLineBuffer(const char* start)
{
	const char* end = beginWrite();
	assert(start<=end);
	if(start==end) return "";
	const char* crlf = findCRLF(start);

	if(!crlf) {
		return "";
	}else {
		size_t len = crlf - beginRead();
		retrieve(len + 2);
		return std::string(start, crlf);
	}
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


