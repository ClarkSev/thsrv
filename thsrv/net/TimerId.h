/****************************************************************************
* @File:	TimerId封装类
* @Date:	2020-2-26
* @Author:	T.H.
* @Note:	该类为Timer的ID标识，用于快速定位Timer，同时这是唯一的
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_TIMERID_H
#define THSRV_NET_TIMERID_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/Logger.h"


namespace thsrv
{
	
namespace net
{
	
class Timer;

class TimerId
{
public:
    // TimerId(const int fd);
    TimerId(Timer* timerArg, uint64_t idArg):\
            timer_(timerArg),id_(idArg){}
    TimerId():timer_(NULL),id_(0){}
    uint64_t id()const { return id_;}
    Timer* timer(){ return timer_; }
private:
    Timer* timer_;
    uint64_t id_;

};  //END TimerId CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TIMERID_H

