#pragma once

class INet;
class INetMediator {
public:
	INetMediator() {}
	virtual ~INetMediator(){}
	virtual bool openInet() = 0;
	virtual bool sendData(char* data,int len,long to) = 0;
	virtual bool dealData(char* data,int len,long from) = 0;
	virtual void closeInet() = 0;
protected:
	INet* m_pNet;
};