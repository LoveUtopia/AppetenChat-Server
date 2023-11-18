#pragma once
#include "inetMediator.h"

class TcpSeverMediator : public INetMediator {
public:
	TcpSeverMediator();
	~TcpSeverMediator();
	bool openInet();
	bool sendData(char* data, int len, long to);
	bool dealData(char* data, int len, long from);
	void closeInet();

};