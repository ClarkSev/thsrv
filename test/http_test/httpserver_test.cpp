/*
    @file: test the httpserver
*/
#include "thsrv/net/http/HttpServer.h"
#include "thsrv/net/http/HttpResponse.h"
#include "thsrv/net/http/HttpParse.h"

#include "thsrv/net/EventLoop.h"
#include "thsrv/net/InetAddress.h"
#include "thsrv/base/Logger.h"
#include "thsrv/base/TimeStamp.h"

#include <string>
#include <iostream>

using namespace std;
using namespace thsrv;
using namespace thsrv::net;
using namespace thsrv::base;

bool flg = false;

void httpCallback(const HttpParse& req, HttpResponse& response)
{
    cout<<"Request : "<<req.getMethod()<<" "<<req.path()<<endl;
    if(!flg){
        cout<<"request headers:"<<endl;
        for(auto& header : req.headers()){
            cout<<header.first<<": "<<header.second<<endl;
        }
        cout<<endl;
        flg = true;
    }
    if(req.path()=="/"){
        response.setStatusCode(HttpResponse::ResponseState::k200OK);
        response.setContentType("text/html");
        response.addHeader("Server", "thsrv");
        std::string now = TimeStamp::now().toStringFmt(false);
        std::string body = "<html><head><title>This is title!</title></head>"
                            "<body><h1>Hello,Now is"+ now +
                            "</h1></body></html>";
        response.setBody(body);
    }else if(req.path()=="/hello"){
        response.setStatusCode(HttpResponse::ResponseState::k200OK);
        response.setContentType("text/plain");
        response.addHeader("Server", "thsrv");
        response.setBody("<html><body><h1>Hello world!</h1></body></html>");
    }else{
        response.setStatusCode(HttpResponse::ResponseState::k404NotFound);
        response.setCloseConn(true);
    }

}

int main()
{
    EventLoop loop;
    InetAddress addr(8080);
    std::string name = "httpserver_test";
    HttpServer server(&loop, name, addr);

    server.setHttpCallback(httpCallback);
    server.start();
    
    loop.start();
    return 0;
}
