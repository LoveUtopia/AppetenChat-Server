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
	//ȡ��Э��ͷ
	packType type = *(packType*)data;
	type = type - _DEF_PROTOCOL_BASE - 1;
	//�жϺϷ����Ƿ�Խ��
	if (type >= 0 && type < _DEF_PROTOCOL_COUNT) {
		PFun func = m_pfun[type];
		if (func) {
			(this->*func)(data,len,from);
		}
		else {
			//1.�ṹ���ʼ������ 2.��ʼ�����������
			cout << "type2 error" << type << endl;
		}
	}
	else {
		//1.�ṹ���ʼ������ 2.offsetδ����
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
	//1.���
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)data;
	//2.�ж����ݺϷ���
	
	
	//3.�����ǳƲ�ѯ
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name from t_user where name = '%s';",rq->name);
	list<string> lst;
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
		cout << "select error" << sqlBuf << endl;
		return;
	}
	STRU_TCP_REGISTER_RS rs;
	if (lst.size() > 0) {
		cout << "�ǳƲ�ѯ�ظ�" << endl;
		rs.result = register_name_repeat;
	}
	else {
		//4.���ݵ绰�Ų�ѯ
		sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
		if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
			cout << "select error" << sqlBuf << endl;
			return;
		}
		else {
			if (lst.size() > 0) {
				cout << "�绰�Ų�ѯ�ظ�" << endl;
				rs.result = register_tel_repeat;
			}
			else {
				//5.��������
				sprintf_s(sqlBuf,"insert into t_user (name,tel,password,icon,feeling) values('%s','%s','%s',1,'nothing');",rq->name,rq->tel,rq->password);
				if (!m_sql.UpdateMySql(sqlBuf)) {
					cout << "����ʧ��" << sqlBuf << endl;
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
	//���
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)data;
	//�жϺϷ���

	//���ݵ绰������
	STRU_TCP_LOGIN_RS rs;
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)){
		cout<<"��ѯʧ��"<<sqlBuf<<endl;
		return;
	}
	else {
		//�绰����Ϊ��
		if (lst.size()==0) {
			rs.result = login_tel_not_exist;
		}
		else {
			lst.pop_front();
			//���ݵ绰�Ų�����
			sprintf_s(sqlBuf, "select password,id from t_user where tel = '%s';", rq->tel);
			if (!m_sql.SelectMySql(sqlBuf, 2, lst)) {
				cout << "��ѯʧ��" << sqlBuf << endl;
				return;
			}
			else {
				string psd = rq->password;
				string temp = lst.front();
				lst.pop_front();
				//��ȡ�û�id
				int userId = stoi(lst.front());
				lst.pop_front();

				
				if (psd != temp) {
					rs.result = login_password_error;
				}
				else {
					rs.result = login_success;
					//�ɹ��󱣴�id
					m_mapIdToSocket[userId] = from;
					//�ظ���Ϣ��id
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
	//1.���
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)data;
	//2.�������Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId)>0) {
		//���ߣ����͸���һ������
		m_INetMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//�����ߣ��ظ�ʧ��
		STRU_TCP_CHAT_RS rs;
		rs.result = send_failed;
		rs.friendId = rq->friendId;
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}

void CKernel::dealGroupChatRq(char* data, int len, long from)
{
	//1.���
	STRU_TCP_GROUP_CHAT_RQ* rq = (STRU_TCP_GROUP_CHAT_RQ*)data;
	//2.����ѯȺ��Ա�б�
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", rq->groupId);
	if (!m_sql.SelectMySql(sqlBuf, 1, lst)) {
		cout << "��ѯʧ��" << sqlBuf << endl;
		return;
	}
	while (lst.size() > 0) {
		//��ȡ�û�id
		int userId = stoi(lst.front());
		lst.pop_front();
		if (m_mapIdToSocket.count(userId) > 0) {
			//���߷��͸�Ⱥ�ĳ�Ա
			m_INetMediator->sendData(data, len, m_mapIdToSocket[userId]);
		}
	}
}

void CKernel::dealOfflineRq(char* data, int len, long from)
{
	//���
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)data;
	//��ȡ�����б�
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", rq->userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	int friendId = 0;
	while (idList.size() > 0) {
		//ȡ��id
		friendId = stoi(idList.front());
		idList.pop_front();
		//����id��ѯ�Ƿ�����
		if (m_mapIdToSocket.find(friendId)!=m_mapIdToSocket.end()) {
		//���߷�����������
			m_INetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}
	//������Դ
	auto ite = m_mapIdToSocket.find(rq->userId);
	if (ite != m_mapIdToSocket.end()) {
		closesocket(from);
		m_mapIdToSocket.erase(ite);
	}
}

void CKernel::dealAddFriendRq(char* data, int len, long from)
{
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)data;
	//�ж��Ƿ��к��������
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	//�жϲ�ѯ���
	STRU_TCP_ADD_FRIEND_RS rs;
	if (0 == List.size()) {
		//û�и���
		//���ý��
 		rs.result = add_friend_no_this_user;
		rs.userId = rq->userId;
		strcpy_s(rs.friendName, rq->friendName);
		//�ظ���ԭ������
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//�и���
		//ȡ������id
		int friendId = stoi(List.front());
		List.pop_front();
		//���Ѵ����ж��Ƿ�����
		if (m_mapIdToSocket.count(friendId) > 0) {
			//ת������һ������
			m_INetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//ʧ�ܲ�����
			rs.result = add_friend_user_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.friendName, rq->friendName);
			//�ظ���ԭ������
			m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}

}

void CKernel::dealAddFriendRs(char* data, int len, long from)
{
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)data;
	//�ж���ӽ���Ƿ�ɹ�
	if (add_friend_success == rs->result) {
		//�Ѻ��ѹ�ϵд�����ݿ⣬д���Σ�������˫��洢
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "insert into t_friend values (%d,%d);", rs->friendId,rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "��Ӻ�����Ϣʧ��:" << sqlBuf << endl;
		}
		sprintf_s(sqlBuf, "insert into t_friend values (%d,%d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "��Ӻ�����Ϣʧ��:" << sqlBuf << endl;
		}
		//4.���º����б�˫��ķ��ͣ�ֻ��һ�ξ��� ˭��id����ν
		sendUserInfo(rs->userId);
	}
	//5.�ѼӺ��ѽ����Ϣ���͸� ������
	m_INetMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
}

void CKernel::dealDeleteFriendRq(char* data, int len, long from)
{
	STRU_TCP_DELETE_FRIEND_RQ* rq = (STRU_TCP_DELETE_FRIEND_RQ*)data;
	//ֱ�������ݿ���ɾ�����ѹ�ϵ,ɾ������
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "delete from t_friend where idA = %d and idB = %d;",rq->friendId,rq->userId);
	if (!m_sql.UpdateMySql(sqlBuf)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	
	sprintf_s(sqlBuf, "delete from t_friend where idA = %d and idB = %d;", rq->userId, rq->friendId);
	if (!m_sql.UpdateMySql(sqlBuf)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	//�ظ�
	STRU_TCP_DELETE_FRIEND_RS rs;
	rs.friendId = rq->friendId;
	rs.userId = rq->userId;
	rs.result = _delete_success;
	//�ظ��� ɾ����
	m_INetMediator->sendData((char*)&rs, sizeof(rs),from);
	//�����ɾ�������� ��ô�ظ��� ��ɾ����
	if (m_mapIdToSocket.count(rq->friendId)) {
		m_INetMediator->sendData((char*)&rs, sizeof(rs), m_mapIdToSocket[rq->friendId]);
	}
	
}

void CKernel::dealCreateGroupRq(char* data, int len, long from)
{
	//���
	STRU_TCP_CREATE_GROUP_RQ* rq = (STRU_TCP_CREATE_GROUP_RQ*)data;
	//��ѯ�Ƿ���ڸ�Ⱥ��
	STRU_TCP_CREATE_GROUP_RS rs;
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_user where name = '%s';", rq->groupName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}

	if (List.size() > 0) {
		rs.result = _create_group_name_repeat;
	}
	else {
		rs.result = _create_group_success;
		//д�뵽���ݿ�
		sprintf_s(sqlBuf, "insert into t_group_name (name) values('%s');", rq->groupName);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "д��ʧ��:" << sqlBuf << endl;
		}
		sprintf_s(sqlBuf, "insert into t_group_user (groupId,userId,isLeader) values((select groupId from t_group_name where name = '%s'),%d,'yes');",rq->groupName, rq->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "д��ʧ��:" << sqlBuf << endl;
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
	//���
	STRU_TCP_ADD_GROUP_RQ* rq = (STRU_TCP_ADD_GROUP_RQ*)data;
	list<string> List;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_name where name = '%s';", rq->groupName);
	if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	//�жϲ�ѯ���
	STRU_TCP_ADD_GROUP_RS rs;
	if (0 == List.size()) {
		//û�и�Ⱥ
		//���ý��
		rs.result = add_group_no_this_group;
		rs.userId = rq->userId;
		strcpy_s(rs.groupName, rq->groupName);
		//�ظ���ԭ������
		m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//�и�Ⱥ
		//ȡ��Ⱥid
		int groupId = stoi(List.front());
		List.pop_front();
		//����Ⱥ��
		sprintf_s(sqlBuf, "select userId from t_group_user where groupId = %d and isLeader = 'yes';", groupId);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
		}
		//ȡ��Ⱥ��id
		int leaderId = stoi(List.front());
		List.pop_front();
		//Ⱥ�����ж�Ⱥ���Ƿ�����
		if (m_mapIdToSocket.count(leaderId) > 0) {
			//ת������һ��Ⱥ��
			m_INetMediator->sendData(data, len, m_mapIdToSocket[leaderId]);
		}
		else {
			//ʧ�ܲ�����
			rs.result = add_group_leader_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.groupName, rq->groupName);
			//�ظ���ԭ������
			m_INetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}

}

void CKernel::dealAddGroupRs(char* data, int len, long from)
{
	STRU_TCP_ADD_GROUP_RS* rs = (STRU_TCP_ADD_GROUP_RS*)data;
	//�ж���ӽ���Ƿ�ɹ�
	if (add_group_success == rs->result) {
		//�ѹ�ϵд�����ݿ⣬������˫��洢
		//����Ⱥid
		list<string> List;
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "select groupId from t_group_name where name = '%s';", rs->groupName);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
		}
		//ȡ��Ⱥid
		int groupId = stoi(List.front());
		List.pop_front();

		sprintf_s(sqlBuf, "insert into t_group_user (groupId,userId) values (%d,%d);", groupId, rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf)) {
			cout << "��Ӻ�����Ϣʧ��:" << sqlBuf << endl;
		}
		//4.����Ⱥ֪ͨ�����˸���Ⱥ
		sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", groupId);
		if (!m_sql.SelectMySql(sqlBuf, 1, List)) {
			cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
		}
		while (List.size() > 0) {
			int userId = stoi(List.front());
			List.pop_front();
			sendGroupInfo(userId);
		}
		
	}
	//5.�ѼӺ��ѽ����Ϣ���͸� ������
	m_INetMediator->sendData(data, len, m_mapIdToSocket[rs->userId]);
}

void CKernel::sendUserInfo(int userId)
{
	cout << __FUNCTION__ << endl;
	//1.��ѯ�Լ���Ϣ
	STRU_TCP_FRIEND_INFO userInfo;
	getUserInfo(userId, &userInfo);
	//2.���Լ���Ϣ���͸������
	if (m_mapIdToSocket.count(userId)) {
		m_INetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}//3.��ѯ������Ϣ�б��id
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "��ѯ������Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	//4.����id��ѯ��Ϣ�����͸������
	int friendId = 0;
	STRU_TCP_FRIEND_INFO friendInfo;
	while (idList.size() > 0) {
		//ȡ��id
		friendId = stoi(idList.front());
		idList.pop_front();
		//����id��ѯ
		getUserInfo(friendId, &friendInfo);
		//�Ѻ�����Ϣ���͸��ͻ���
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
		}
		//���Լ����߷��͸�����
		if (m_mapIdToSocket.count(friendId)) {
			m_INetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

void CKernel::sendGroupInfo(int userId)
{
	//1.��ѯȺ��Ϣ�б��id
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select groupId from t_group_user where userId = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "��ѯȺ����Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	//2.����Ⱥ����Ϣ���͸��ͻ���

	int groupId = 0;
	STRU_TCP_GROUP_INFO groupInfo;
	while (idList.size() > 0) {
		//ȡ��groupid
		groupId = stoi(idList.front());
		idList.pop_front();
		//����id��ѯȺ��
		getGroupInfo(groupId, &groupInfo);
		//��Ⱥ����Ϣ���͸��ͻ���
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&groupInfo, sizeof(groupInfo), m_mapIdToSocket[userId]);
		}
		//����Ⱥ��Ա��Ϣ
		sendGroupMember(userId, groupId);
	}
}

void CKernel::sendGroupMember(int userId,int groupId)
{
	//ȡ����Ⱥ���г�Աid�б�
	list<string> idList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select userId from t_group_user where groupId = '%d';", groupId);
	if (!m_sql.SelectMySql(sqlBuf, 1, idList)) {
		cout << "��ѯȺ����Ϣ�б�ʧ��:" << sqlBuf << endl;
	}
	int memberId = 0;
	STRU_TCP_MEMBER_INFO memberInfo;
	while (idList.size() > 0) {
		memberId = stoi(idList.front());
		idList.pop_front();
		//ȡ��Ⱥ��Ա��Ϣ
		getMemberInfo(groupId,memberId, &memberInfo);
		//��Ⱥ��Ա��Ϣ���͸���½��
		if (m_mapIdToSocket.count(userId)) {
			m_INetMediator->sendData((char*)&memberInfo, sizeof(memberInfo), m_mapIdToSocket[userId]);
		}
	}
}
void CKernel::getMemberInfo(int groupId, int userId, STRU_TCP_MEMBER_INFO* rs)
{
	//�ж��������
	rs->friendId = userId;
	if (m_mapIdToSocket.count(userId)) {
		rs->status = _status_online;
	}
	else {
		rs->status = _status_offline;
	}
	//�����ݿ����ѯ
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 3, lst)) {
		cout << "��ѯʧ��:" << sqlBuf << endl;
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
	//�ж��������
	rs->friendId = userId;
	if (m_mapIdToSocket.count(userId)) {
		rs->status = _status_online;
	}
	else {
		rs->status = _status_offline;
	}
	//�����ݿ����ѯ
	list<string> lst;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id = '%d';", userId);
	if (!m_sql.SelectMySql(sqlBuf, 3, lst)) {
		cout << "��ѯʧ��:" << sqlBuf << endl;
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
		cout << "��ѯʧ��:" << sqlBuf << endl;
	}

	if (2 == lst.size()) {
		strcpy(rs->groupName, lst.front().c_str());
		lst.pop_front();

		rs->iconId = stoi(lst.front());
		lst.pop_front();
	}
}




