#pragma once
#include "inet.h"
class UdpNet :public INet {
public:
	UdpNet();
	~UdpNet();
	//��ʼ��
	bool initInet();
	//��������
	void recvData();
	//��������
	bool sendData(char* buf, int len, long to);
	//�ر�����
	void closeinet();

};