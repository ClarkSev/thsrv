
#include "thsrv/base/Logger.h"
#include "thsrv/base/TimeStamp.h"
#include "thsrv/net/Timer-my.h"
#include "thsrv/net/EventLoop.h"

#include <time.h>

using namespace std;
using namespace thsrv;
using namespace thsrv::net;

int g_cnt = 0;
Timer* g_tm = NULL;
void timer1_func()
{
    LOG_INFO<<"calling timer1_func now = "<<TimeStamp::now().toStringFmt();
}
void timer2_func()
{
    LOG_INFO<<"calling timer2_func now = "<<TimeStamp::now().toStringFmt();
    LOG_INFO<<"g_cnt = "<<g_cnt++;
    if(g_cnt==10){
        TimeStamp when = addTime(TimeStamp::now(), 3);
        g_tm->restart(when, timer1_func, 2);
    }
}

int main()
{
    EventLoop main_loop;
    TimeStamp when1 = addTime(TimeStamp::now(), 3),
              when2 = addTime(TimeStamp::now(), 5);
    Timer tm1(&main_loop, when1, timer1_func),
          tm2(&main_loop, when2, timer2_func, 1);

    LOG_INFO<<"main now = "<<TimeStamp::now().toStringFmt();
    g_tm = &tm2;
    tm1.start(); tm2.start();
    main_loop.start();
    return 0;
}


