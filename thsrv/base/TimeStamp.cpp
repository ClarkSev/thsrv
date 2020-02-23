/****************************************************************************
* @Date:	2020-2-5
* @Author:	T.H.
* @Note:	时间戳实现文件
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include "thsrv/base/TimeStamp.h"

// #include <unistd.h>
#include <stdio.h>    // for snprintf
#include <sys/time.h> // for gettimeofday/time_t/...
#include <string.h>  // for memcpy/strlen/...
#include <inttypes.h> // for PRId64(To make sure can rightly use in 64-bit or 32-bit system)

namespace thsrv
{

/// get current time --- all of the object have just one instance
TimeStamp TimeStamp::now()
{
	struct timeval tmVal;
	gettimeofday(&tmVal, NULL);
	int64_t sec = tmVal.tv_sec;
	return TimeStamp(sec * kNumMicroSecondPerSecond + tmVal.tv_usec);
}

///
/// 将时间类型转换为字符串便于显示
///
std::string TimeStamp::toString()const
{
	char buf[32] = {0};
	int64_t sec = kMicroSecondSinceEpoch_ / kNumMicroSecondPerSecond;
	int64_t microSec = kMicroSecondSinceEpoch_ % kNumMicroSecondPerSecond;
	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", sec, microSec);  // 预留一个'\0'
	return buf;
}
/// 用于时间格式化输出
std::string TimeStamp::toStringFmt(bool enShowUs)const
{
	struct tm tm_st;
	char buf[64] = {0};
	time_t sec = static_cast<time_t>(kMicroSecondSinceEpoch_ / kNumMicroSecondPerSecond);
	gmtime_r(&sec, &tm_st);
	if(enShowUs){   /// 显示毫秒精度
		int microSec = static_cast<int>(kMicroSecondSinceEpoch_ % kNumMicroSecondPerSecond);
		snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d.%06d",
				tm_st.tm_year+1900, tm_st.tm_mon+1, tm_st.tm_mday, 
				tm_st.tm_hour, tm_st.tm_min, tm_st.tm_sec,
				microSec);
	}else{
		snprintf(buf, sizeof(buf)-1, "%04d-%02d-%02d %02d:%02d:%02d",
				tm_st.tm_year+1900, tm_st.tm_mon+1, tm_st.tm_mday, 
				tm_st.tm_hour, tm_st.tm_min, tm_st.tm_sec);
	}
	return buf;
}
	
}  //END THSRV NAMESPACE

