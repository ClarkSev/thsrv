

#include "thsrv/net/TcpClient.h"
#include "thsrv/base/Atomic.h"
#include "thsrv/base/Mutex.h"
#include "thsrv/base/Condition.h"
#include "thsrv/net/Buffer.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/base/Thread.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/TcpConnection.h"

#include <iostream>
#include <string>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;
using namespace std::placeholders;

EventLoop* g_loop;
// MutexLock  g_mx;
// Condition  g_cond(g_mx);
Atomic<int> g_flg_started_client;

class EchoClient
{
public:
    EchoClient(EventLoop* loop, const InetAddress& serverAddr):\
    loop_(loop),echoclient_(loop, serverAddr)//,start_(false)
    {
        echoclient_.setConnectionCallback(std::bind(&EchoClient::onConnection, this, _1));
        echoclient_.setOnMessageCallback(std::bind(&EchoClient::onMessage, this, _1, _2,_3));
        echoclient_.setWriteCompleteCallback(std::bind(&EchoClient::writeComplete, this, _1));
    }
    void connect()
    {
        echoclient_.connect();
    }
    void reconnect()
    {
        echoclient_.reconnect();
    }
    void send(string& tstr)
    {
        echoclient_.send(tstr);
    }
    // void started()const{ return start_ == true; }
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer& buf,TimeStamp receiveTime)
    {
        LOG_INFO<<"onMessage.";
        LOG_INFO<<"recv data = "<<buf.getReadBufferToString();
    }
    void writeComplete(const TcpConnectionPtr& conn)
    {
        LOG_INFO<<" writeComplete.";
    }
private:
    EventLoop* loop_;
    TcpClient echoclient_;
    // bool start_;
};
void procData(EchoClient* client)
{
    string str;
    LOG_INFO<<"Please input something to Transform.";
    cin>>str;
    client->send(str);
    if(str == "reconnect"){
        client->reconnect();
    }
}
void EchoClient::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO<<conn->getPeerAddr().toIpAndPort()<<" -> "\
            <<conn->getLocalAddr().toIpAndPort()<<" is "\
            <<(conn->connected()? "UP" : "DOWN");
    if(conn->connected()){
        g_flg_started_client.fetchAndAdd(1);
        // start_ = true;
        LOG_INFO<<"connection.";
        // loop_->runInLoop(std::bind(procData, this));
        echoclient_.send("12345");
    }else{
        // start_ = false;
        g_flg_started_client.store(0);
        LOG_INFO<<"disconnection.";
        loop_->stop();
    }
    
}
EchoClient *g_client;

void testfunc1()
{
    while(!g_flg_started_client.load()) {}
    while(g_flg_started_client.load()){
        g_loop->runInLoop(std::bind(procData, g_client));           
        sleep(4);
    }
    LOG_INFO<<"Thread testfunc1 quit.";
}
void testfunc2()
{       
    LOG_INFO<<"Start testfunc2";
    EventLoop thread_loop;
    g_loop = &thread_loop;
    InetAddress addr("192.168.163.128" ,1234);
    EchoClient client(g_loop, addr);
    g_client = &client;
    g_client->connect();
    g_loop->start();
}
int main()
{
    Thread t1,t2;
    t1.setThreadFunc(testfunc1);
    t2.setThreadFunc(testfunc2);
    t1.start(); t2.start();
    sleep(5);
    LOG_INFO<<"main func";
    // g_client->reconnect();
    return 0;
}

