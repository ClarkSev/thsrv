/*
    test eventloopthread
*/

#include "thsrv/base/Logger.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/net/EventLoopThread.h"
#include "thsrv/base/Thread.h"

#include <functional>

using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;
using namespace std;

void func1()
{
    LOG_INFO<<"calling func1";
    while(1){
        LOG_INFO<<"loop func1";
        sleep(1);
    }
    LOG_INFO<<"quit func1";
}
void func2()
{
    LOG_INFO<<"calling func2";
    while(1){
        LOG_INFO<<"loop func2";
        sleep(2);
    }
    LOG_INFO<<"quit func2";
}
int main()
{
    int cnt = 0;
    EventLoopThread th1,th2;
    EventLoop* loop1 = th1.startLoop();
    EventLoop* loop2 = th2.startLoop();
    loop1->runInLoop(func1);
    loop2->runInLoop(func2);
    LOG_INFO<<"main func";
    while(1){
        LOG_INFO<<"loop main";
        sleep(1);
        cnt++;
        if(cnt==8)  break;
    }
    // loop1->stop();
    return 0;
}
