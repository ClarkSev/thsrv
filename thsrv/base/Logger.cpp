/****************************************************************************
* @File:	日志登记器
* @Date:	2020-2-7
* @Author:	T.H.
* @Note:	功能说明：
	1.宏LOG_INFO等可直接使用，且可设置日志的输出终端（文件还是控制台）
	2.含不同的日志粒度，debug的粒度最小，可以详细观察哪一行代码出错
	
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Logger.h"

#include <stdio.h>   //for fwrite
#include <string.h>  //for strrchr
#include <assert.h>

namespace thsrv
{
	
void defaultOutput(const char* msg, const size_t tlen)
{
	size_t wtlen = fwrite(msg, sizeof(char), tlen, stdout);
	assert(wtlen==static_cast<size_t>(tlen));  //FIXME
}
void defaultFlush()
{
	fflush(stdout);
}

// 全局输出刷新函数
Logger::OutputFunc g_outputFunc = defaultOutput;
Logger::FlushFunc  g_flushFunc  = defaultFlush;

// 分离文件名与路径
void SourceFile::SplitString(const char* tfile)
{
	filename_ = strrchr(tfile, '/');
	if(filename_ == NULL){  // not found
		filename_ = tfile;
		return;
	}
	filename_ += 1;
}

//END SOURCEFILE CLASS

/// logger为全部项目服务，所以将其放置在thsrv空间中	
/// For Warn
Logger::Logger(SourceFile tfile,const char* tfunc,int tline,LogLevel tloglevel,TimeStamp tm):exitFunc(NULL)
{
	stream_<<"FILE:"<<tfile.filename()<<"\t"\
		   <<"FUNC:"<<tfunc<<"  "\
		   <<"LINE:"<<tline<<"\t"\
		   <<LogName[tloglevel]<<"  "\
		   <<"TIME:"<<tm.toStringFmt(false)<<"  ";
}
/// for Info
Logger::Logger(LogLevel tloglevel,TimeStamp tm):exitFunc(NULL)
{
	stream_<<LogName[tloglevel]<<"  "\
		   <<"TIME:"<<tm.toStringFmt(false)<<"  ";
}
///for error,fatal---其中func为错误处理函数---abort()
Logger::Logger(SourceFile tfile,const char* tfunc,int tline,LogLevel tloglevel,TimeStamp tm,void(*func)()):exitFunc(func)
{
	stream_<<"FILE:"<<tfile.filename()<<"\t"\
		   <<"FUNC:"<<tfunc<<"  "\
		   <<"LINE:"<<tline<<"\t"\
		   <<LogName[tloglevel]<<"  "\
		   <<"TIME:"<<tm.toStringFmt(false)<<"  ";
	// //注册出错处理函数
	// exitFunc = func;
}

void Logger::setOutput(OutputFunc tfunc)
{
	g_outputFunc = tfunc;
}
void Logger::setFlush(FlushFunc tfunc)
{
	g_flushFunc = tfunc;
}

//在析构时刷新缓存
Logger::~Logger()
{
	if(!exitFunc)  stream_<<"\n";   // add '\n'
	const LogStream::Buffer& lbuf(stream_.buffer());
	g_outputFunc(lbuf.data(), lbuf.size());
	g_flushFunc();
	if(exitFunc){
		LogFinish(); // 结束标识
		exitFunc();  // 异常退出
	}
}

//END LOGGER CLASS
	
}  //END THSRV NAMESPACE

