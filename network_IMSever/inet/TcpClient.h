#pragma once
#include "inet.h"
#include "packDef.h"
class TcpClient :public INet{
public:
	TcpClient(INetMediator* m_pNetMediator);
	~TcpClient();
	//初始化
	bool initInet();
	//接收数据
	void recvData();
	//发送数据
	bool sendData(char* buf, int len, long to);
	//关闭网络
	void closeinet();
	
private:
	static  unsigned __stdcall recvThread(void* pArguments);
private:
	//套接字
	SOCKET m_sock;
	//线程句柄
	HANDLE m_handle;
	//线程结束标志
	bool m_bStop;
};