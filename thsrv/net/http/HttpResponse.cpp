/****************************************************************************
* @File:	Http响应报文
* @Date:	2020-5-9
* @Author:	T.H.
* @Note:	功能说明：
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/http/HttpResponse.h"
#include "thsrv/net/Buffer.h"

#include <string>
#include <unordered_map>

namespace thsrv
{

namespace net
{

/// START CLASS

const std::string HttpResponse::kCRLF = "\r\n";

const std::string HttpResponse::kUnknown="Unknown";
const std::string HttpResponse::kHttp10="HTTP/1.0";
const std::string HttpResponse::kHttp11="HTTP/1.1"; 

HttpResponse::HttpResponse(bool close, std::string ver):\
closeConn_(close),\
state_(ResponseState::kInvalid),\
version_(ver)
{
    setCloseConn(closeConn_);
}

void HttpResponse::addHeader(const std::string& key, const std::string& val)
{
    headers_[key] = val;
}
const std::string HttpResponse::getHeader(const std::string& field)
{
    if(headers_.find(field)==headers_.end()) return "";
    else return headers_[field];
}
void HttpResponse::setCloseConn(bool on)
{
    std::string close = on? "close" : "keep-alive";
    addHeader("Connection", close);
    closeConn_ = on;
}
void HttpResponse::setBody(const std::string& body)
{
    body_ = body;
    addHeader("Content-Length", std::to_string(body_.size()));
}
void HttpResponse::appendInBuffer(Buffer& buf)
{
    if(!isValidVersion()){
        setStatusCode(ResponseState::k400BadRequest);
    }
    std::string code = parseStatusCode();
    std::string line = version_ + " " + code + " " + stateMsg_ + kCRLF;
    buf.appendInWtBuf(line);
    // append headers
    for(auto& header : headers_){
        line = header.first + ":" + header.second + kCRLF;
        buf.appendInWtBuf(line);
        line.clear();
    }

    buf.appendInWtBuf(kCRLF);
    // append body
    buf.appendInWtBuf(body_ + kCRLF);
}

std::string HttpResponse::parseStatusCode()
{
    std::string ans = "Invalid";
    switch(state_){
        case ResponseState::k200OK: 
            ans = "200", stateMsg_ = "OK";break;
        case ResponseState::k301MovePermanently:
            ans = "301", stateMsg_ = "Move Permanently";break;
        case ResponseState::k400BadRequest:
            ans = "400", stateMsg_ = "Bad Request";break;
        case ResponseState::k404NotFound:
            ans = "404", stateMsg_ = "Not Found";break;
        default:
            stateMsg_ = "Invalid";
    }
    return ans;
}

/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE

