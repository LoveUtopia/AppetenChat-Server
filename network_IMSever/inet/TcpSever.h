#pragma once
#include <map>
#include <list>
#include "inet.h"
#include "packDef.h"
class TcpSever :public INet {
public:
	TcpSever(INetMediator* m_pNetMediator);
	~TcpSever();
	//初始化
	bool initInet();
	//接收数据
	void recvData();
	//发送数据
	bool sendData(char* buf, int len, long to);
	//关闭网络
	void closeinet();
private:
	//接受线程函数
	static  unsigned __stdcall acceptThread(void* pArguments);
	//接收数据线程函数
	static  unsigned __stdcall recvThread(void* pArguments);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	//线程结束标志位
	bool m_bStop;
	//线程id和socket对应
	map<unsigned int, SOCKET> m_mapThreadIdToSock;
	//装句柄的链表
	list<HANDLE> m_listHandle;
};