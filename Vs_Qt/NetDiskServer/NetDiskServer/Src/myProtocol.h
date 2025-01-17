﻿#pragma once
/* 自定义通信协议 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

#define REGISTER_OK						"register ok"
#define REGISTER_FAILED					"register failed, name is existed"
#define LOGIN_OK						"login ok"
#define LOGIN_FAILED					"login failed"
#define SEARCH_OK_ONLINE				"search successful, status is online"
#define SEARCH_OK_OFFLINE				"search successful, status is offline"
#define SEARCH_NO_RESULT				"search no result"
#define ADD_FRIEND_ERROR				"add friend error"
#define ADD_FRIEND_IS_EXIST				"add friend,friend is exist"
#define ADD_FRIEND_USER_ONLINE			"add friend,user is online"
#define ADD_FRIEND_USER_OFFLINE			"add friend,user is offline"
#define DEL_FRIEND_OK					"delete friend is successful"
#define DEL_FRIEND_FAILED				"delete friend is failed"

enum ENUM_MSG_TYPE {
	ENUM_MSG_TYPE_MIN = 0,			//初始值

	ENUM_MSG_TYPE_REGIST_REQUEST,	//注册请求
	ENUM_MSG_TYPE_REGIST_RESPONSE,  //注册回复
	ENUM_MSG_TYPE_LOGIN_REQUEST,  //登录请求
	ENUM_MSG_TYPE_LOGIN_RESPONSE,  //登录回复
	ENUM_MSG_TYPE_USER_ONLINE_REQUEST,  //用户在线请求
	ENUM_MSG_TYPE_USER_ONLINE_RESPONSE,  //用户在线回复
	ENUM_MSG_TYPE_SEARCH_USER_REQUEST,  //搜索用户回复
	ENUM_MSG_TYPE_SEARCH_USER_RESPONSE,  //搜索用户回复
	ENUM_MSG_TYPE_ADD_USER_REQUEST,  //添加用户请求
	ENUM_MSG_TYPE_ADD_USER_RESPONSE,  //添加用户回复
	ENUM_MSG_TYPE_AGREE_ADD_USER_RESPONSE,  //客户端同意添加用户回复
	ENUM_MSG_TYPE_REFRESH_FRIEND_LIST_REQUEST,  //刷新好友列表请求
	ENUM_MSG_TYPE_REFRESH_FRIEND_LIST_RESPONSE,  //刷新好友列表回复
	ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,  //删除好友请求
	ENUM_MSG_TYPE_DELETE_FRIEND_RESPONSE,  //删除好友回复

	ENUM_MSG_TYPE_MAX = 0x00ffffff	//最大值
};

typedef struct protocol_data_unit{
	uint PDULen;//整个PDU大小
	uint MsgType;//信息类型
	char caData[64];//固定64字节大小数据
	uint MsgLen;//信息大小
	int caMsg[];//动态信息内容

} PDU;

PDU* getPDU(uint uiMsgLen);//传入信息大小，分配PUD内存，得到结构体指针，