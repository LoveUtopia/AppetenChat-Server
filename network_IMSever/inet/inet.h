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
	//��ʼ��
	virtual bool initInet() = 0;
	//��������
	virtual void recvData() = 0;
	//��������
	virtual bool sendData(char* buf, int len, long to) = 0;
	//�ر�����
	virtual void closeinet() = 0;
protected:
	INetMediator* m_pNetMediator;
};