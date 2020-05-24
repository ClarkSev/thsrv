/****************************************************************************
* @File:	当前线程封装类
* @Date:	2020-2-10
* @Author:	T.H.
* @Note:	功能说明：

* @Version:	V0.1
****************************************************************************/

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/CurrentThread.h"

namespace thsrv
{

namespace CurrentThread
{

__thread pid_t cacheTid_ = 0;

} //END CurrentThread NAMESPACE
	
}  //END THSRV NAMESPACE
