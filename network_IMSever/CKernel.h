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
	//初始化指针数组
	void initProtocolArray();
	//处理注册请求信息
	void dealRegisterRq(char* data, int len, long from);
	//处理注册回复信息
	void dealLoginRq(char* data, int len, long from);
	//处理聊天请求
	void dealChatRq(char* data, int len, long from);
	//处理群聊天请求
	void dealGroupChatRq(char* data, int len, long from);
	//处理下线请求
	void dealOfflineRq(char* data, int len, long from);
	//处理添加好友请求
	void dealAddFriendRq(char* data, int len, long from);
	//处理添加好友回复
	void dealAddFriendRs(char* data, int len, long from);
	//处理删除好友回复
	void dealDeleteFriendRq(char* data, int len, long from);
	//处理创建群组请求
	void dealCreateGroupRq(char* data, int len, long from);
	//处理加入群请求
	void dealAddGroupRq(char* data, int len, long from);
	//处理加入群回复
	void dealAddGroupRs(char* data, int len, long from);
	//发送好友信息表以及个人信息
	void sendUserInfo(int userId);
	//发送群信息列表
	void sendGroupInfo(int userId);
	//发送群成员信息
	void sendGroupMember(int userId,int groupId); 
	//根据id获取用户信息
	void getUserInfo(int userId, STRU_TCP_FRIEND_INFO* rs);
	//根据id获取群组信息
	void getGroupInfo(int userId, STRU_TCP_GROUP_INFO* rs);
	//根据id获取群成员信息
	void getMemberInfo(int groupId,int userId, STRU_TCP_MEMBER_INFO* rs);
	
public:
	static CKernel* m_kernel;
private:
	INetMediator* m_INetMediator;
	CMySql m_sql;
	PFun m_pfun[_DEF_PROTOCOL_COUNT];
	//储存用户id
	map<int, SOCKET> m_mapIdToSocket;
};

