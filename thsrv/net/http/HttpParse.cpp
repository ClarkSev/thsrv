/****************************************************************************
* @File:	Http请求解析器
* @Date:	2020-5-8
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/http/HttpParse.h"
#include "thsrv/net/Buffer.h"

#include <string>
#include <assert.h>
#include <iostream>

#include <algorithm>

namespace thsrv
{

namespace net
{

/// START CLASS

const std::string HttpParse::kInvalid="Invalid";
const std::string HttpParse::kGet="GET";
const std::string HttpParse::kPost="POST";
const std::string HttpParse::kHead="HEAD";

const std::string HttpParse::kUnknown="Unknown";
const std::string HttpParse::kHttp10="HTTP/1.0";
const std::string HttpParse::kHttp11="HTTP/1.1"; 

HttpParse::HttpParse(Buffer& buf, TimeStamp receivedTime):method_(kInvalid), \
    version_(kUnknown),\
    receivedTime_(receivedTime)
{
    err_ = parseRequest(buf) ? false : true;
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
    method_.assign(start, end);
    if(method_!=kGet && method_!=kPost && method_!=kHead){
        method_ = kInvalid;
        success = false;
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
        path_.assign(start, end);
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
    version_.assign(start, end);
    if(version_ != kHttp11 && version_!= kHttp10){
        version_ = kUnknown;
        return false;
    }
    return true;
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
            // while(*cur==' ' && cur!=end) cur++;
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
bool HttpParse::parseRequest(Buffer& buf)
{
    std::string line = buf.getlineReadBuf();
    bool success = parseRequestLine(&*line.begin(), &*line.end());
    const char* colon = nullptr;
    // save the request headers
    while(true){
        line = buf.getlineReadBuf();
        if(line.empty() || 0==buf.readable()){
            break;
        }
        // 存在header
        colon = std::find(&*line.begin(), &*line.end(), ':');
        success &= addHeader(&*line.begin(), colon, &*line.end());
    }
    std::string body = buf.getReadBufferToString();
    setBody(body);
    return success;
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE

