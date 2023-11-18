#pragma once
#include "inet.h"
class UdpNet :public INet {
public:
	UdpNet();
	~UdpNet();
	//初始化
	bool initInet();
	//接收数据
	void recvData();
	//发送数据
	bool sendData(char* buf, int len, long to);
	//关闭网络
	void closeinet();

};