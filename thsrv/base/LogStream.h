/****************************************************************************
* @Date:	2020-2-6
* @Author:	T.H.
* @Note:	日志数据流，其实质就是一个buffer。另：formatSI未实现
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_LOGSTREAM_H
#define THSRV_BASE_LOGSTREAM_H
//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

// #include "thsrv/base/TimeStamp.h"
#include "thsrv/base/noncopyable.h"
#include "thsrv/base/StringPiece.h"

#include <string.h>   // for memset/memcpy/strlen/...
#include <assert.h>   // for assert

#include <string>

namespace thsrv
{
	
namespace detail
{

// just only for LogStream class
template<int SZBUF>
class FixBuffer : public noncopyable
{
public:
	//ctor.
	FixBuffer()
	{
		memset(buf_, 0, sizeof(buf_));
		cur_ = buf_;
	}
	explicit FixBuffer(const char* tchr)
	{
		int len = strlen(tchr);
		assert(len<=SZBUF);
		memcpy(buf_, tchr, len);
		cur_ = buf_ + len;
	}
	explicit FixBuffer(const char* tchr,const size_t tlen)
	{
		assert(tlen<=SZBUF);
		memcpy(buf_, tchr, tlen);
		cur_ = buf_ + tlen;
	}
	explicit FixBuffer(const std::string& tstr)
	{
		int len = static_cast<int>(tstr.size());
		assert(len<=SZBUF);
		memcpy(buf_, tstr.data(), len);
		cur_ = buf_ + len;
	}
	// method and interface
	const char* data()const{ return buf_; }
	size_t size()const { return (cur_ - buf_); }
	size_t avail()const { return (buf_ - cur_) + SZBUF; }
	char* current()const{ return cur_; }
	
	void append(const std::string& tstr) { append(tstr.data(), tstr.size()); }
	void append(char* tchr,const size_t tlen) { appendImpl(static_cast<void*>(tchr), tlen); }
	void append(const char* tchr, const size_t tlen) { append(const_cast<char*>(tchr), tlen); }
	void append(const StringPiece& tstr){ append(tstr.data(), tstr.size()); }
	// void append(void* ptr, const int tlen);  // unused

private:  // private method
	// just for append
	void appendImpl(void* ptr,const size_t tlen)
	{
		assert(tlen<=avail());
		memcpy(cur_, static_cast<const char*>(ptr), tlen);
		cur_ += tlen;
	}	
	
private:  // private property
	char buf_[SZBUF];
	char* cur_;
};  //END FIXBUFFER CLASS


}  //END DETAIL NAMESPACE

class LogStream : public noncopyable
{

public:  // public property
	const static int kMaxSzFixBuf = 4 * 1024;
	const static int kMinSzFixBuf = 1024;
	typedef thsrv::detail::FixBuffer<kMinSzFixBuf> Buffer;

public:  // public method and interface
	// friend 破坏了其封装
	//字符型，StringPiece兼容string与char*、char的隐式转换
	LogStream& operator<<(const StringPiece& tstr);
	LogStream& operator<<(const std::string& tstr);
	LogStream& operator<<(const char* tchr);
	//布尔型
	LogStream& operator<<(bool tnum)
	{
		buf_.append(tnum ? "1" : "0", 1); 
		return *this;
	}
	//数值型
	LogStream& operator<<(double tnum);
	LogStream& operator<<(float tnum);
	
	LogStream& operator<<(short tnum);
	LogStream& operator<<(unsigned short tnum);
	LogStream& operator<<(int tnum);
	LogStream& operator<<(unsigned int tnum);
	LogStream& operator<<(long tnum);
	LogStream& operator<<(unsigned long tnum);
	LogStream& operator<<(long long tnum);
	LogStream& operator<<(unsigned long long tnum);

	Buffer& buffer(){ return buf_; }

private:  // unused	
	void append(const char* tchr, const int tlen) 
	{
		buf_.append(tchr, tlen);
	}
private:  //private property
	Buffer buf_;

};  //END LOGSTREAM CLASS	

/// 全局函数
/// 将数据转换为字符串
template<typename T,int SZBUF=64>std::string num2Str(const T tnum);

/// 将数值转换为科学计数法
std::string formatSI(int64_t tnum); 

/// 将内存数值转换为适合查看的数值
std::string formatIEC(int64_t tnum);	

}  //END THSRV NAMESPACE


#endif  // END THSRV_BASE_LOGSTREAM_H