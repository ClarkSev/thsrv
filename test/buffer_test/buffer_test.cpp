/*
    @ file: test the buffer's getline
*/
// related the project
// c libraries
// c++ libraries
// others headers
#include "thsrv/net/Buffer.h"

#include <stdio.h>

#include <iostream>
#include <string>

using namespace thsrv::net;
// using namespace thsrv::base;
using namespace std;

int main()
{
    Buffer buf;
    const char tmp[] = "123\r\n456\r\n\r\n";
    buf.appendInRdBuf(tmp, strlen(tmp));
    const char* crlf = buf.findCRLFReadBuf();
    if(crlf){
        printf("the pos = %d\n", crlf - buf.beginReadOutBuf());
    }else{
        printf("the pos is NULL\n");
    }
    string lines = buf.getlineReadBuf();

    cout<<lines<<endl;
    crlf = buf.findCRLFReadBuf();

    lines = buf.getlineReadBuf();
    cout<<lines<<endl;

    lines = buf.getlineReadBuf();
    cout<<lines<<endl;
    cout<<buf.readable()<<endl;
    return 0;
}


