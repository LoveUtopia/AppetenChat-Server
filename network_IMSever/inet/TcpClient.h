#pragma once
#include "inet.h"
#include "packDef.h"
class TcpClient :public INet{
public:
	TcpClient(INetMediator* m_pNetMediator);
	~TcpClient();
	//��ʼ��
	bool initInet();
	//��������
	void recvData();
	//��������
	bool sendData(char* buf, int len, long to);
	//�ر�����
	void closeinet();
	
private:
	static  unsigned __stdcall recvThread(void* pArguments);
private:
	//�׽���
	SOCKET m_sock;
	//�߳̾��
	HANDLE m_handle;
	//�߳̽�����־
	bool m_bStop;
};