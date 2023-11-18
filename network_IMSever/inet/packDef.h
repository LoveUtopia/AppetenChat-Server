#pragma once
#define _DEF_TCP_PORT (7212)
#define _DEF_SEVER_IP ("127.0.0.1")
#define _DEF_NAME_MAX (100)
#define _DEF_CONTENT_LENGTH (4096)
// 协议头
#define _DEF_PROTOCOL_BASE (1000)
#define _DEF_PROTOCOL_COUNT (19)
// 注册请求
#define _DEF_TCP_REGISTER_RQ (_DEF_PROTOCOL_BASE +1)
// 注册回复
#define _DEF_TCP_REGISTER_RS (_DEF_PROTOCOL_BASE +2)
// 登录请求
#define _DEF_TCP_LOGIN_RQ (_DEF_PROTOCOL_BASE +3)
// 登录回复
#define _DEF_TCP_LOGIN_RS (_DEF_PROTOCOL_BASE +4)
// 聊天请求
#define _DEF_TCP_CHAT_RQ (_DEF_PROTOCOL_BASE +5)
// 聊天回复
#define _DEF_TCP_CHAT_RS (_DEF_PROTOCOL_BASE +6)
// 添加好友请求
#define _DEF_TCP_ADD_FRIEND_RQ (_DEF_PROTOCOL_BASE +7)
// 添加好友回复
#define _DEF_TCP_ADD_FRIEND_RS (_DEF_PROTOCOL_BASE +8)
// 下线请求
#define _DEF_TCP_OFFLINE_RQ (_DEF_PROTOCOL_BASE +9)
// 好友信息
#define _DEF_TCP_FRIEND_INFO (_DEF_PROTOCOL_BASE +10)
// 删除好友请求
#define _DEF_TCP_DELETE_FRIEND_RQ (_DEF_PROTOCOL_BASE +11)
// 删除好友回复
#define _DEF_TCP_DELETE_FRIEND_RS (_DEF_PROTOCOL_BASE +12)
// 创建群聊请求
#define _DEF_TCP_CREATE_GROUP_RQ (_DEF_PROTOCOL_BASE +13)
// 创建群聊回复
#define _DEF_TCP_CREATE_GROUP_RS (_DEF_PROTOCOL_BASE +14)
// 群组信息
#define _DEF_TCP_GROUP_INFO (_DEF_PROTOCOL_BASE +15)
//群聊天请求
#define _DEF_TCP_GROUP_CHAT_RQ (_DEF_PROTOCOL_BASE +16)
//添加群聊请求
#define _DEF_TCP_ADD_GROUP_RQ (_DEF_PROTOCOL_BASE +17)
//添加群聊回复
#define _DEF_TCP_ADD_GROUP_RS (_DEF_PROTOCOL_BASE +18)
//群成员信息
#define _DEF_TCP_MEMBER_INFO (_DEF_PROTOCOL_BASE +19)
//注册结果
#define register_success        (0)
#define register_name_repeat    (1)
#define register_tel_repeat     (2)
//登录结果
#define login_success           (0)
#define login_tel_not_exist     (1)
#define login_password_error    (2)
//聊天结果
#define send_success            (0)
#define send_failed             (1)
//添加好友结果 为了简单对方不在线，发不了请求。现实是把请求放到数据库，等他上线再发送
#define add_friend_success      (0)
#define add_friend_no_this_user (1)
#define add_friend_user_refuse  (2)
#define add_friend_user_offline (3)
//好友状态
#define _status_online          (0)
#define _status_offline         (1)
//删除好友结果
#define _delete_success         (0)
#define _delete_failed          (1)
//创建群聊结果回复
#define _create_group_success       (0)
#define _create_group_name_repeat   (1)
//加入群聊回复
#define add_group_success       (0)
#define add_group_no_this_group (1)
#define add_group_leader_refuse  (2)
#define add_group_leader_offline (3)
// 重新定义协议头变量
typedef int packType;
// 注册请求
typedef struct STRU_TCP_REGISTER_RQ {
    STRU_TCP_REGISTER_RQ() :type(_DEF_TCP_REGISTER_RQ) {
        memset(name, 0, _DEF_NAME_MAX);//全部设置为0初始化
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //昵称 长度固定用char不用string
    char name[_DEF_NAME_MAX];
    //手机号
    char tel[_DEF_NAME_MAX];
    //密码
    char password[_DEF_NAME_MAX];
}STRU_TCP_REGISTER_RQ;
// 注册回复
typedef struct STRU_TCP_REGISTER_RS {
    STRU_TCP_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS), result(register_name_repeat) {

    }
    //类型
    packType type;
    //结果
    int result;
}STRU_TCP_REGISTER_RS;
// 登录请求
typedef struct STRU_TCP_LOGIN_RQ {
    STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ) {
        memset(tel, 0, _DEF_NAME_MAX);
        memset(password, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //手机号
    char tel[_DEF_NAME_MAX];
    //密码
    char password[_DEF_NAME_MAX];
}STRU_TCP_LOGIN_RQ;
// 登录回复
typedef struct STRU_TCP_LOGIN_RS {
    STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(login_password_error), id(0) {

    }
    //类型
    packType type;
    //
    int result;
    //用户id
    int id;
}STRU_TCP_LOGIN_RS;
// 聊天请求
typedef struct STRU_TCP_CHAT_RQ {
    STRU_TCP_CHAT_RQ() :type(_DEF_TCP_CHAT_RQ), userId(0), friendId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
    }
    //类型
    packType type;
    //聊天内容
    char content[_DEF_CONTENT_LENGTH];
    //我是谁
    int userId;
    //对方id
    int friendId;
}STRU_TCP_CHAT_RQ;
// 聊天回复
typedef struct STRU_TCP_CHAT_RS {
    STRU_TCP_CHAT_RS() :type(_DEF_TCP_CHAT_RS), result(send_failed) {
    }
    //类型
    packType type;
    //结果
    int result;
    //朋友id
    int friendId;
}STRU_TCP_CHAT_RS;
// 添加好友请求
typedef struct STRU_TCP_ADD_FRIEND_RQ {
    STRU_TCP_ADD_FRIEND_RQ() :type(_DEF_TCP_ADD_FRIEND_RQ), userId(0) {
        memset(userName, 0, _DEF_NAME_MAX);
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //用户昵称
    char userName[_DEF_NAME_MAX];
    //用户id
    int userId;
    //对方昵称
    char friendName[_DEF_NAME_MAX];
}STRU_TCP_ADD_FRIEND_RQ;
// 添加好友回复
typedef struct STRU_TCP_ADD_FRIEND_RS {
    STRU_TCP_ADD_FRIEND_RS() :type(_DEF_TCP_ADD_FRIEND_RS), result(add_friend_no_this_user) {
        memset(friendName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //对方昵称
    char friendName[_DEF_NAME_MAX];
    //结果
    int result;
    //自己的id
    int userId;
    //好友的id
    int friendId;
}STRU_TCP_ADD_FRIEND_RS;
// 下线请求
typedef struct STRU_TCP_OFFLINE_RQ {
    STRU_TCP_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE_RQ), userId(0) {

    }
    //类型
    packType type;
    //用户id
    int userId;
}STRU_TCP_OFFLINE_RQ;
//好友信息
typedef struct STRU_TCP_FRIEND_INFO {
    STRU_TCP_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), userId(0), status(0), friendId(0) {
        memset(name, 0, _DEF_NAME_MAX);
        memset(feeling, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //用户id
    int userId;
    //用户头像
    int iconId;
    //用户昵称
    char name[_DEF_NAME_MAX];
    //个性标签
    char feeling[_DEF_NAME_MAX];
    //状态
    int status;
    //对端id
    int friendId;
}STRU_TCP_FRIEND_INFO;

typedef struct STRU_TCP_DELETE_FRIEND_RQ {
    STRU_TCP_DELETE_FRIEND_RQ() :type(_DEF_TCP_DELETE_FRIEND_RQ), userId(0), friendId(0) {

    }
    //类型
    packType type;
    //用户id
    int userId;
    //好友id
    int friendId;
}STRU_TCP_DELETE_FRIEND_RQ;

typedef struct STRU_TCP_DELETE_FRIEND_RS {
    STRU_TCP_DELETE_FRIEND_RS() :type(_DEF_TCP_DELETE_FRIEND_RS), userId(0), friendId(0), result(0) {

    }
    //类型
    packType type;
    int userId;

    int friendId;
    //删除结果
    int result;
}STRU_TCP_DELETE_FRIEND_RS;

typedef struct STRU_TCP_CREATE_GROUP_RQ {
    STRU_TCP_CREATE_GROUP_RQ() :type(_DEF_TCP_CREATE_GROUP_RQ), userId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //用户id
    int userId;
    //群名称
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_CREATE_GROUP_RQ;

typedef struct STRU_TCP_CREATE_GROUP_RS {
    STRU_TCP_CREATE_GROUP_RS() :type(_DEF_TCP_CREATE_GROUP_RS), result(1) {
    }
    //类型
    packType type;
    //创建结果
    int result;
}STRU_TCP_CREATE_GROUP_RS;

typedef struct STRU_TCP_GROUP_INFO {
    STRU_TCP_GROUP_INFO() :type(_DEF_TCP_GROUP_INFO), groupId(0), iconId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //群名称
    char groupName[_DEF_NAME_MAX];
    //群id
    int groupId;
    //群iconId
    int iconId;
}STRU_TCP_GROUP_INFO;

typedef struct STRU_TCP_GROUP_CHAT_RQ {
    STRU_TCP_GROUP_CHAT_RQ() :type(_DEF_TCP_GROUP_CHAT_RQ), groupId(0), userId(0) {
        memset(content, 0, _DEF_CONTENT_LENGTH);
        memset(name, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //群id
    int groupId;
    //内容
    char content[_DEF_CONTENT_LENGTH];
    int userId;
    //用户名称
    char name[_DEF_NAME_MAX];
}STRU_TCP_GROUP_CHAT_RQ;

typedef struct STRU_TCP_ADD_GROUP_RQ {
    STRU_TCP_ADD_GROUP_RQ() :type(_DEF_TCP_ADD_GROUP_RQ), userId(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
        memset(userName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //用户
    int userId;
    //用户昵称
    char userName[_DEF_NAME_MAX];
    //群名称
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_ADD_GROUP_RQ;

typedef struct STRU_TCP_ADD_GROUP_RS {
    STRU_TCP_ADD_GROUP_RS() :type(_DEF_TCP_ADD_GROUP_RS), userId(0), groupId(0), result(0) {
        memset(groupName, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //申请人id
    int userId;
    //群id
    int groupId;
    //结果
    int result;
    //群名称
    char groupName[_DEF_NAME_MAX];
}STRU_TCP_ADD_GROUP_RS;

typedef struct STRU_TCP_MEMBER_INFO {
    STRU_TCP_MEMBER_INFO() :type(_DEF_TCP_MEMBER_INFO), userId(0), status(0), friendId(0), groupId(0){
        memset(name, 0, _DEF_NAME_MAX);
        memset(feeling, 0, _DEF_NAME_MAX);
    }
    //类型
    packType type;
    //群id
    int groupId;
    //用户id
    int userId;
    //用户头像
    int iconId;
    //用户昵称
    char name[_DEF_NAME_MAX];
    //个性标签
    char feeling[_DEF_NAME_MAX];
    //状态
    int status;
    //对端id
    int friendId;
}STRU_TCP_MEMBER_INFO;