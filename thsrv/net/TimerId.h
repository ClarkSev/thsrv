/****************************************************************************
* @File:	TimerId封装类
* @Date:	2020-2-25
* @Author:	T.H.
* @Note:	使用timerfd_create产生 fd 将其封装为 TimerId，该类自动管理生存期
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

#include <sys/timerfd.h>

/*
	int timerfd_create(int clockid, int flags);
    int timerfd_settime(int fd, int flags,
                            const struct itimerspec *new_value,
                            struct itimerspec *old_value);
    int timerfd_gettime(int fd, struct itimerspec *curr_value);
*/

namespace thsrv
{
	
namespace net
{
	
class TimerId
{
public:
    // TimerId(const int fd);
    TimerId();
    ~TimerId();
    int fd()const{ return fd_; }
private:
    const int fd_;

};  //END TimerId CLASS
	
}  //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


#endif   //END THSRV_NET_TIMERID_H

