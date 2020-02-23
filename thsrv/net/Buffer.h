/****************************************************************************
* @File:	buffer class
* @Date:	2020-2-17
* @Author:	T.H.
* @Note:	使用vector实现buffer，该类是字符存储buffer，主要用于网络传输
	时的数据缓存；
	rdbuf_ 表示向socket读取数据的缓存器，wtbuf_ 向socket发送数据的缓存器
	
	注：使用默认的复制构造函数，该函数主要是内部vector的copy，是深拷贝
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
	
	
	/// 异步写入socket
	void appendInWtBuf(const char* buf, size_t tlen);
	void appendInWtBuf(const std::string& tstr);
	// 终端向缓存器中索取数据
	void retrieveFromWtBuf(size_t tlen);
	void retrieveAllFromWtBuf();
	std::string retrieveToStringFromWtBuf(size_t tlen);
	std::string retrieveAllToStringFromWtBuf();
	
	/// 异步读取socket
	ssize_t readFd(const int tfd, int* saveErrno);  // return the length of data
	// 读取从socket接收到的数据
	size_t getReadBuffer(char* buf, size_t tlen=0);
	std::string getReadBufferToString();
	void appendInRdBuf(const char* buf, size_t tlen)
	{
		if(tlen >= appendable_rdbuf()){
			makeSpaceRdBuf();
		}
		rdbuf_.assign(buf, buf+tlen);
	}
	
	char* beginWriteInBuf()
	{
		return &*wtbuf_.begin() + bufWritebytes_;
	}
	char* beginReadInBuf()
	{
		return &*rdbuf_.begin() + bufReadbytes_;
	}
	char* beginWriteOutBuf()
	{
		return &*wtbuf_.begin() + shWritebytes_;
	}
	char* beginReadOutBuf()
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
	// 终端已读数据
	size_t shReadbytes_;
	// 终端已提取数据
	size_t shWritebytes_;
	// buffer
	size_t bufReadbytes_;
	size_t bufWritebytes_;
	
	std::vector<char>rdbuf_;
	std::vector<char>wtbuf_;
	
};
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif  // END THSRV_NET_BUFFER_H

