/****************************************************************************
* @Date:	2020-2-6
* @Author:	T.H.
* @Note:	日志数据流，其实质就是一个buffer
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include "thsrv/base/LogStream.h"

#include <algorithm>
// #include <iostream>

namespace thsrv
{

/// 局部类与函数
namespace detail
{

/// 该转换来源于muduo中的转换
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
// static_assert(sizeof(digits) == 20, "wrong number of digits");

// const char digitsHex[] = "0123456789ABCDEF";
// static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");

// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
  T i = value;
  char* p = buf;

  do
  {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0)
  {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

}  //END DETAIL NAMESPACE

//------ LogStream Class -------//

/// public method and interface
// 字符串
LogStream& LogStream::operator<<(const StringPiece& tstr)
{
	buf_.append(tstr.data(), tstr.size());
	return *this;
}
// 兼容const char[]类的常量
LogStream& LogStream::operator<<(const std::string& tstr)
{
	buf_.append(tstr.data(), tstr.size());
	return *this;
}
LogStream& LogStream::operator<<(const char* tchr)
{
	buf_.append(tchr, strlen(tchr));
	return *this;
}
// 浮点数
LogStream& LogStream::operator<<(double tnum)
{
	char lbuf[64] = {0};
	int len = snprintf(lbuf, 64, "%.6lf", tnum);
	buf_.append(lbuf, len);
	return *this;
}
LogStream& LogStream::operator<<(float tnum)
{
	char lbuf[64] = {0};
	int len = snprintf(lbuf, 64, "%.4f", tnum);
	buf_.append(lbuf, len);
	return *this;
}
// 整数
LogStream& LogStream::operator<<(short tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(unsigned short tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(int tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(unsigned int tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(long tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(unsigned long tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(long long tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}
LogStream& LogStream::operator<<(unsigned long long tnum)
{
	buf_.append(num2Str(tnum));
	return *this;
}


/// 全局函数

/// 将数值转换为科学计数法
std::string formatSI(int64_t tnum)
{
	return "";
}

/// 将内存数值转换为适合查看的数值
std::string formatIEC(int64_t tnum)
{
	return "";
}

/// 将数据转换为字符串
template<typename T,int SZBUF>
std::string num2Str(const T tnum)
{
	char lbuf[SZBUF] = {0};
	int len = static_cast<int>(detail::convert(lbuf, tnum));
	assert(len<=SZBUF);
	return lbuf;
}

}  //END THSRV NAMESPACE

