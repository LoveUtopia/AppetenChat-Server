#pragma once
#include "TcpSever.h"
#include "../inetMediator/TcpSeverMediator.h"

TcpSever::TcpSever(INetMediator* m_pNetMediator):m_handle(NULL),m_sock(NULL),m_bStop(false){
	this->m_pNetMediator = m_pNetMediator;
}
TcpSever::~TcpSever() {
	closeinet();
}
//初始化
//加载库，创建套接字，绑定ip，监听，创建线程接收连接
bool TcpSever::initInet() {
	//加载库
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
	//创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket create error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket create success!" << endl;
	}
	//绑定ip
	SOCKADDR_IN addrin;
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(_DEF_TCP_PORT);
	addrin.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_sock, (SOCKADDR*)&addrin, sizeof(addrin));
	if (SOCKET_ERROR == err) {
		cout << "bind failed" << WSAGetLastError() << endl;
		return false;
	}
	//监听
	err = listen(m_sock, 10);
	if (SOCKET_ERROR == err) {
		cout << "listen error" << WSAGetLastError() << endl;
		return false;
	}
	//创建线程接收数据
	m_handle = (HANDLE)_beginthreadex(0, 0, &acceptThread,(void*)this, 0, NULL);
	if (m_handle) {
		m_listHandle.push_back(m_handle);
	}
	return true;
}
//
//接受连接
unsigned __stdcall TcpSever::acceptThread(void* mthis) {
	TcpSever* m_this = (TcpSever*)mthis;
	SOCKET m_sock = INVALID_SOCKET;
	SOCKADDR_IN addrClient;
	int addrClinetSize = sizeof(addrClient);
	unsigned int threadId;
	while (!m_this->m_bStop) {
		m_sock = accept(m_this->m_sock, (SOCKADDR*)&addrClient, &addrClinetSize);
		if (SOCKET_ERROR != m_sock) {
			//接收成功打印ip地址
			cout << inet_ntoa(addrClient.sin_addr)<<endl;
			//创建接收数据线程
			HANDLE m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, (void*)m_this, 0, &threadId);
			//将线程id和m_sock添加到map中
			if(m_handle)
			m_this->m_mapThreadIdToSock[threadId] = m_sock;
			//将句柄添加到list中
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

//接收数据
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
		//1.接收包大小
		//清零标志位
		nOffset = 0;
		nRecvNum = recv(sock, (char*)&packSize, sizeof(int), 0);
		if (nRecvNum > 0) {
			char* recvBuf = new char[packSize];
			//2.循环接收文件内容
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
			//3.将内容发送给媒介网络
			m_pNetMediator->dealData(recvBuf, nOffset, sock);
			
		}
		else {
			cout << "recive error!" << WSAGetLastError() << endl;
			break;
		}
	}
}
//发送数据
bool TcpSever::sendData(char* buf, int len, long to) {
	//1.判断数据是否合法
	if (!buf || len <= 0) {
		cout << "parameter error" << endl;
		return false;
	}
	//2.发送包大小
	int err = send(to, (char*)&len, sizeof(int), 0);
	if (err == SOCKET_ERROR) {
		cout << "send packet size error" << WSAGetLastError() << endl;
		return false;
	}
	//3.发送内容
	err = send(to, buf, len, 0);
	if(err == SOCKET_ERROR) {
		cout << "send packet content error" << WSAGetLastError() << endl;
		return false;
	}
	return 0;
}
//关闭网络
void TcpSever::closeinet() {
	//1.结束工作 2.关闭句柄
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
	
	//关闭套接字 卸载库
	for (auto ite = m_mapThreadIdToSock.begin(); ite != m_mapThreadIdToSock.end();) {
		if (ite->second != INVALID_SOCKET && ite->second) {
			closesocket(ite->second);
		}
		ite = m_mapThreadIdToSock.erase(ite);
	}
	WSACleanup();
}

