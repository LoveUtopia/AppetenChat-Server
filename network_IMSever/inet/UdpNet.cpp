#pragma once
#include "UdpNet.h"
UdpNet::UdpNet() {}
UdpNet::~UdpNet() {}
//初始化
bool UdpNet::initInet() {
	return 0;
}
//接收数据
void UdpNet::recvData() {}
//发送数据
bool UdpNet::sendData(char* buf, int len, long to) {
	return 0;
}
//关闭网络
void UdpNet::closeinet() {}

