#pragma once
#include "UdpNet.h"
UdpNet::UdpNet() {}
UdpNet::~UdpNet() {}
//��ʼ��
bool UdpNet::initInet() {
	return 0;
}
//��������
void UdpNet::recvData() {}
//��������
bool UdpNet::sendData(char* buf, int len, long to) {
	return 0;
}
//�ر�����
void UdpNet::closeinet() {}

