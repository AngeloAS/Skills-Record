#include <iostream>
#include <vector>
#include <mysql.h>
#include <string>
#include <atltime.h>

using namespace std;
// 待插入的数据的数据库、表的信息
struct dataBaseInfo{
	string servername; // 服务器名
	string username;   // 用户名
	string password;   // 密码
	string basename;   // 数据库名
	string tablename;  // 待插入的表名
	dataBaseInfo()
	{
		servername = "localhost"; // 服务器名 localhost是指本地
		username = "root";
		password = "123456";
		basename = "classroom";
		tablename = "CquptPersonCount";
	}
};

// 人数统计表的数据结构
struct personDataType{
	int classroomId;
	string time;
	int personCount;
	personDataType()
	{
		classroomId = 0;
		time = "";
		personCount = 0;
	}
};

// 姿态统计表的数据结构
struct poseDataType{
	int classroomId;
	string time;
	int standing;
	int sitting;
	int lying;
	poseDataType()
	{
		classroomId = 0;
		time = "";
		standing = 0;
		sitting = 0;
		lying = 0;
	}
};
class storeToSql
{
private:
	string getSqlcommand(string tablename, personDataType datatmp)
	{
		// string 类型进行SQL语句拼接
		string pre_str = "insert into ";
		string middle_str = " values('";
		string t = "', '";
		string str = std::to_string(datatmp.classroomId) + t + datatmp.time + t + std::to_string(datatmp.personCount);
		string end_str = "')";
		string sql_command = pre_str + tablename + middle_str + str + end_str;
		return sql_command;
	}
	string getSqlcommand(string tablename, poseDataType datatmp)
	{
		// string 类型进行SQL语句拼接
		string pre_str = "insert into ";
		string middle_str = " values('";
		string t = "', '";
		string str = std::to_string(datatmp.classroomId) + t + datatmp.time + t + std::to_string(datatmp.standing) + t + std::to_string(datatmp.sitting) + t + std::to_string(datatmp.lying);
		string end_str = "')";
		string sql_command = pre_str + tablename + middle_str + str + end_str;
		return sql_command;
	}
public:
	int insertData(dataBaseInfo dbInfo, personDataType datatmp)
	{
		MYSQL conn;
		int return_num;
		mysql_init(&conn);

		const char* servername = dbInfo.servername.data();
		const char* username = dbInfo.username.data();
		const char* password = dbInfo.password.data();
		const char* basename = dbInfo.basename.data();

		// 连接数据库，这里的几个参数severname， username，password，bassname，必须均是char*
		if (mysql_real_connect(&conn, servername, username, password, basename, 0, NULL, CLIENT_FOUND_ROWS))
		{
			cout << "connect database ok" << endl;
			// string 类型进行SQL语句拼接
			string sql_command = getSqlcommand(dbInfo.tablename, datatmp);
			const char* p = sql_command.data();
			return_num = mysql_query(&conn, p);
			if (return_num)
			{
				return 1;
			}
			mysql_close(&conn);
			return 0;
		}
	}
	int insertData(dataBaseInfo dbInfo, poseDataType datatmp)
	{
		MYSQL conn;
		int return_num;
		mysql_init(&conn);

		const char* servername = dbInfo.servername.data();
		const char* username = dbInfo.username.data();
		const char* password = dbInfo.password.data();
		const char* basename = dbInfo.basename.data();

		// 连接数据库，这里的几个参数severname， username，password，bassname，必须均是char*
		if (mysql_real_connect(&conn, servername, username, password, basename, 0, NULL, CLIENT_FOUND_ROWS))
		{
			cout << "connect database ok" << endl;
			// string 类型进行SQL语句拼接
			string sql_command = getSqlcommand(dbInfo.tablename, datatmp);
			const char* p = sql_command.data();
			return_num = mysql_query(&conn, p);
			if (return_num)
			{
				return 1;
			}
			mysql_close(&conn);
			return 0;
		}
	}
	
};

int main(int argc, char *argv[])
{
	// 获取string类型当前时间
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	string updateTime = string(strTime.GetBuffer(), strTime.GetLength());
	std::cout << updateTime << endl;

	// 构造人数统计表，数据库及表的信息
	dataBaseInfo dbInfo;
	dbInfo.servername = "localhost";
	dbInfo.username = "root";
	dbInfo.password = "123456";
	dbInfo.basename = "classroom";
	dbInfo.tablename = "cquptpersoncount";
	// 构造人数统计表结构体
	personDataType tmp1;
	tmp1.classroomId = 1010; //教室号
	tmp1.time = updateTime;
	tmp1.personCount = 11;   //总人数统计结果

	// 存入数据库
	storeToSql sqlStorage;
	int return_num = sqlStorage.insertData(dbInfo, tmp1);
	if (return_num == 0)cout << "insert ok,no error" << endl;
	else cout << "error happened,please check your data" << endl;

	// 构造人数统计表，数据库及表的信息
	// dataBaseInfo dbInfo;
	dbInfo.servername = "localhost";
	dbInfo.username = "root";
	dbInfo.password = "123456";
	dbInfo.basename = "classroom";
	dbInfo.tablename = "cquptposecount";
	// 构造人数统计表结构体
	poseDataType tmp2;
	tmp2.classroomId = 1010; //教室号
	tmp2.time = updateTime;
	tmp2.standing = 11;   //standing人数统计结果
	tmp2.sitting = 12;   //sitting人数统计结果
	tmp2.lying = 13;   //lying人数统计结果


	// 存入数据库
	return_num = sqlStorage.insertData(dbInfo, tmp2);
	if (return_num == 0)cout << "insert ok,no error" << endl;
	else cout << "error happened,please check your data" << endl;
	
	getchar();
	return 0;
}
