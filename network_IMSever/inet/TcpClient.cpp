#pragma once
#include "TcpClient.h"
#include "../inetMediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* m_pNetMediator) :m_sock(NULL),m_handle(NULL),m_bStop(false) {
	this->m_pNetMediator = m_pNetMediator;
}
TcpClient::~TcpClient() {
	closeinet();
}
//��ʼ��
//���ؿ⣬�����׽��֣������߳�ר�Ž�������
bool TcpClient::initInet() {
	WORD w = MAKEWORD(2, 2);
	//���ؿ�
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
	//�����׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET) {
		cout << "socket create error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket create success!" << endl;
	}
	//����
	SOCKADDR_IN addrin;
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(_DEF_TCP_PORT);
	addrin.sin_addr.S_un.S_addr = inet_addr(_DEF_SEVER_IP);
	err = connect(m_sock, (SOCKADDR*)&addrin, sizeof(addrin));
	if (err == SOCKET_ERROR) {
		cout << "connect error" << WSAGetLastError() << endl;
		return false;
	}
	    //�����������ݵ��߳�
		// CreateThread��ExitThread��һ�ԣ�������߳�����ʹ��C++����ʱ��(����strcpy���ں���������ռ䲻�ͷ�)
		// /ExitThreadҲ�����ͷ�����ռ䣬�ͻ�����ڴ�й©��
		//_beginthreadex��_endthreadex��һ�ԣ�_endthreadex���Ȼ��տռ䣬�ٵ���ExitThread��
		// �����̵߳�ʱ�򣬲���ϵͳ����:������߳�id���ں˶���
		//�����߳�:1�������̹߳���:2���رվ��
	m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, (void*)this, 0, NULL);
	return true;
}
//�̺߳���
unsigned __stdcall TcpClient::recvThread(void* mthis) {
	TcpClient* m_this = (TcpClient*)mthis;
	m_this->recvData();
	return 1;
}
//��������
void TcpClient::recvData() {
	int packSize = 0;
	int nRecvNum = 0;
	int nOffset = 0;
	while (!m_bStop) {
		//1.���հ���С
		nOffset = 0;
		nRecvNum = recv(m_sock, (char*)&packSize, sizeof(int),0);
		if (nRecvNum > 0) {
			char* recvBuf = new char[packSize];
			//2.ѭ�������ļ�����
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
			//3.�����ݷ��͸�ý������
			m_pNetMediator->dealData(recvBuf, nOffset, m_sock);
		}
		else {
			cout << "recive error!" << WSAGetLastError() << endl;
			break;
		}
	}
}
//��������
bool TcpClient::sendData(char* buf, int len, long to) {
	//1.�ж����ݵĺϷ���
	if (!buf || len <= 0) {
		cout << "parameter error" << endl;
		return false;
	}
	//2.�������ݰ���С
	int err = send(m_sock, (char*)&len, sizeof(int), 0);
	if (err == SOCKET_ERROR) {
		cout << "send packetsize error" << endl;
		return false;
	}
	//3.�������ݰ�����
	err = send(m_sock, buf, len, 0);
	if (err == SOCKET_ERROR) {
		cout << "send packet content error" << endl;
		return false;
	}
	return true;
}
//�ر�����
void TcpClient::closeinet(){
	m_bStop = true;
	//1.�������� 2.�رվ��
	if (m_handle) {
		//�ȴ���ʱ ֱ��ɱ���߳�
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle,500)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	//�ر��׽��� ж�ؿ�
	if (m_sock != INVALID_SOCKET && m_sock) {
		closesocket(m_sock);
	}
	WSACleanup();
}

