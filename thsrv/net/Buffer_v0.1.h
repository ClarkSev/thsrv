/****************************************************************************
* @File:	buffer class
* @Date:	2020-2-17
* @Author:	T.H.
* @Note:	该类采用双缓存机制实现，且使用vector作为存储容器，实现可根据需求进行
	扩展；其中rdbuf表示用户从socket接收到的数据存放的buffer，wtbuf表示用户向socket
	发送的数据buffer；在内部shell会自动从wtbuf中取出一定量的数据，并发送给socket；
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_BUFFER_H
#define THSRV_NET_BUFFER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/copyable.h"
#include "thsrv/base/StringPiece.h"

// #include <stdio.h>
#include <string.h>

#include <vector>
#include <string>

namespace thsrv
{
	
namespace net
{

class Buffer : public copyable
{
public:
	// ctor. ---- copy and move ctor. use default function.
	explicit Buffer(int initSz = 1024):shReadbytes_(0),shWritebytes_(0),
	bufReadbytes_(0),bufWritebytes_(0)
	{
		rdbuf_.resize(initSz);
		wtbuf_.resize(initSz);
	}
	void swap(Buffer& others)
	{
		std::swap(shReadbytes_, others.shReadbytes_);
		std::swap(shWritebytes_, others.shWritebytes_);
		std::swap(bufReadbytes_, others.bufReadbytes_);
		std::swap(bufWritebytes_, others.bufWritebytes_);
		std::swap(rdbuf_, others.rdbuf_);
		std::swap(wtbuf_, others.wtbuf_);
	}
	
	size_t readable() const{ return bufReadbytes_ - shReadbytes_; }
	size_t writable() const{ return bufWritebytes_ - shWritebytes_; }
	size_t appendable_rdbuf() const{ return rdbuf_.size() - bufReadbytes_; }
	size_t appendable_wtbuf() const{ return wtbuf_.size() - bufWritebytes_; }
	
	
	/// 本地向socket发送数据，等待shell将数据发送出去
	void appendInWtBuf(const char* buf, size_t tlen);
	void appendInWtBuf(const std::string& tstr);
	// shell从buffer中获取需要发送的数据
	void retrieveFromWtBuf(size_t tlen);
	void retrieveAllFromWtBuf();
	std::string retrieveToStringFromWtBuf(size_t tlen);
	std::string retrieveAllToStringFromWtBuf();
	
	/// 从socket中读取数据
	ssize_t readFd(const int tfd, int* saveErrno);  // return the length of data
	// shell从buffer中读取接收到的数据
	size_t getReadBuffer(char* buf, size_t tlen=0);
	std::string getReadBufferToString();

	// 从rdbuf中查找CRLF，并返回相应的地址----- for httpParse
	const char* findCRLFReadBuf();
	const char* findCRLFReadBuf(const char* start);
	std::string getlineReadBuf();
	std::string getlineReadBuf(const char* start);

	void appendInRdBuf(const char* buf, size_t tlen)
	{
		if(tlen >= appendable_rdbuf()){
			makeSpaceRdBuf();
		}
		rdbuf_.assign(buf, buf+tlen);
		bufReadbytes_ += tlen;
	}
	
	char* beginWriteInBuf()
	{
		return &*wtbuf_.begin() + bufWritebytes_;
	}
	const char* beginWriteInBuf()const
	{
		return &*wtbuf_.begin() + bufWritebytes_;
	}
	char* beginReadInBuf()
	{
		return &*rdbuf_.begin() + bufReadbytes_;
	}
	const char* beginReadInBuf()const
	{
		return &*rdbuf_.begin() + bufReadbytes_;
	}
	char* beginWriteOutBuf()
	{
		return &*wtbuf_.begin() + shWritebytes_;
	}
	const char* beginWriteOutBuf()const
	{
		return &*wtbuf_.begin() + shWritebytes_;
	}
	char* beginReadOutBuf()
	{
		return &*rdbuf_.begin() + shReadbytes_;
	}
	const char* beginReadOutBuf()const
	{
		return &*rdbuf_.begin() + shReadbytes_;
	}
	
private:  // private method
	void makeSpaceRdBuf()
	{
		size_t lszbuf = rdbuf_.size();
		rdbuf_.resize(lszbuf*2);
	}
	void makeSpaceWtBuf()
	{
		size_t lszbuf = wtbuf_.size();
		wtbuf_.resize(lszbuf*2);
	}
private:
	// shell可从rdbuf中获取的字节
	size_t shReadbytes_;
	// shell可从wtbuf中获取的字节
	size_t shWritebytes_;
	// buffer
	size_t bufReadbytes_;
	size_t bufWritebytes_;
	
	static const char kCRLF[];

	std::vector<char>rdbuf_;
	std::vector<char>wtbuf_;
	
};
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif  // END THSRV_NET_BUFFER_H

