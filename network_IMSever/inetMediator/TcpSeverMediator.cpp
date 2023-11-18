#pragma once
#include "TcpSeverMediator.h"
#include "../inet/TcpSever.h"
#include "../CKernel.h"
TcpSeverMediator::TcpSeverMediator() {
	m_pNet = new TcpSever(this);
}
TcpSeverMediator::~TcpSeverMediator() {}
bool TcpSeverMediator::openInet() {
	return m_pNet->initInet();
}
bool TcpSeverMediator::sendData(char* data, int len, long to) {
	return m_pNet->sendData(data, len, to);
}
bool TcpSeverMediator::dealData(char* data, int len, long from) {
	CKernel::m_kernel->dealData(data, len, from);
	return true;
}
void TcpSeverMediator::closeInet() {
	return m_pNet->closeinet();
}