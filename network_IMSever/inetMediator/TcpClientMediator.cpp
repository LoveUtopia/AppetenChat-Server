#pragma once
#include "TcpClientMediator.h"
#include "../inet/TcpClient.h"
TcpClientMediator::TcpClientMediator(){
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator(){
	if (m_pNet) {
		m_pNet->closeinet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}
bool TcpClientMediator::openInet(){
	return m_pNet->initInet();
}
bool TcpClientMediator::sendData(char* data, int len, long to){
	return m_pNet->sendData(data,len,to);
}
bool TcpClientMediator::dealData(char* data, int len, long from){
	return false;
}
void TcpClientMediator::closeInet(){
	return m_pNet->closeinet();
}