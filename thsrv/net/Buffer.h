/****************************************************************************
* @File:	buffer class
* @Date:	2020-5-26
* @Author:	T.H.
* @Note:	使用单一 vector<char> 实现buffer， 并在buffer头添加 prepend 信息
	便于后续解码使用
* @Version:	V0.2
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
#include <assert.h>

#include <vector>
#include <string>

namespace thsrv
{
	
namespace net
{

class Buffer : public copyable
{
public:
	const static int kPrependBytes;
public:
	// ctor. ---- copy and move ctor. use default function.
	explicit Buffer(int initSz = 1024):readIdx_(kPrependBytes),\
	writeIdx_(kPrependBytes), buf_(kPrependBytes + initSz)
	{}

	void swap(Buffer& others)
	{
		std::swap(writeIdx_, others.writeIdx_);
		std::swap(readIdx_, others.readIdx_);
		buf_.swap(others.buf_);
	}
	
	size_t readable() const{ return writeIdx_ - readIdx_; }
	size_t writable() const{ return buf_.size() - writeIdx_; }
	size_t prepenable() const{ return readIdx_; }
	
	// 读取数据的起始地址
	const char* peek(){ return &*buf_.begin()+readIdx_; }

	char* beginWrite(){ return &*buf_.begin()+writeIdx_; }
	const char* beginWrite()const { return &*buf_.begin()+writeIdx_; }
	char* beginRead(){ return &*buf_.begin()+readIdx_; }
	const char* beginRead()const { return &*buf_.begin()+readIdx_; }

	// 向buffer头添加长度信息
	void prepend(const void* data, size_t tlen);

	void append(const char* buf, size_t tlen);
	void append(const std::string& tstr);
	
	void retrieve(size_t tlen);
	void retrieveAll();
	std::string retrieveToString(size_t tlen);
	std::string retrieveAllToString();
	
	void hasWritten(size_t tlen);

	/// 从socket中读取数据
	ssize_t readFd(const int tfd, int* saveErrno);  // return the length of data
	// shell从buffer中读取接收到的数据
	size_t getReadBuffer(char* buf, size_t tlen=0);
	std::string getReadBufferToString();

	// 从rdbuf中查找CRLF，并返回相应的地址----- for httpParse
	const char* findCRLF();
	const char* findCRLF(const char* start);
	std::string getLineBuffer();
	std::string getLineBuffer(const char* start);

	
private:  // private method
	// 首先检测空间是否可以通过移动满足需求，
	// 若不能，就进行resize()扩容
	void makeSpaceBuffer(size_t len)
	{
		// move the data
		assert(readIdx_>static_cast<size_t>(kPrependBytes));
		size_t l_readable = readable();
		std::copy(beginRead(), beginWrite(), buf_.begin() + kPrependBytes);
		readIdx_ = kPrependBytes;
		writeIdx_ = readIdx_ + l_readable;
		if(writable()<len){
			buf_.resize(2 * len);  // increase the space
		}
	}

private:
	static const char kCRLF[];

	size_t readIdx_;
	size_t writeIdx_;

	std::vector<char>buf_;	
};
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif  // END THSRV_NET_BUFFER_H

