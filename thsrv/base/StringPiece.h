/****************************************************************************
* @File：	字符串类，为兼容char* 与string
* @Date:	2020-2-6
* @Author:	T.H.
* @Note:	该类unsafe，直接将指针传出
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_BASE_STRINGPIECE_H
#define THSRV_BASE_STRINGPIECE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header

#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include <boost/operators.hpp>  // for less_than_comparable<>/...

#include <ostream>  // for ostream
#include <memory>   // for share_ptr
#include <string>   // C++ library
#include <type_traits>  // for __type_traits

namespace thsrv
{
	
class StringPiece : public boost::less_than_comparable<StringPiece>, //:public boost::copyable
				    public boost::equality_comparable<StringPiece>
{
public:
	//ctor.
	StringPiece():ptr_(NULL), szStr_(0){}
	StringPiece(const char* tchr):ptr_(tchr), szStr_(static_cast<int>(strlen(tchr))){}
	StringPiece(const char tchr):ptr_(&tchr), szStr_(sizeof(tchr)){}
	//StringPiece(const unsigned char* tchr):ptr_(static_cast<const char*>(tchr)), szStr_(static_cast<int>(strlen(tchr))){}
	StringPiece(const char* tchr, int tsz):ptr_(tchr), szStr_(tsz){}
	//StringPiece(const unsigned char* tchr, int tsz):ptr_(static_cast<const char*>(tchr)), szStr_(tsz){}
	StringPiece(const std::string& tstr):ptr_(tstr.data()), szStr_(tstr.size()){}
	//copy ctor.
	StringPiece(const StringPiece& tstr):ptr_(tstr.ptr_),szStr_(tstr.szStr_){}
	//move ctor.
	StringPiece(const StringPiece&& tstr) :ptr_(tstr.ptr_), szStr_(tstr.szStr_) {}

	// Normal method or interface
	// StringPiece& operator=(const StringPiece& tstr){}  // Using default method
	char operator[](int idx)const { return *(ptr_ + idx); }   // may be throw exception and can't be changed
	const char* data()const{ return ptr_; }
	size_t size()const{ return szStr_; }
	const char* begin()const{ return ptr_; }
	const char* end()const{ return ptr_ + szStr_; }
	void set(const char* tchr, int tsz)
	{
		ptr_ = tchr;
		szStr_ = tsz;
	}
	void clear(){ ptr_ = NULL;  szStr_ = 0; }
	std::string toString()
	{
		return ptr_;
	}
	void removePrefix(int npos)
	{
		ptr_ += npos;
		szStr_ += npos;
	}
	void removeSuffix(int npos)
	{
		szStr_ -= npos;
	}
   // just for inner calling
	~StringPiece()
	{
		if(ptr_){
			ptr_ = NULL;
		}
		szStr_ = 0;
	}
private:
	const char* ptr_;
	size_t szStr_;
	
};  //END STRINGPIECE CLASS
	
// override the operator
inline bool operator==(const StringPiece& lhs, const StringPiece& rhs)
{
	return ( lhs.size() == rhs.size() && \
			 memcmp(lhs.data(), rhs.data(), lhs.size())==0);
}
inline bool operator<(const StringPiece& lhs, const StringPiece& rhs)
{
	if(lhs.size()<rhs.size()){
		return true;
	}else if(lhs.size()==rhs.size() && \
			 memcmp(lhs.data(), rhs.data(), lhs.size())==-1){
		return true;
	}
	return false;
}

inline std::ostream& operator<<(std::ostream& os, const StringPiece& tstr)
{
	if (tstr.size() == 0)
		return os;
	// FIXME: didn't check the size.
	os << tstr.data();
	return os;
}

#ifdef HAVE_TYPE_TRAITS
// Setup STL adaption eg: vector<StringPiece>
template<> struct __type_traits<thsrv::StringPiece> {
  typedef __true_type    has_trivial_default_constructor;
  typedef __true_type    has_trivial_copy_constructor;
  typedef __true_type    has_trivial_assignment_operator;
  typedef __true_type    has_trivial_destructor;
  typedef __true_type    is_POD_type;
};

#endif  //END HAVE_TYPE_TRAITS

}  //END THSRV NAMESPACE


#endif  //END THSRV_BASE_STRINGPIECE_H

