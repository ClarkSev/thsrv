/****************************************************************************
* @Date:	2020-2-5
* @Author:	T.H.
* @Note:	时间戳头文件，将char* 与 string统一使用StringPiece接收
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_TIMESTAMP_H
#define THSRV_BASE_TIMESTAMP_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include "thsrv/base/copyable.h"
#include "thsrv/base/StringPiece.h"

#include <boost/operators.hpp>  // for less_than_comparable<> / equility_comparable<>

#include <string>  //for std::string

namespace thsrv
{
	
// namespace base   // TimeStamp的生命期设置为与thsrv一致
// {
	
class TimeStamp:public copyable,
				public boost::less_than_comparable<TimeStamp>,
				public boost::equality_comparable<TimeStamp>
{
public:
	explicit TimeStamp(const int64_t tMsSinceEpoch):kMicroSecondSinceEpoch_(tMsSinceEpoch){};
	TimeStamp() :kMicroSecondSinceEpoch_(0) {};
	
	// TimeStamp(const TimeStamp&)=default;  // 使用默认拷贝构造函数即可
	static TimeStamp now();    // 因为类中只含常规数据类型（整型），
							   // 所以引用和原对象的区别不明显
	static TimeStamp invaild() { return TimeStamp(0); }
	bool vaild()const{ return kMicroSecondSinceEpoch_ != 0; }
	//FIXME: Using string or stringpiece(ctor. by self)
	std::string toString()const;    
	std::string toStringFmt(bool enShowUs=true)const;    // enShowUs--显示精度是否为s以上
	
	/// 公有接口
	int64_t microSecondSinceEpoch() const{ return kMicroSecondSinceEpoch_; }
	int64_t secondSinceEpoch() const
	{ 
		return static_cast<int64_t>(kMicroSecondSinceEpoch_/kNumMicroSecondPerSecond); 
	}
	
	const static int kNumMicroSecondPerSecond = 1000 * 1000;
private:
	int64_t kMicroSecondSinceEpoch_;
	
};  //END TIMESTAMP CLASS

///
/// 重载比较运算符，使用全局函数实现
///
inline bool operator==(const TimeStamp lhs, const TimeStamp rhs)
{
	if(lhs.microSecondSinceEpoch()==rhs.microSecondSinceEpoch()){
		return true;
	}
	return false;
}

inline bool operator<(const TimeStamp lhs, const TimeStamp rhs)
{
	if(lhs.microSecondSinceEpoch()<rhs.microSecondSinceEpoch()){
		return true;
	}
	return false;
}

/// 时间添加
inline TimeStamp addTime(TimeStamp timeStamp, double second)
{
	int64_t delta = static_cast<int64_t>(second * TimeStamp::kNumMicroSecondPerSecond);
	return TimeStamp(timeStamp.microSecondSinceEpoch() + delta);
}

/// 时间差异函数
inline TimeStamp timeDifference(const TimeStamp high, const TimeStamp low)
{
	int64_t timeDist = high.microSecondSinceEpoch() - low.microSecondSinceEpoch();
	return TimeStamp(timeDist);	
}

/*
    struct timespec {
        time_t tv_sec;                // Seconds 
        long   tv_nsec;               // Nanoseconds 十亿分之一秒
    }
*/
inline struct timespec timeToSpec(const TimeStamp& ths)
{
	struct timespec ret;
	ret.tv_sec = static_cast<time_t>(ths.microSecondSinceEpoch() / TimeStamp::kNumMicroSecondPerSecond);
	ret.tv_nsec = static_cast<long>(ths.microSecondSinceEpoch() % TimeStamp::kNumMicroSecondPerSecond)*1000;
	return ret;
}

// }  //END BASE NAMESPACE
	
}  //END THSRV NAMESPACE


#endif  // END THSRV_BASE_TIMESTAMP_H





