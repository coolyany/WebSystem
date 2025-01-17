﻿#include "OperateDB.h"
#include <QDebug>
#include <QMessageBox>

OperateDB *OperateDB::instance = nullptr;

OperateDB::OperateDB(QObject* parent)
	: QObject(parent)
{
	// 连接数据库
	m_db = QSqlDatabase::addDatabase("QSQLITE"); // 连接的数据库类型,这里选择sqlite
	init();
}

OperateDB::~OperateDB()
{
	m_db.close(); // 关闭数据库连接
}

OperateDB* OperateDB::getInstance()
{
	// TODO: 在此处插入 return 语句
	//static OperateDB instance;
	if (instance == NULL)
	{
		instance = new OperateDB();
	}
	return instance;
}

bool OperateDB::UserRegister(const char* username, const char* password)
{
	if (username == NULL || password == NULL)
	{
		return false;
	}

	char insert_userinfo_sql[128] = { '\0' };
	sprintf(insert_userinfo_sql, "INSERT INTO userInfo (name, password) VALUES ('%s','%s')", username, password);

	printf("sql query :: %s\n", insert_userinfo_sql);

	QSqlQuery sql_query;
	return sql_query.exec(insert_userinfo_sql);

}

bool OperateDB::UserLogin(const char * username, const char * password)
{
	if (username == NULL || password == NULL)
	{
		return false;
	}

	char query_userinfo_sql[128] = { '\0' };
	sprintf(query_userinfo_sql, "SELECT * FROM userInfo WHERE name='%s' and password='%s' and online=0", username, password);

	printf("sql query :: %s\n", query_userinfo_sql);

	QSqlQuery sql_query;
	sql_query.exec(query_userinfo_sql);

	//移动到查询结果的下一条记录
	if (sql_query.next())
	{
		char update_userinfo_sql[128] = { '\0' };
		sprintf(update_userinfo_sql, "UPDATE userInfo SET online=1 WHERE name='%s' and password='%s'", username, password);

		return sql_query.exec(update_userinfo_sql);
	}
	else
	{
		return false;
	}
}

QStringList OperateDB::GetUserOnline()
{
	char online_userinfo_sql[128] = { '\0' };
	sprintf(online_userinfo_sql, "SELECT name FROM userInfo WHERE online=1");

	QSqlQuery sql_query;
	sql_query.exec(online_userinfo_sql);
	QStringList userNames;
	while (sql_query.next()) {
		userNames.append(sql_query.value(0).toString());
	}

	return userNames;
}

int OperateDB::SearchUser(const char * name)
{
	if (!name)
	{
		return -1;
	}

	char search_userinfo_sql[128] = { '\0' };
	sprintf(search_userinfo_sql, "SELECT online FROM userInfo WHERE name='%s'", name);

	QSqlQuery sql_query;
	sql_query.exec(search_userinfo_sql);
	//存在
	if (sql_query.next())
	{
		int status = sql_query.value(0).toInt();
		//不在线
		if (status == 0)
		{
			return 0;
		}
		//在线
		else if (status == 1)
		{
			return 1;
		}
	}
	//不存在
	else 
	{
		return -1;
	}
}

int OperateDB::AddUser(const char * friendName, const char * localName)
{
	if (!friendName | !localName) {
		printf("firendName or localName is NULL\n");
		return -1;
	}
	char add_userFriendInfo_sql[256] = { '\0' };
	sprintf(add_userFriendInfo_sql, "select * from userFriendInfo "
		"where (id=(select id from userInfo where name='%s') "
		"and friendId=(select id from userInfo where name='%s')) "
		"or (id=(select id from userInfo where name='%s') "
		"and friendId=(select id from userInfo where name='%s'))", friendName, localName, localName, friendName);
	printf("sql :: %s", add_userFriendInfo_sql);
	QSqlQuery query;
	query.exec(add_userFriendInfo_sql);
	//好友存在
	if (query.next()) {
		qDebug() << "add users value ::	" << query.value(0).toString() << "|" << query.value(1).toString();
		return 2;
	}
	//好友不存在
	else
	{
		char find_userInfo_sql[128] = { '\0' };
		sprintf(find_userInfo_sql, "SELECT online FROM userInfo WHERE name='%s'", friendName);
		printf("sql :: %s", find_userInfo_sql);
		//查询用户是否在线
		QSqlQuery sql_query;
		sql_query.exec(find_userInfo_sql);
		if (sql_query.next()) 
		{
			qDebug() << "friend online :: " << sql_query.value(0).toInt();
			return  sql_query.value(0).toInt();
		}
		else
		{
			return -1;
		}
	}


	return -1;
}

bool OperateDB::AgreeAddUser(const char* friendName, const char* localName)
{
	/*char sql_str[128] = { '\0' };
	sprintf(sql_str, "INSERT INTO userFriendInfo (id, friendId) VALUES ((select id from userInfo where name='%s')," 
		"(select id from userInfo where name='%s'))", localName, friendName);

	qDebug() << "sql :: " << sql_str;
	QSqlQuery sql_query;
	return sql_query.exec(sql_str);*/

	char select_id_userInfo_sql[64] = { '\0' };
	//char select_local_userInfo_sql[64] = { '\0' };
	char insert_userFriendInfo_sql[64] = { '\0' };
	sprintf(select_id_userInfo_sql, "SELECT id FROM userInfo WHERE (name='%s') or (name='%s')", localName, friendName);

	QSqlQuery sql_select;
	int re = sql_select.exec(select_id_userInfo_sql);
	if (!re)
	{
		return false;
	}
	int idBuf[2];
	int i = 0;
	while (sql_select.next())
	{
		idBuf[i] = sql_select.value(0).toInt();
		i++;
	}

	sprintf(insert_userFriendInfo_sql, "INSERT INTO userFriendInfo  (id,friendId) VALUES (%d, %d)", idBuf[0], idBuf[1]);

	qDebug() << "add friend sql :: " << insert_userFriendInfo_sql;
	QSqlQuery sql_insert;
	return sql_insert.exec(insert_userFriendInfo_sql);

}

QStringList OperateDB::GetFriendList(const char* name)
{
	QStringList friendList;
	friendList.clear();

	//在userFriendInfo表中找到所有好友的id
	QVector<int> friendIds;
	friendIds.clear();
	char sql_str[128] = { '\0' };
	sprintf(sql_str, "SELECT id FROM userFriendInfo WHERE friendId=(SELECT id FROM userInfo WHERE name='%s')", name);
	QSqlQuery sql;
	if (!sql.exec(sql_str)) {
		printf("select id from userFriendInfo error");
		return friendList;
	}
	while (sql.next())
	{
		friendIds.push_back(sql.value(0).toInt());
	}

	memset(sql_str, 0, 128);
	sql.clear();
	sprintf(sql_str, "SELECT friendId FROM userFriendInfo WHERE id=(SELECT id FROM userInfo WHERE name='%s')", name);
	if (!sql.exec(sql_str)) {
		printf("select id from userFriendInfo error");
		return friendList;
	}
	while (sql.next())
	{
		friendIds.push_back(sql.value(0).toInt());
	}

	//在userInfo表中找name
	for (int id : friendIds) 
	{
		memset(sql_str, 0, 128);
		sql.clear();
		sprintf(sql_str, "SELECT name,online FROM userInfo WHERE id=%d", id);
		if (!sql.exec(sql_str)) {
			printf("get friend name error");
			return friendList;
		}
		if (sql.next())
		{
			friendList.append(sql.value(0).toString() + "," + sql.value(1).toString());
		}
	}

	return friendList;
}

bool OperateDB::DelFriend(const char * friendName, const char * localName)
{
	if (!friendName || !localName)
	{
		printf("delete friend DB, firendName or localName is NULL\n");
		return false;
	}
	char del_userFriendInfo_sql[256] = { '\0' };
	sprintf(del_userFriendInfo_sql, "DELETE FROM userFriendInfo "
		"WHERE (id=(SELECT id FROM userInfo WHERE name='%s') "
		"AND friendId=(SELECT id FROM userInfo WHERE name='%s')) "
		"OR (id=(SELECT id FROM userInfo WHERE name='%s') "
		"AND friendId=(SELECT id FROM userInfo WHERE name='%s'))", friendName, localName, localName, friendName);
	printf("del friend sql str :: %s", del_userFriendInfo_sql);
	QSqlQuery query;
	return query.exec(del_userFriendInfo_sql);
}

void OperateDB::setOffline(const char * name)
{
	if (!name) {
		printf("name is NULL\n");
		return;
	}
	//登录状态设置为0，下线状态
	char offline_userinfo_sql[128] = { '\0' };
	sprintf(offline_userinfo_sql, "UPDATE userInfo SET online=0 WHERE name='%s'", name);

	printf("sql query :: %s\n", offline_userinfo_sql);

	QSqlQuery query;
	query.exec(offline_userinfo_sql);
}

bool OperateDB::connectSql(const QString & dbName)
{
	m_db.setHostName("localhost");//本地数据库
	m_db.setDatabaseName(dbName);
	//m_db.setDatabaseName("netdisksystem.db");
	m_db.setUserName("root");
	m_db.setPassword("123456");
	if (!m_db.open()) {
		QMessageBox::critical(0, QObject::tr("Database Error"),
			m_db.lastError().text());
		return false;
	}
	return true;
}

void OperateDB::init()
{
	//如果打开成功
	if (connectSql("netdisk.db"))
	{
		qDebug() << "open successful";

		QSqlQuery sql_query;

		

		//QSqlQuery query;
		bool re = sql_query.exec("select * from userInfo");
		//若不存在userInfo表，则创建一张
		if (!re)
		{
			//创建用户信息表,id为主键（自动增长）
			QString create_userinfo_sql = "CREATE TABLE userInfo("
				"id INTEGER PRIMARY KEY AUTOINCREMENT,"
				"name VARCHAR(30) UNIQUE,"// 不能重名,唯一
				"password VARCHAR(30),"
				"online INTEGER DEFAULT(0))";
			sql_query.prepare(create_userinfo_sql);
			if (!sql_query.exec())
			{
				qDebug() << sql_query.lastError();
			}
			else
			{
				qDebug() << "table created!";
			}
		}

		re = sql_query.exec("select * from userFriendInfo");
		//若不存在userInfo表，则创建一张
		if (!re)
		{
			//创建用户信息表,id为主键（自动增长）
			QString create_userFriendInfo_sql = "CREATE TABLE userFriendInfo("
				"id INTEGER,"
				"friendId INTEGER,"
				"PRIMARY KEY(id,friendId))";
			sql_query.prepare(create_userFriendInfo_sql);
			if (!sql_query.exec())
			{
				qDebug() << sql_query.lastError();
			}
			else
			{
				qDebug() << "table created!";
			}
		}
		
		//查询
		re = sql_query.exec("select * from userInfo");
		while (sql_query.next())
		{
			QString data = QString("%1, %2, %3, %4").arg(sql_query.value(0).toString()).arg(sql_query.value(1).toString())
				.arg(sql_query.value(2).toString()).arg(sql_query.value(3).toString());
			qDebug() << data;
		}

		re = sql_query.exec("select * from userFriendInfo");
		while (sql_query.next())
		{
			QString data = QString("%1, %2").arg(sql_query.value(0).toString()).arg(sql_query.value(1).toString());
			qDebug() << data;
		}
	}
}
