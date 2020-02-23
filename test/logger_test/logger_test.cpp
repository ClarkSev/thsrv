/*
	╡Бйтнд╪Ч
*/

#include "thsrv/base/StringPiece.h"
//#include "base/LogStream.h"
#include "thsrv/base/Logger.h"

#include <iostream>

using namespace std;
using thsrv::StringPiece;
//using thsrv::LogStream;
using namespace thsrv;

int main()
{
	StringPiece str1 = "123455";
	string str = "1234";
	LOG_INFO << 123 <<"\n";
	LOG_WARN<<"12842"<<" str1 = "<<str1<<"  str = "<<str<<"\n";
	return 0;
}



