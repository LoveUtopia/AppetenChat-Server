#pragma once
#include <WinSock2.h>
#include <process.h>  
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
class INetMediator;
class INet {
public:
	INet(){}
	virtual ~INet() {};
	//初始化
	virtual bool initInet() = 0;
	//接收数据
	virtual void recvData() = 0;
	//发送数据
	virtual bool sendData(char* buf, int len, long to) = 0;
	//关闭网络
	virtual void closeinet() = 0;
protected:
	INetMediator* m_pNetMediator;
};