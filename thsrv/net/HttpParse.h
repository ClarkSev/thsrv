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

#include "thsrv/net/Buffer.h"

#include <string>
#include <unordered_map>

namespace thsrv
{
	
namespace net
{


class HttpParse
{

// using namespace std;
public:
    HttpParse(Buffer* buf);

    std::string getHeader(std::string& field){
        if(headers_.find(field)==headers_.end()) return "NULL";
        else{
            return headers_[field];
        }
    }
    const Method getMethod() const{ return method_; }
    const Version getVersion() const{ return version_; }
    const bool getError() const{ return err_; }
    const std::string getBody() const{ return body_; }

private:  // method
    void setBody(const std::string& body){ body_ = body; }
    bool addHeader(const char* start, const char* colon, const char* end);
    bool setMethod(const char* start, const char* end);
    bool setPath(const char* start, const char* end);
    bool setQuery(const char* start, const char* end);
    bool setVersion(const char* start, const char* end);
    bool parseRequestLine(const char* start, const char* end);

    bool parseRequest(Buffer* buf);

public:  // proporities
    enum Method{kInvalid, kGet, kPost, kHead};
    enum Version{kUnknown, kHttp10, kHttp11};    

private:  // proporities
    bool err_;  // 判断是否解析成功
    Method method_;
    Version version_;

    std::string path_;
    std::string query_;
    std::string body_;

	std::unordered_map<std::string, std::string>headers_;

};  //END HttpParse CLASS

HttpParse::HttpParse(Buffer* buf):method_(kInvalid), \
    version_(kUnknown), \
    state_(kExpectRequestLine)
{
    err_ = parseRequest(buf);
}
bool HttpParse::addHeader(const char* start,const char* colon, const char* end)
{
    if(colon==start || colon==end || start==end) return false;
    while(*colon==' ' && colon!=end) colon++;
    std::string field(start, colon), value(colon, end);
    headers_[field] = value;
    return true;
}
bool HttpParse::setMethod(const char* start, const char* end)
{
    bool success = true;
    std::string method(start, end);
    assert(method_ == kInvalid);
    switch(method){
        case "GET": method_ = kGet;break;
        case "POST": method_ = kPost;break;
        case "HEAD": method_ = kHead;break;
        default:success = false;
    }
    return success;
}
bool HttpParse::setPath(const char* start, const char* end)
{
    bool success = true;
    const char* question = std::find(start, end, '?');
    if(question!=end){
        path_.assign(start, question);
        query_.assign(question+1, end);
    }else{
        path.assign(start, end);
    }
    return success;
}
bool HttpParse::setQuery(const char* start, const char* end)
{
    if(start==end) return false;
    query_.assign(start, end);
    return true;
}
bool HttpParse::setVersion(const char* start, const char* end)
{
    bool success = true;
    std::string version(start, end);
    switch(version){
        case "HTTP/1.0": version_ = kHttp10; break;
        case "HTTP/1.1": version_ = kHttp11; break;
        default: version_ = kUnknown; success = false;
    }
    return success;
}

bool HttpParse::parseRequestLine(const char* start, const char* end)
{
    bool success = true;
    const char* cur = start;
    const char* space = std::find(cur, end, ' ');
    if(space!=end && setMethod(cur, space)){
        cur = space + 1;
        while(*cur==' ' && cur!=end) cur++;  // filter the surplus space
        space = std::find(cur, end, ' ');
        if(space!=end && setPath(cur, space)){
            cur = space + 1;
            while(*cur==' ' && cur!=end) cur++;
            if(cur!=end && setVersion(cur, end)){
                success = true;
            }else{
                success = false;
            }

        }else{
            success = false;
        }
    }else{
        success = false;
    }

    return success;
}
bool HttpParse::parseRequest(Buffer* buf)
{
    std::string line = buf.getlineReadBuf();
    bool success = parseRequestLine(line.begin(), line.end());
    const char* colon = nullptr;
    // save the request headers
    while(true){
        
        line = buf.getlineReadBuf();
        if(line.emtpy() || 0==buf.readable()){
            break;
        }
        // 存在header
        colon = std::find(line.begin(), line.end(), ':');
        success &= addHeader(line.begin(), colon, line.end());
    }
    std::string body = buf.getReadBufferToString();
    setBody(body);
    return success;
}


}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_EVENTLOOP_H

