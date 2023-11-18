#pragma once
#include "TcpClient.h"
#include "../inetMediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* m_pNetMediator) :m_sock(NULL),m_handle(NULL),m_bStop(false) {
	this->m_pNetMediator = m_pNetMediator;
}
TcpClient::~TcpClient() {
	closeinet();
}
//初始化
//加载库，创建套接字，创建线程专门接收数据
bool TcpClient::initInet() {
	WORD w = MAKEWORD(2, 2);
	//加载库
	WSADATA wdata;
	int err = WSAStartup(w, &wdata);
	if (0 != err) {
		cout << "WSAStartup error" << WSAGetLastError() << endl;
		return false;
	}
	if (HIBYTE(wdata.wVersion) != 2 || LOBYTE(wdata.wVersion) != 2) {
		cout << "WDATA verion error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "WSAStartup success!" << endl;
	}
	//创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "socket create error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket create success!" << endl;
	}
	//连接
	SOCKADDR_IN addrin;
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(_DEF_TCP_PORT);
	addrin.sin_addr.S_un.S_addr = inet_addr(_DEF_SEVER_IP);
	err = connect(m_sock, (SOCKADDR*)&addrin, sizeof(addrin));
	if (err == SOCKET_ERROR) {
		cout << "connect error" << WSAGetLastError() << endl;
		return false;
	}
	    //创建接收数据的线程
		// CreateThread和ExitThread是一对，如果在线程里面使用C++运行时库(例如strcpy，在函数中申请空间不释放)
		// /ExitThread也不会释放这个空间，就会造成内存泄漏。
		//_beginthreadex和_endthreadex是一对，_endthreadex会先回收空间，再调用ExitThread。
		// 创建线程的时候，操作系统分配:句柄、线程id、内核对象
		//回收线程:1、结束线程工作:2、关闭句柄
	m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, (void*)this, 0, NULL);
	return true;
}
//线程函数
unsigned __stdcall TcpClient::recvThread(void* mthis) {
	TcpClient* m_this = (TcpClient*)mthis;
	m_this->recvData();
	return 1;
}
//接收数据
void TcpClient::recvData() {
	int packSize = 0;
	int nRecvNum = 0;
	int nOffset = 0;
	while (!m_bStop) {
		//1.接收包大小
		nOffset = 0;
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(int),0);
		if (nRecvNum > 0) {
			char* recvBuf = new char[packSize];
			//2.循环接收文件内容
			while (packSize>0) {
				nRecvNum = recv(m_sock, recvBuf + nOffset, packSize,0);
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
			m_pNetMediator->dealData(recvBuf, nOffset, m_sock);
		}
		else {
			cout << "recive error!" << WSAGetLastError() << endl;
			break;
		}
	}
}
//发送数据
bool TcpClient::sendData(char* buf, int len, long to) {
	//1.判断数据的合法性
	if (!buf || len <= 0) {
		cout << "parameter error" << endl;
		return false;
	}
	//2.发送数据包大小
	int err = send(m_sock, (char*)&len, sizeof(int), 0);
	if (err == SOCKET_ERROR) {
		cout << "send packetsize error" << endl;
		return false;
	}
	//3.发送数据包内容
	err = send(m_sock, buf, len, 0);
	if (err == SOCKET_ERROR) {
		cout << "send packet content error" << endl;
		return false;
	}
	return true;
}
//关闭网络
void TcpClient::closeinet(){
	m_bStop = true;
	//1.结束工作 2.关闭句柄
	if (m_handle) {
		//等待超时 直接杀死线程
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle,500)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	//关闭套接字 卸载库
	if (m_sock != INVALID_SOCKET && m_sock) {
		closesocket(m_sock);
	}
	WSACleanup();
}

