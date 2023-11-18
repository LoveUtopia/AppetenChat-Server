#pragma once
#include <iostream>
#include"inetMediator/inetMediator.h"
#include "inet/packDef.h"
#include "mysql/CMySql.h"
#include <map>
using namespace std;
class CKernel;
typedef void (CKernel::*PFun)(char* data, int len, long from);
class CKernel
{
public:
	CKernel();
	~CKernel();
	bool startSever();
	void closeSever();
	void dealData(char* data, int len, long from);
	//��ʼ��ָ������
	void initProtocolArray();
	//����ע��������Ϣ
	void dealRegisterRq(char* data, int len, long from);
	//����ע��ظ���Ϣ
	void dealLoginRq(char* data, int len, long from);
	//������������
	void dealChatRq(char* data, int len, long from);
	//����Ⱥ��������
	void dealGroupChatRq(char* data, int len, long from);
	//������������
	void dealOfflineRq(char* data, int len, long from);
	//������Ӻ�������
	void dealAddFriendRq(char* data, int len, long from);
	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* data, int len, long from);
	//����ɾ�����ѻظ�
	void dealDeleteFriendRq(char* data, int len, long from);
	//������Ⱥ������
	void dealCreateGroupRq(char* data, int len, long from);
	//�������Ⱥ����
	void dealAddGroupRq(char* data, int len, long from);
	//�������Ⱥ�ظ�
	void dealAddGroupRs(char* data, int len, long from);
	//���ͺ�����Ϣ���Լ�������Ϣ
	void sendUserInfo(int userId);
	//����Ⱥ��Ϣ�б�
	void sendGroupInfo(int userId);
	//����Ⱥ��Ա��Ϣ
	void sendGroupMember(int userId,int groupId); 
	//����id��ȡ�û���Ϣ
	void getUserInfo(int userId, STRU_TCP_FRIEND_INFO* rs);
	//����id��ȡȺ����Ϣ
	void getGroupInfo(int userId, STRU_TCP_GROUP_INFO* rs);
	//����id��ȡȺ��Ա��Ϣ
	void getMemberInfo(int groupId,int userId, STRU_TCP_MEMBER_INFO* rs);
	
public:
	static CKernel* m_kernel;
private:
	INetMediator* m_INetMediator;
	CMySql m_sql;
	PFun m_pfun[_DEF_PROTOCOL_COUNT];
	//�����û�id
	map<int, SOCKET> m_mapIdToSocket;
};

