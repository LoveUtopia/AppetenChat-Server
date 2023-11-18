#pragma once
#include <map>
#include <list>
#include "inet.h"
#include "packDef.h"
class TcpSever :public INet {
public:
	TcpSever(INetMediator* m_pNetMediator);
	~TcpSever();
	//��ʼ��
	bool initInet();
	//��������
	void recvData();
	//��������
	bool sendData(char* buf, int len, long to);
	//�ر�����
	void closeinet();
private:
	//�����̺߳���
	static  unsigned __stdcall acceptThread(void* pArguments);
	//���������̺߳���
	static  unsigned __stdcall recvThread(void* pArguments);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	//�߳̽�����־λ
	bool m_bStop;
	//�߳�id��socket��Ӧ
	map<unsigned int, SOCKET> m_mapThreadIdToSock;
	//װ���������
	list<HANDLE> m_listHandle;
};