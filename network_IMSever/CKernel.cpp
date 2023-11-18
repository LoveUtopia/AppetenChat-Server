#pragma once
#include "CKernel.h"
#include"inetMediator/TcpSeverMediator.h"

CKernel* CKernel::m_kernel = nullptr;
CKernel::CKernel() {
	m_kernel = this;
	m_INetMediator = new TcpSeverMediator;
}
CKernel::~CKernel() {
	closeSever();
}
bool CKernel::startSever() {
	initProtocolArray();
	if (!m_sql.ConnectMySql("127.0.0.1", "root", "zsh2223221198", "qqsql")) {
		cout << "sql connect error" << endl;
		return false;
	}
	if (!m_INetMediator->openInet()) {
		cout << "m_INetMediator open error" << endl;
		return false;
	}
	return true;
}
void CKernel::closeSever() {
	if (m_INetMediator) {
		m_INetMediator->closeInet();
		delete m_INetMediator;
		m_INetMediator = nullptr;
	}
}
void CKernel::dealData(char* data, int len, long from) {
	//取出协议头
	packType type = *(packType*)data;
	type = type - _DEF_PROTOCOL_BASE - 1;
	//判断合法性是否越界
	if (type >= 0 && type < _DEF_PROTOCOL_COUNT) {
		PFun func = m_pfun[type];
		if (func) {
			(this->*func)(data,len,from);
		}
		else {
			//1.结构体初始化出错 2.初始化数组出问题
			cout << "type2 error" << type << endl;
		}
	}
	else {
		//1.结构体初始化出错 2.offset未置零
		cout << "type1 error:" << type << endl;
	}
	delete[] data;
}

void CKernel::initProtocolArray()
{
	memset(m_pfun, 0, sizeof(m_pfun));
	m_pfun[_DEF_TCP_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_pfun[_DEF_TCP_LOGIN_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_pfun[_DEF_TCP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_pfun[_DEF_TCP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_pfun[_DEF_TCP_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_pfun[_DEF_TCP_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
	m_pfun[_DEF_TCP_DELETE_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealDeleteFriendRq;
	m_pfun[_DEF_TCP_CREATE_GROUP_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealCreateGroupRq;
	m_pfun[_DEF_TCP_GROUP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupChatRq;
	m_pfun[_DEF_TCP_ADD_GROUP_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddGroupRq;
	m_pfun[_DEF_TCP_ADD_GROUP_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddGroupRs;
}

void CKernel::dealRegisterRq(char* data, int len, long from)
{
	//1.拆包
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)data;
	//2.判断数据合法性
	
	
	//3.根据昵称查询
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name from t_user where name = '%s';",rq->name);
	list<string> lst;
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
		cout << "select error" << sqlBuf << endl;
		return;
	}
	STRU_TCP_REGISTER_RS rs;
	if (lst.size() > 0) {
		cout << "昵称查询重复" << endl;
		rs.result = register_name_repeat;
	}
	else {
		//4.根据电话号查询
		sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
		if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
			cout << "select error" << sqlBuf << endl;
			return;
		}
		else {
			if (lst.size() > 0) {
				cout << "电话号查询重复" << endl;
				rs.result = register_tel_repeat;
			}
			else {
				//5.插入数据
				sprintf_s(sqlBuf,"insert into t_user (name,tel,password,icon,feeling) values('%s','%s','%s',1,'nothing');",rq->name,rq->tel,rq->password);
				if (!m_sql.UpdateMySql(sqlBuf)) {
					cout << "插入失败" << sqlBuf << endl;
					return;
				}
				rs.result = register_success;
			}
		}
	}
	
	
	

	m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::dealLoginRq(char* data, int len, long from)
{
	//拆包
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)data;
	//判断合法性

	//根据电话查密码
	STRU_TCP_LOGIN_RS rs;
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)){
		cout<<"查询失败"<<sqlBuf<<endl;
		return;
	}
	else {
		//电话号码为空
		if (lst.size()==0) {
			rs.result = login_tel_not_exist;
		}
		else {
			lst.pop_front();
			//根据电话号查密码
			sprintf_s(sqlBuf, "select password,id from t_user where tel = '%s';", rq->tel);
			if (!m_sql.SelectMySql(sqlBuf, 2, lst)) {
				cout << "查询失败" << sqlBuf << endl;
				return;
			}
			else {
				string psd = rq->password;
				string temp = lst.front();
				lst.pop_front();
				//获取用户id
				int userId = stoi(lst.front());
				lst.pop_front();

				
				if (psd != temp) {
					rs.result = login_password_error;
				}
				else {
					rs.result = login_success;
					//成功后保存id
					m_mapIdToSocket[userId] = from;
					//回复信息加id
					rs.id = userId;
					m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
					sendUserInfo(userId);
					sendGroupInfo(userId);
					
					return;
				}
			}
		}
	}
	

	m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::dealChatRq(char* data, int len, long from)
{
	//1.拆包
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)data;
	//2.看好友是否在线
	if (m_mapIdToSocket.count(rq->friendId)>0) {
		//先线，发送给另一个好友
		m_INetMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//不在线，回复失败
		STRU_TCP_CHAT_RS rs;
		rs.result = send_failed;
		rs.friendId = rq->friendId;
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}

void CKernel::dealGroupChatRq(char* data, int len, long from)
{
	//1.拆包
	STRU_TCP_GROUP_CHAT_RQ* rq = (STRU_TCP_GROUP_CHAT_RQ*)data;
	//2.看查询群成员列表
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", rq->groupId);
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
		cout << "查询失败" << sqlBuf << endl;
		return;
	}
	while (lst.size() > 0) {
		//获取用户id
		int userId = stoi(lst.front());
		lst.pop_front();
		if (m_mapIdToSocket.count(userId) > 0) {
			//在线发送给群聊成员
			m_INetMediator->sendData(data, len, m_mapIdToSocket[userId]);
		}
	}
}

void CKernel::dealOfflineRq(char* data, int len, long from)
{
	//拆包
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)data;
	//获取好友列表
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", rq->userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	int friendId = 0;
	while (idList.size() > 0) {
		//取出id
		friendId = stoi(idList.front());
		idList.pop_front();
		//根据id查询是否在线
		if (m_mapIdToSocket.find(friendId)!=m_mapIdToSocket.end()) {
		//在线发送下线请求
			m_INetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}
	//回收资源
	auto ite = m_mapIdToSocket.find(rq->userId);
	if (ite != m_mapIdToSocket.end()) {
		closesocket(from);
		m_mapIdToSocket.erase(ite);
	}
}

void CKernel::dealAddFriendRq(char* data, int len, long from)
{
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)data;
	//判断是否有好友这个人
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	//判断查询结果
	STRU_TCP_ADD_FRIEND_RS rs;
	if (0 == List.size()) {
		//没有该人
		//设置结果
 		rs.result = add_friend_no_this_user;
		rs.userId = rq->userId;
		strcpy_s(rs.friendName, rq->friendName);
		//回复给原来的人
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//有该人
		//取出好友id
		int friendId = stoi(List.front());
		List.pop_front();
		//好友存在判断是否在线
		if (m_mapIdToSocket.count(friendId) > 0) {
			//转发给另一个好友
			m_INetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//失败不在线
			rs.result = add_friend_user_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.friendName, rq->friendName);
			//回复给原来的人
			m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}

}

void CKernel::dealAddFriendRs(char* data, int len, long from)
{
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)data;
	//判断添加结果是否成功
	if (add_friend_success == rs->result) {
		//把好友关系写入数据库，写两次，用事务双向存储
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "insert into t_friend values (%d,%d);", rs->friendId,rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "添加好友信息失败:" << sqlBuf << endl;
		}
		sprintf_s(sqlBuf, "insert into t_friend values (%d,%d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "添加好友信息失败:" << sqlBuf << endl;
		}
		//4.更新好友列表，双向的发送，只用一次就行 谁的id无所谓
		sendUserInfo(rs->userId);
	}
	//5.把加好友结果消息发送给 申请者
	m_INetMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
}

void CKernel::dealDeleteFriendRq(char* data, int len, long from)
{
	STRU_TCP_DELETE_FRIEND_RQ* rq = (STRU_TCP_DELETE_FRIEND_RQ*)data;
	//直接在数据库里删除好友关系,删除两次
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "delete from t_friend where idA = %d and idB = %d;",rq->friendId,rq->userId);
	if (!m_sql.UpdateMySql(sqlBuf)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	
	sprintf_s(sqlBuf, "delete from t_friend where idA = %d and idB = %d;", rq->userId, rq->friendId);
	if (!m_sql.UpdateMySql(sqlBuf)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	//回复
	STRU_TCP_DELETE_FRIEND_RS rs;
	rs.friendId = rq->friendId;
	rs.userId = rq->userId;
	rs.result = _delete_success;
	//回复给 删除者
	m_INetMediator->sendData((char*)&rs, sizeof(rs),from);
	//如果被删除者在线 那么回复给 被删除者
	if (m_mapIdToSocket.count(rq->friendId)) {
		m_INetMediator->sendData((char*)&rs, sizeof(rs), m_mapIdToSocket[rq->friendId]);
	}
	
}

void CKernel::dealCreateGroupRq(char* data, int len, long from)
{
	//拆包
	STRU_TCP_CREATE_GROUP_RQ* rq = (STRU_TCP_CREATE_GROUP_RQ*)data;
	//查询是否存在该群组
	STRU_TCP_CREATE_GROUP_RS rs;
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_user where name = '%s';", rq->groupName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}

	if (List.size() > 0) {
		rs.result = _create_group_name_repeat;
	}
	else {
		rs.result = _create_group_success;
		//写入到数据库
		sprintf_s(sqlBuf, "insert into t_group_name (name) values('%s');", rq->groupName);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "写入失败:" << sqlBuf << endl;
		}
		sprintf_s(sqlBuf, "insert into t_group_user (groupId,userId,isLeader) values((select groupId from t_group_name where name = '%s'),%d,'yes');",rq->groupName, rq->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "写入失败:" << sqlBuf << endl;
		}
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
		sendGroupInfo(rq->userId);
		return;
	}

	m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	
	return;
}

void CKernel::dealAddGroupRq(char* data, int len, long from)
{
	//拆包
	STRU_TCP_ADD_GROUP_RQ* rq = (STRU_TCP_ADD_GROUP_RQ*)data;
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_name where name = '%s';", rq->groupName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	//判断查询结果
	STRU_TCP_ADD_GROUP_RS rs;
	if (0 == List.size()) {
		//没有该群
		//设置结果
		rs.result = add_group_no_this_group;
		rs.userId = rq->userId;
		strcpy_s(rs.groupName, rq->groupName);
		//回复给原来的人
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//有该群
		//取出群id
		int groupId = stoi(List.front());
		List.pop_front();
		//查找群主
		sprintf_s(sqlBuf, "select userId from t_group_user where groupId = %d and isLeader = 'yes';", groupId);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "查询好友信息列表失败:" << sqlBuf << endl;
		}
		//取出群主id
		int leaderId = stoi(List.front());
		List.pop_front();
		//群存在判断群主是否在线
		if (m_mapIdToSocket.count(leaderId) > 0) {
			//转发给另一个群主
			m_INetMediator->sendData(data, len, m_mapIdToSocket[leaderId]);
		}
		else {
			//失败不在线
			rs.result = add_group_leader_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.groupName, rq->groupName);
			//回复给原来的人
			m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}

}

void CKernel::dealAddGroupRs(char* data, int len, long from)
{
	STRU_TCP_ADD_GROUP_RS* rs = (STRU_TCP_ADD_GROUP_RS*)data;
	//判断添加结果是否成功
	if (add_group_success == rs->result) {
		//把关系写入数据库，用事务双向存储
		//查找群id
		list<string> List;
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select groupId from t_group_name where name = '%s';", rs->groupName);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "查询好友信息列表失败:" << sqlBuf << endl;
		}
		//取出群id
		int groupId = stoi(List.front());
		List.pop_front();

		sprintf_s(sqlBuf, "insert into t_group_user (groupId,userId) values (%d,%d);", groupId, rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "添加好友信息失败:" << sqlBuf << endl;
		}
		//4.更新群通知所有人更新群
		sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", groupId);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "查询好友信息列表失败:" << sqlBuf << endl;
		}
		while (List.size() > 0) {
			int userId = stoi(List.front());
			List.pop_front();
			sendGroupInfo(userId);
		}
		
	}
	//5.把加好友结果消息发送给 申请者
	m_INetMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
}

void CKernel::sendUserInfo(int userId)
{
	cout << __FUNCTION__ << endl;
	//1.查询自己信息
	STRU_TCP_FRIEND_INFO userInfo;
	getUserInfo(userId, &userInfo);
	//2.将自己信息发送给服务端
	if (m_mapIdToSocket.count(userId)) {
		m_INetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}//3.查询好友信息列表的id
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "查询好友信息列表失败:" << sqlBuf << endl;
	}
	//4.根据id查询信息并发送给服务端
	int friendId = 0;
	STRU_TCP_FRIEND_INFO friendInfo;
	while (idList.size() > 0) {
		//取出id
		friendId = stoi(idList.front());
		idList.pop_front();
		//根据id查询
		getUserInfo(friendId, &friendInfo);
		//把好友信息发送给客户端
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
		}
		//把自己上线发送给好友
		if (m_mapIdToSocket.count(friendId)) {
			m_INetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

void CKernel::sendGroupInfo(int userId)
{
	//1.查询群信息列表的id
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_user where userId = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "查询群组信息列表失败:" << sqlBuf << endl;
	}
	//2.根据群组信息发送给客户端

	int groupId = 0;
	STRU_TCP_GROUP_INFO groupInfo;
	while (idList.size() > 0) {
		//取出groupid
		groupId = stoi(idList.front());
		idList.pop_front();
		//根据id查询群组
		getGroupInfo(groupId, &groupInfo);
		//把群组信息发送给客户端
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&groupInfo, sizeof(groupInfo), m_mapIdToSocket[userId]);
		}
		//发送群成员信息
		sendGroupMember(userId, groupId);
	}
}

void CKernel::sendGroupMember(int userId,int groupId)
{
	//取出该群所有成员id列表
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", groupId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "查询群组信息列表失败:" << sqlBuf << endl;
	}
	int memberId = 0;
	STRU_TCP_MEMBER_INFO memberInfo;
	while (idList.size() > 0) {
		memberId = stoi(idList.front());
		idList.pop_front();
		//取出群成员信息
		getMemberInfo(groupId,memberId, &memberInfo);
		//把群成员信息发送给登陆者
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&memberInfo, sizeof(memberInfo), m_mapIdToSocket[userId]);
		}
	}
}
void CKernel::getMemberInfo(int groupId, int userId, STRU_TCP_MEMBER_INFO* rs)
{
	//判断在线与否
	rs->friendId = userId;
	if (m_mapIdToSocket.count(userId)) {
		rs->status = _status_online;
	}
	else {
		rs->status = _status_offline;
	}
	//在数据库里查询
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 3, lst)) {
		cout << "查询失败:" << sqlBuf << endl;
	}

	if (3 == lst.size()) {
		strcpy(rs->name, lst.front().c_str());
		lst.pop_front();

		rs->iconId = stoi(lst.front());
		lst.pop_front();

		strcpy(rs->feeling, lst.front().c_str());
		lst.pop_front();
	}
	rs->groupId = groupId;
}
void CKernel::getUserInfo(int userId, STRU_TCP_FRIEND_INFO* rs)
{
	//判断在线与否
	rs->friendId = userId;
	if (m_mapIdToSocket.count(userId)) {
		rs->status = _status_online;
	}
	else {
		rs->status = _status_offline;
	}
	//在数据库里查询
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 3, lst)) {
		cout << "查询失败:" << sqlBuf << endl;
	}

	if (3 == lst.size()) {
		strcpy(rs->name, lst.front().c_str());
		lst.pop_front();

		rs->iconId=stoi(lst.front());
		lst.pop_front();

		strcpy(rs->feeling,lst.front().c_str());
		lst.pop_front();
	}
}

void CKernel::getGroupInfo(int groupId, STRU_TCP_GROUP_INFO* rs)
{
	rs->groupId = groupId;
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,iconId from t_group_name where groupId = '%d';", groupId);
	if (!m_sql.SelectMySql(sqlBuf, 2, lst)) {
		cout << "查询失败:" << sqlBuf << endl;
	}

	if (2 == lst.size()) {
		strcpy(rs->groupName, lst.front().c_str());
		lst.pop_front();

		rs->iconId = stoi(lst.front());
		lst.pop_front();
	}
}




