#include "thsrv/net/TimerQueue.h"
#include "thsrv/net/TimerId.h"
#include "thsrv/net/EventLoop.h"
#include "thsrv/base/Logger.h"
#include "thsrv/base/TimeStamp.h"
#include "thsrv/base/Thread.h"

using namespace std;
using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;

EventLoop* g_loop = NULL;
int g_cnt = 0;
void func1()
{
    LOG_INFO<<"calling func1";
}
void func2()
{
    LOG_INFO<<"calling func2";
    LOG_INFO<<"g_cnt = "<<g_cnt++;
    if(g_cnt == 10){
        g_loop->stop();
    }
}

int main()
{
    EventLoop main_loop;
    g_loop = &main_loop;
    g_loop->runAfter(1.2, func1);
    g_loop->runEvery(2, func2);
    g_loop->start();
    return 0;
}
