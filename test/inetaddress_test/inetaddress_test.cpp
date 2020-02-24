
#include "thsrv/base/Logger.h"
#include "thsrv/base/Thread.h"
#include "thsrv/net/InetAddress.h"
// #include "thsrv/net/TcpServer.h"

#include <string>

using namespace std;
using namespace thsrv;
using namespace thsrv::base;
using namespace thsrv::net;

int main()
{
    std::string ip = "1.2.3.4";
    uint16_t port = 1234;
    InetAddress addr(ip,port);
    LOG_INFO<<"addr = "<<addr.toIpAndPort();
    InetAddress addr1;
    LOG_INFO<<"addr1 = "<<addr1.toIpAndPort();
    return 0;
}