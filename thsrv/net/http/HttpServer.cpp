/****************************************************************************
* @File:	Http服务器
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
#include "thsrv/net/http/HttpServer.h"
#include "thsrv/net/http/HttpParse.h"
#include "thsrv/net/http/HttpResponse.h"
#include "thsrv/net/TcpConnection.h"

#include "thsrv/base/Logger.h"

namespace thsrv
{

namespace net
{

namespace detail
{
    void defaultHttpCallback(const HttpParse& request, HttpResponse& response)
    {

    }
}  // END NAMESPACE DETAIL

using namespace std::placeholders;

HttpServer::HttpServer(EventLoop* loop, const std::string& name, const InetAddress& addr):
server_(loop, name, addr),
httpcb_(detail::defaultHttpCallback)
{
    // server_.setConnectionCallback(std::bind(HttpServer::onConnection, this, _1));
    server_.setOnMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

// void HttpServer::onConnection(const TcpConnectionPtr& conn)
// {
//     if(conn->connected()){
//         LOG_INFO<<"HttpSercer: "<<conn->getPeerAddr().toIpAndPort()<<"\n";
//     }
// }

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer& buf, TimeStamp receiveTime)
{
    HttpParse request(buf, receiveTime);
    if(request.getError()){
        // printf("The request Error!\n");
        conn->send("HTTP/1.1 400 Bad Request\r\n");
        conn->closeConnection();
    }else{
        onRequest(conn, request);
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn,HttpParse& req)
{
    std::string field = "Conenction";
    const std::string& connection = req.getHeader(field);
    bool closed = connection=="close";
    HttpResponse response(closed, req.getVersion());
    httpcb_(req, response);
    Buffer buf;
    response.appendInBuffer(buf);
    conn->send(buf);
    if(response.getCloseConn()){
        conn->closeConnection();
    }else{
        ////////////// fixme: add expire time
        // conn->closeConnection();
    }
}

}  // END NAMESPACE NET

}  // END NAMESPACE THSRV


