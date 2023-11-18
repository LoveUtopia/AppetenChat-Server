#pragma once
#define _DEF_TCP_PORT (7212)
#define _DEF_SEVER_IP ("127.0.0.1")
#define _DEF_NAME_MAX (100)
#define _DEF_CONTENT_LENGTH (4096)
// Э��ͷ
#define _DEF_PROTOCOL_BASE (1000)
#define _DEF_PROTOCOL_COUNT (19)
// ע������
#define _DEF_TCP_REGISTER_RQ (_DEF_PROTOCOL_BASE +1)
// ע��ظ�
#define _DEF_TCP_REGISTER_RS (_DEF_PROTOCOL_BASE +2)
// ��¼����
#define _DEF_TCP_LOGIN_RQ (_DEF_PROTOCOL_BASE +3)
// ��¼�ظ�
#define _DEF_TCP_LOGIN_RS (_DEF_PROTOCOL_BASE +4)
// ��������
#define _DEF_TCP_CHAT_RQ (_DEF_PROTOCOL_BASE +5)
// ����ظ�
#define _DEF_TCP_CHAT_RS (_DEF_PROTOCOL_BASE +6)
// ��Ӻ�������
#define _DEF_TCP_ADD_FRIEND_RQ (_DEF_PROTOCOL_BASE +7)
// ��Ӻ��ѻظ�
#define _DEF_TCP_ADD_FRIEND_RS (_DEF_PROTOCOL_BASE +8)
// ��������
#define _DEF_TCP_OFFLINE_RQ (_DEF_PROTOCOL_BASE +9)
// ������Ϣ
#define _DEF_TCP_FRIEND_INFO (_DEF_PROTOCOL_BASE +10)
// ɾ����������
#define _DEF_TCP_DELETE_FRIEND_RQ (_DEF_PROTOCOL_BASE +11)
// ɾ�����ѻظ�
#define _DEF_TCP_DELETE_FRIEND_RS (_DEF_PROTOCOL_BASE +12)
// ����Ⱥ������
#define _DEF_TCP_CREATE_GROUP_RQ (_DEF_PROTOCOL_BASE +13)
// ����Ⱥ�Ļظ�
#define _DEF_TCP_CREATE_GROUP_RS (_DEF_PROTOCOL_BASE +14)
// Ⱥ����Ϣ
#define _DEF_TCP_GROUP_INFO (_DEF_PROTOCOL_BASE +15)
//Ⱥ��������
#define _DEF_TCP_GROUP_CHAT_RQ (_DEF_PROTOCOL_BASE +16)
//���Ⱥ������
#define _DEF_TCP_ADD_GROUP_RQ (_DEF_PROTOCOL_BASE +17)
//���Ⱥ�Ļظ�
#define _DEF_TCP_ADD_GROUP_RS (_DEF_PROTOCOL_BASE +18)
//Ⱥ��Ա��Ϣ
#define _DEF_TCP_MEMBER_INFO (_DEF_PROTOCOL_BASE +19)
//ע����
#define register_success        (0)
#define register_name_repeat    (1)
#define register_tel_repeat     (2)
//��¼���
#define login_success           (0)
#define login_tel_not_exist     (1)
#define login_password_error    (2)
//������
#define send_success            (0)
#define send_failed             (1)
//��Ӻ��ѽ�� Ϊ�˼򵥶Է������ߣ�������������ʵ�ǰ�����ŵ����ݿ⣬���������ٷ���
#define add_friend_success      (0)
#define add_friend_no_this_user (1)
#define add_friend_user_refuse  (2)
#define add_friend_user_offline (3)
//����״̬
#define _status_online          (0)
#define _status_offline         (1)
//ɾ�����ѽ��
#define _delete_success         (0)
#define _delete_failed          (1)
//����Ⱥ�Ľ���ظ�
#define _create_group_success       (0)
#define _create_group_name_repeat   (1)
//����Ⱥ�Ļظ�
#define add_group_success       (0)
#define add_group_no_this_group (1)
#define add_group_leader_refuse  (2)
#define add_group_leader_offline (3)
// ���¶���Э��ͷ����
typedef int packType;
// ע������
typedef struct STRU_TCP_REGISTER_RQ {
    STRU_TCP_REGISTER_RQ() :type(_DEF_TCP_REGISTER_RQ) {
        memset(name, 0, _DEF_NAME_MAX);//ȫ������Ϊ0��ʼ��
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�ǳ� ���ȹ̶���char����string
    char name[_DEF_NAME_MAX];
    //�ֻ���
    char tel[_DEF_NAME_MAX];
    //����
    char password[_DEF_NAME_MAX];
}STRU_TCP_REGISTER_RQ;
// ע��ظ�
typedef struct STRU_TCP_REGISTER_RS {
    STRU_TCP_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS), result(register_name_repeat) {

    }
    //����
    packType type;
    //���
    int result;
}STRU_TCP_REGISTER_RS;
// ��¼����
typedef struct STRU_TCP_LOGIN_RQ {
    STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ) {
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�ֻ���
    char tel[_DEF_NAME_MAX];
    //����
    char password[_DEF_NAME_MAX];
}STRU_TCP_LOGIN_RQ;
// ��¼�ظ�
typedef struct STRU_TCP_LOGIN_RS {
    STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(login_password_error), id(0) {

    }
    //����
    packType type;
    //
    int result;
    //�û�id
    int id;
}STRU_TCP_LOGIN_RS;
// ��������
typedef struct STRU_TCP_CHAT_RQ {
    STRU_TCP_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }
    //����
    packType type;
    //��������
    char content[_DEF_CONTENT_LENGTH];
    //����˭
    int userId;
    //�Է�id
    int friendId;
}STRU_TCP_CHAT_RQ;
// ����ظ�
typedef struct STRU_TCP_CHAT_RS {
    STRU_TCP_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_failed) {
    }
    //����
    packType type;
    //���
    int result;
    //����id
    int friendId;
}STRU_TCP_CHAT_RS;
// ��Ӻ�������
typedef struct STRU_TCP_ADD_FRIEND_RQ {
    STRU_TCP_ADD_FRIEND_RQ() :type(_DEF_TCP_ADD_FRIEND_RQ), userId(0) {
        memset(userName, 0, _DEF_NAME_MAX);
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�û��ǳ�
    char userName[_DEF_NAME_MAX];
    //�û�id
    int userId;
    //�Է��ǳ�
    char friendName[_DEF_NAME_MAX];
}STRU_TCP_ADD_FRIEND_RQ;
// ��Ӻ��ѻظ�
typedef struct STRU_TCP_ADD_FRIEND_RS {
    STRU_TCP_ADD_FRIEND_RS() :type(_DEF_TCP_ADD_FRIEND_RS), result(add_friend_no_this_user) {
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�Է��ǳ�
    char friendName[_DEF_NAME_MAX];
    //���
    int result;
    //�Լ���id
    int userId;
    //���ѵ�id
    int friendId;
}STRU_TCP_ADD_FRIEND_RS;
// ��������
typedef struct STRU_TCP_OFFLINE_RQ {
    STRU_TCP_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0) {

    }
    //����
    packType type;
    //�û�id
    int userId;
}STRU_TCP_OFFLINE_RQ;
//������Ϣ
typedef struct STRU_TCP_FRIEND_INFO {
    STRU_TCP_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), userId(0), status(0), friendId(0) {
        memset(name, 0, _DEF_NAME_MAX);
        memset(feeling, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�û�id
    int userId;
    //�û�ͷ��
    int iconId;
    //�û��ǳ�
    char name[_DEF_NAME_MAX];
    //���Ա�ǩ
    char feeling[_DEF_NAME_MAX];
    //״̬
    int status;
    //�Զ�id
    int friendId;
}STRU_TCP_FRIEND_INFO;

typedef struct STRU_TCP_DELETE_FRIEND_RQ {
    STRU_TCP_DELETE_FRIEND_RQ() :type(_DEF_TCP_DELETE_FRIEND_RQ), userId(0), friendId(0) {

    }
    //����
    packType type;
    //�û�id
    int userId;
    //����id
    int friendId;
}STRU_TCP_DELETE_FRIEND_RQ;

typedef struct STRU_TCP_DELETE_FRIEND_RS {
    STRU_TCP_DELETE_FRIEND_RS() :type(_DEF_TCP_DELETE_FRIEND_RS), userId(0), friendId(0), result(0) {

    }
    //����
    packType type;
    int userId;

    int friendId;
    //ɾ�����
    int result;
}STRU_TCP_DELETE_FRIEND_RS;

typedef struct STRU_TCP_CREATE_GROUP_RQ {
    STRU_TCP_CREATE_GROUP_RQ() :type(_DEF_TCP_CREATE_GROUP_RQ), userId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�û�id
    int userId;
    //Ⱥ����
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_CREATE_GROUP_RQ;

typedef struct STRU_TCP_CREATE_GROUP_RS {
    STRU_TCP_CREATE_GROUP_RS() :type(_DEF_TCP_CREATE_GROUP_RS), result(1) {
    }
    //����
    packType type;
    //�������
    int result;
}STRU_TCP_CREATE_GROUP_RS;

typedef struct STRU_TCP_GROUP_INFO {
    STRU_TCP_GROUP_INFO() :type(_DEF_TCP_GROUP_INFO), groupId(0), iconId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //Ⱥ����
    char groupName[_DEF_NAME_MAX];
    //Ⱥid
    int groupId;
    //ȺiconId
    int iconId;
}STRU_TCP_GROUP_INFO;

typedef struct STRU_TCP_GROUP_CHAT_RQ {
    STRU_TCP_GROUP_CHAT_RQ() :type(_DEF_TCP_GROUP_CHAT_RQ), groupId(0), userId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
        memset(name, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //Ⱥid
    int groupId;
    //����
    char content[_DEF_CONTENT_LENGTH];
    int userId;
    //�û�����
    char name[_DEF_NAME_MAX];
}STRU_TCP_GROUP_CHAT_RQ;

typedef struct STRU_TCP_ADD_GROUP_RQ {
    STRU_TCP_ADD_GROUP_RQ() :type(_DEF_TCP_ADD_GROUP_RQ), userId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
        memset(userName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //�û�
    int userId;
    //�û��ǳ�
    char userName[_DEF_NAME_MAX];
    //Ⱥ����
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_ADD_GROUP_RQ;

typedef struct STRU_TCP_ADD_GROUP_RS {
    STRU_TCP_ADD_GROUP_RS() :type(_DEF_TCP_ADD_GROUP_RS), userId(0), groupId(0), result(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //������id
    int userId;
    //Ⱥid
    int groupId;
    //���
    int result;
    //Ⱥ����
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_ADD_GROUP_RS;

typedef struct STRU_TCP_MEMBER_INFO {
    STRU_TCP_MEMBER_INFO() :type(_DEF_TCP_MEMBER_INFO), userId(0), status(0), friendId(0), groupId(0){
        memset(name, 0, _DEF_NAME_MAX);
        memset(feeling, 0, _DEF_NAME_MAX);
    }
    //����
    packType type;
    //Ⱥid
    int groupId;
    //�û�id
    int userId;
    //�û�ͷ��
    int iconId;
    //�û��ǳ�
    char name[_DEF_NAME_MAX];
    //���Ա�ǩ
    char feeling[_DEF_NAME_MAX];
    //״̬
    int status;
    //�Զ�id
    int friendId;
}STRU_TCP_MEMBER_INFO;