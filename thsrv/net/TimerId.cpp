/****************************************************************************
* @File:	TimerId封装类
* @Date:	2020-2-25
* @Author:	T.H.
* @Note:	使用timerfd_create产生 fd 将其封装为 TimerId，该类自动管理生存期
* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/net/TimerId.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

namespace thsrv
{

namespace net
{

/// START CLASS
TimerId::TimerId():\
fd_(::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC ))
{
    assert(fd_>=0);
}
TimerId::~TimerId()
{
    if(::close(fd_) != 0){
        LOG_FATAL<<"ERROR"<<strerror(errno);
    }
}
/// END CLASS

} //END NET NAMESPACE
	
}  //END THSRV NAMESPACE


