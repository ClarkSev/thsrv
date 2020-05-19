/****************************************************************************
* @File:	Http请求头解析器
* @Date:	2020-5-08
* @Author:	T.H.
* @Note:	功能说明：  该类针对请求报文设计
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_HTTPPARSE_H
#define THSRV_NET_HTTPPARSE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/copyable.h"
#include "thsrv/base/TimeStamp.h"

#include <string>
#include <map>

namespace thsrv
{
	
namespace net
{

class Buffer;

class HttpParse : public copyable
{
public:  // proporities
    static const std::string kInvalid, kGet, kPost, kHead;
    static const std::string kUnknown, kHttp10, kHttp11; 
    enum State{ kExpectRequestLine, kExpectHeaders, kExpectBody, kGotAll };
public:
    HttpParse();

    std::string getHeader(const std::string& field){
        if(headers_.find(field)==headers_.end()) return "NULL";
        else{
            return headers_[field];
        }
    }
    const std::map<std::string, std::string>headers()const{ return headers_; }
    const std::string getMethod() const{ return method_; }
    const std::string path() const{ return path_; }
    const std::string getVersion() const{ return version_; }
    const std::string getBody() const{ return body_; }
    bool gotAll()const{ return state_ == kGotAll; }
    bool parseRequest(Buffer& buf, const TimeStamp receivedTime);

    void reset();
    void swap(HttpParse& that);

private:  // method
    void setBody(const std::string& body){ body_ = body; }
    bool addHeader(const char* start, const char* colon, const char* end);
    bool setMethod(const char* start, const char* end);
    bool setPath(const char* start, const char* end);
    bool setQuery(const char* start, const char* end);
    bool setVersion(const char* start, const char* end);
    void setReceiveTime(const TimeStamp t_time){ receivedTime_ = t_time; }
    bool parseRequestLine(const char* start, const char* end);

private:  // proporities
    State state_;

    std::string method_;
    std::string version_;
    TimeStamp receivedTime_;

    std::string path_;
    std::string query_;
    std::string body_;

	std::map<std::string, std::string>headers_;

};  //END HttpParse CLASS


}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_HTTPPARSE_H

