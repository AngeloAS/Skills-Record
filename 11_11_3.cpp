#include <iostream>
#include <vector>
#include <mysql.h>
#include <string>
#include <atltime.h>

using namespace std;

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
string getSqlcommand(string tablename,personDataType datatmp)
{
	// string 类型进行SQL语句拼接
	string pre_str = "insert into ";
	string middle_str = " values('";
	string t = "', '";
	string str = std::to_string(datatmp.classroomId) + t + datatmp.time + t + std::to_string(datatmp.personCount);
	string end_str = "')";
	string sql_command = pre_str + tablename+ middle_str + str + end_str;
	return sql_command;
}
int insertdata(dataBaseInfo dbInfo, personDataType datatmp)
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
		return_num= mysql_query(&conn, p);
		if (return_num)
		{
			return 1;
		}
		mysql_close(&conn);
		return 0;
	}
}


int main(int argc, char *argv[])
{
	dataBaseInfo dbInfo;
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	string updateTime = string(strTime.GetBuffer(), strTime.GetLength());
	std::cout << updateTime << endl;
	vector<personDataType> data;
	personDataType tmp;
	tmp.classroomId = 1010;
	tmp.time = updateTime;
	tmp.personCount = 11;
	int return_num = insertdata(dbInfo, tmp);
	if (return_num == 0)cout << "insert ok,no error" << endl;
	else cout << "error happened,please check your data" << endl;
	getchar();


	
	return 0;
}
