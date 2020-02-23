/****************************************************************************
* @File:	buffer class
* @Date:	2020-2-17
* @Author:	T.H.
* @Note:	ʹ��vectorʵ��buffer���������ַ��洢buffer����Ҫ�������紫��
	ʱ�����ݻ��棻
	rdbuf_ ��ʾ��socket��ȡ���ݵĻ�������wtbuf_ ��socket�������ݵĻ�����
	
	ע��ʹ��Ĭ�ϵĸ��ƹ��캯�����ú�����Ҫ���ڲ�vector��copy�������
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
	
	
	/// �첽д��socket
	void appendInWtBuf(const char* buf, size_t tlen);
	void appendInWtBuf(const std::string& tstr);
	// �ն��򻺴�������ȡ����
	void retrieveFromWtBuf(size_t tlen);
	void retrieveAllFromWtBuf();
	std::string retrieveToStringFromWtBuf(size_t tlen);
	std::string retrieveAllToStringFromWtBuf();
	
	/// �첽��ȡsocket
	ssize_t readFd(const int tfd, int* saveErrno);  // return the length of data
	// ��ȡ��socket���յ�������
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
	// �ն��Ѷ�����
	size_t shReadbytes_;
	// �ն�����ȡ����
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

