/****************************************************************************
* @File:	Http响应报文数据
* @Date:	2020-5-9
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_HTTPRESPONSE_H
#define THSRV_NET_HTTPRESPONSE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/copyable.h"

#include <string>
#include <unordered_map>

namespace thsrv
{

namespace net
{

/// START CLASS
class Buffer;

class HttpResponse : public copyable
{
public:    
    static const std::string kCRLF;

    static const std::string kUnknown, kHttp10, kHttp11; 

    // enum Version{kUnknown, kHttp10, kHttp11};    
    enum ResponseState{
        kInvalid, 
        k200OK = 200, 
        k301MovePermanently = 301, 
        k400BadRequest = 400,
        k404NotFound = 404
    };

public:
    // HttpResponse(bool close, Version ver=Version::kUnknown);
    HttpResponse(bool close, std::string ver = kUnknown);

    void addHeader(const std::string& key, const std::string& value);
    const std::string getHeader(const std::string& field);
    
    void setStatusCode(ResponseState code){ state_ = code; }
    void setBody(const std::string& body){ body_ = body; }
    // void setVersion(Version ver){ version_ = ver; }
    void setCloseConn(bool on){ closeConn_ = on; }
    bool getCloseConn() const{ return closeConn_; }
    void setContentType(const std::string& type){ addHeader("Content-Type", type); }

    void appendInBuffer(Buffer& buf);

private:
    bool isValidVersion() const{ return version_ != kUnknown; }
    std::string parseStatusCode();

private:
    bool closeConn_;

    ResponseState state_;
    //////////////////////// FIXME: add expire time
    std::string version_;
    std::string stateMsg_;
    std::string body_;

    std::unordered_map<std::string, std::string>headers_;
};

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE

#endif  // THSRV_NET_HTTPRESPONSE_H
