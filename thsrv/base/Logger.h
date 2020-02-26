/****************************************************************************
* @File:	日志登记器
* @Date:	2020-2-7
* @Author:	T.H.
* @Note:	功能说明：
	1.宏LOG_INFO等可直接使用，且可设置日志的输出终端（文件还是控制台）
	2.含不同的日志粒度，debug的粒度最小，可以详细观察哪一行代码出错
	
	注：目前warn的粒度与debug的粒度一致,warn未添加出错代码
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_LOGGER_H
#define THSRV_BASE_LOGGER_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include "thsrv/base/TimeStamp.h"
#include "thsrv/base/LogStream.h"

#include <string.h>  //for strrchr

namespace thsrv
{

/// public defined
#if !defined MCHECK
#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);         \
                       assert(errnum == 0); errnum;})
#endif // MCHECK

/// 用于分离文件名与路径
class SourceFile
{
public:
	SourceFile():filename_(NULL){}
	SourceFile(const std::string& tstr){ SplitString(tstr.data()); }
	SourceFile(const char* tchr){ SplitString(tchr); }
	const char* filename(){ return filename_; }
private:
	void SplitString(const char* tchr);
	const char* filename_;
};
/// logger为全部项目服务，所以将其放置在thsrv空间中
class Logger : public noncopyable
{
public:
	typedef void(*OutputFunc)(const char*, const size_t);
	typedef void(*FlushFunc)();
	typedef void(*ExitFunc)();
	
	/// logger所需要的组件
	enum LogLevel{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUMLEVEL
	};
	const char* LogName[LogLevel::NUMLEVEL] = {
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL"
	};
	
public:
	/// For Debug
	// Logger();  //添加堆栈信息
	/// for Info
	Logger(LogLevel tloglevel,TimeStamp tm);
	/// For Warn
	Logger(SourceFile tfile,const char* tfunc,int tline,LogLevel tloglevel,TimeStamp tm);
	///for error,fatal---其中func为错误处理函数---abort()
	Logger(SourceFile tfile,const char* tfunc,int tline,LogLevel tloglevel,TimeStamp tm,void(*func)());
	
	~Logger();
	
	LogStream& stream(){ return stream_; }	
	
	void setOutput(OutputFunc tfunc);
	void setFlush(FlushFunc tfunc);
	
private:
	void LogFinish()
	{
		stream_<<".--FINISH.\n";
	}
private:
	LogStream stream_;
	ExitFunc exitFunc;
	
}; //END LOGGER CLASS
	
/// 全局函数与变量

// 默认输出与刷新函数
void defaultOutput(const char* msg,const int tlen);
void defaultFlush();

// 全局输出刷新函数
extern Logger::OutputFunc g_outputFunc;
extern Logger::FlushFunc  g_flushFunc;

// 用于快速使用
#define LOG_DEBUG   Logger(__FILE__, __func__, __LINE__,Logger::DEBUG, TimeStamp::now()).stream()
#define LOG_INFO	Logger(Logger::INFO, TimeStamp::now()).stream()
#define LOG_WARN	Logger(__FILE__, __func__, __LINE__,Logger::WARN, TimeStamp::now()).stream()
#define LOG_ERR		Logger(__FILE__, __func__, __LINE__,Logger::ERROR, TimeStamp::now(),NULL).stream()
#define LOG_FATAL	Logger(__FILE__, __func__, __LINE__,Logger::FATAL, TimeStamp::now(),abort).stream()

}  //END THSRV NAMESPACE

#endif  //END THSRV_BASE_LOGGER_H
