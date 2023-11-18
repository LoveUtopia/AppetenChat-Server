#pragma once
#include "TcpSever.h"
#include "../inetMediator/TcpSeverMediator.h"

TcpSever::TcpSever(INetMediator* m_pNetMediator):m_handle(NULL),m_sock(NULL),m_bStop(false){
	this->m_pNetMediator = m_pNetMediator;
}
TcpSever::~TcpSever() {
	closeinet();
}
//��ʼ��
//���ؿ⣬�����׽��֣���ip�������������߳̽�������
bool TcpSever::initInet() {
	//���ؿ�
	WORD w = MAKEWORD(2, 2);
	WSADATA wdata;
	int err = WSAStartup(w, &wdata);
	if (0 != err) {
		cout << "WSAStartup failed" << WSAGetLastError() << endl;
		return false;
	}
	if (HIBYTE(wdata.wVersion) != 2 || LOBYTE(wdata.wVersion) != 2) {
		cout << "WSAVersion error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "WSAStartup success!" << endl;
	}
	//�����׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket create error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket create success!" << endl;
	}
	//��ip
	SOCKADDR_IN addrin;
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(_DEF_TCP_PORT);
	addrin.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (SOCKADDR*)&addrin, sizeof(addrin));
	if (SOCKET_ERROR == err) {
		cout << "bind failed" << WSAGetLastError() << endl;
		return false;
	}
	//����
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err) {
		cout << "listen error" << WSAGetLastError() << endl;
		return false;
	}
	//�����߳̽�������
	m_handle = (HANDLE)_beginthreadex(0, 0, &acceptThread,(void*)this, 0, NULL);
	if (m_handle) {
		m_listHandle.push_back(m_handle);
	}
	return true;
}
//
//��������
unsigned __stdcall TcpSever::acceptThread(void* mthis) {
	TcpSever* m_this = (TcpSever*)mthis;
	SOCKET m_sock = INVALID_SOCKET;
	SOCKADDR_IN addrClient;
	int addrClinetSize = sizeof(addrClient);
	unsigned int threadId;
	while (!m_this->m_bStop) {
		m_sock = accept(m_this->m_sock, (SOCKADDR*)&addrClient, &addrClinetSize);
		if (SOCKET_ERROR != m_sock) {
			//���ճɹ���ӡip��ַ
			cout << inet_ntoa(addrClient.sin_addr)<<endl;
			//�������������߳�
			HANDLE m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, (void*)m_this, 0, &threadId);
			//���߳�id��m_sock��ӵ�map��
			if(m_handle)
			m_this->m_mapThreadIdToSock[threadId] = m_sock;
			//�������ӵ�list��
			m_this->m_listHandle.push_back(m_handle);
		}
		else {
			cout << "accept error" << WSAGetLastError() << endl;
			break;
		}
		
	}
	return true;
}
unsigned __stdcall TcpSever::recvThread(void* mthis)
{
	TcpSever* m_this = (TcpSever*)mthis;
	m_this->recvData();
	return 0;
}

//��������
void TcpSever::recvData() {
	Sleep(1);
	SOCKET sock = m_mapThreadIdToSock[GetCurrentThreadId()];
	if (sock == INVALID_SOCKET||!sock) {
		cout << "socket error" << WSAGetLastError() << endl;
		return;
	}
	int packSize = 0;
	int nRecvNum = 0;
	int nOffset = 0;
	while (!m_bStop) {
		//1.���հ���С
		//�����־λ
		nOffset = 0;
		nRecvNum = recv(sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			char* recvBuf = new char[packSize];
			//2.ѭ�������ļ�����
			while (packSize > 0) {
				nRecvNum = recv(sock, recvBuf + nOffset, packSize, 0);
				if (nRecvNum > 0) {
					packSize -= nRecvNum;
					nOffset += nRecvNum;
				}
				else {
					cout << "repeat recive error!" << WSAGetLastError() << endl;
					break;
				}
			}
			//3.�����ݷ��͸�ý������
			m_pNetMediator->dealData(recvBuf, nOffset, sock);
			
		}
		else {
			cout << "recive error!" << WSAGetLastError() << endl;
			break;
		}
	}
}
//��������
bool TcpSever::sendData(char* buf, int len, long to) {
	//1.�ж������Ƿ�Ϸ�
	if (!buf || len <= 0) {
		cout << "parameter error" << endl;
		return false;
	}
	//2.���Ͱ���С
	int err = send(to, (char*)&len, sizeof(int), 0);
	if (err == SOCKET_ERROR) {
		cout << "send packet size error" << WSAGetLastError() << endl;
		return false;
	}
	//3.��������
	err = send(to, buf, len, 0);
	if(err == SOCKET_ERROR) {
		cout << "send packet content error" << WSAGetLastError() << endl;
		return false;
	}
	return 0;
}
//�ر�����
void TcpSever::closeinet() {
	//1.�������� 2.�رվ��
	m_bStop = true;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		if (*ite) {
			if (WAIT_TIMEOUT == WaitForSingleObject(*ite, 500)) {
				TerminateThread(*ite, -1);
			}
			CloseHandle(*ite);
			*ite = NULL;
		}
		ite = m_listHandle.erase(ite);
	}
	
	//�ر��׽��� ж�ؿ�
	for (auto ite = m_mapThreadIdToSock.begin(); ite != m_mapThreadIdToSock.end();) {
		if (ite->second != INVALID_SOCKET && ite->second) {
			closesocket(ite->second);
		}
		ite = m_mapThreadIdToSock.erase(ite);
	}
	WSACleanup();
}

