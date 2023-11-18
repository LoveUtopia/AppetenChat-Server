#pragma once
#include "inetMediator.h"
class TcpClientMediator: public INetMediator{
public:
	TcpClientMediator();
	~TcpClientMediator();
	bool openInet();
	bool sendData(char* data, int len, long to);
	bool dealData(char* data, int len, long from);
	void closeInet();
};