/****************************************************************************
* @File:	消息解析中间件
* @Date:	2020-5-26
* @Author:	T.H.
* @Note:	间于TCP传输层与应用层之间，用于数据打包，分包。
    保证当数据出现粘包，或分包时，应用层接收到的数据是完整的一条消息
* @Version:	V0.1
****************************************************************************/
#pragma once

#ifndef THSRV_NET_MESSAGEPARSE_H
#define THSRV_NET_MESSAGEPARSE_H

//----------------------Include Header----------------------------
//related current file header
//c library header
//c++ library header
//related others' project file header
#include "thsrv/base/TimeStamp.h"
#include "thsrv/base/Logger.h"
#include "thsrv/net/Callback.h"
#include "thsrv/net/Buffer.h"
#include "thsrv/net/TcpConnection.h"

#include <string>

namespace thsrv
{

namespace net
{

void parse_msg_to_buffer(Buffer& buf)
{
    size_t sz_msg = buf.readable();
    buf.prepend(&sz_msg, sizeof(sz_msg));
}

std::string buffer_to_parse_msg(const TcpConnectionPtr& conn, Buffer& buf,\
    TimeStamp receiveTime, const MessageCallback& msgcb)
{
    while(buf.readable() >= Buffer::kPrependBytes){
        const void* data = buf.peek();
        size_t sz_msg = *static_cast<const size_t*>(data);
        if(sz_msg<0){
            LOG_ERR<<"Invalid Message Length: "<<static_cast<int>(sz_msg)<<"\n";
            conn->shutdown();
            break;
        }else if(buf.readable() > sz_msg+static_cast<size_t>(kPrependBytes)){
            buf.retrieve(Buffer::kPrependBytes);
            std::string msg(buf.peek(), sz_msg);
            msgcb(conn, msg, receiveTime);
            buf.retrieve(sz_msg);
        }else{
            break;
        }
    }
}

}  // END NAMESPACE NET

} // END NAMESPACE thsrv



#endif  // THSRV_NET_MESSAGEPARSE_H