#include <iostream>
#include <vector>
#include <mysql.h>
#include <string>
#include <atltime.h>

using namespace std;

struct datatype{
	int classroomId;
	int time;
	int allcount;
	int standing;
	int sitting;
	int lying;
	datatype()
	{
		classroomId = 0;
		time = 0;
		allcount = 0;
		standing = 0;
		sitting = 0;
		lying = 0;
	}
};
class database{
private:
	char* servername = "localhost"; // 服务器名 localhost是指本地
	char* username = "root"; // 用户名
	char* password = "123456"; // 密码
	char* basename = "classroom"; // 数据库名
public:
	int insertdata(vector<datatype>&datatmp)
	{
		MYSQL conn;
		int res;
		mysql_init(&conn);
		if (mysql_real_connect(&conn, servername, username, password, basename, 0, NULL, CLIENT_FOUND_ROWS))
		{
			cout << "connect database ok" << endl;
			for (vector<datatype>::iterator it = datatmp.begin(); it != datatmp.end(); it++)
			{
				string pre_str = "insert into classroom values(";
				string t = ", ";
				string str = std::to_string(it->classroomId) + t + std::to_string(it->time) + t + std::to_string(it->allcount) + t + std::to_string(it->standing) + t + std::to_string(it->sitting) + t + std::to_string(it->lying);
				string end_str = ")";
				string sstr = pre_str + str + end_str;
				const char* p = sstr.data();
				res = mysql_query(&conn, p);
				if (res)
				{
					return 1;
				}
					
			}
			mysql_close(&conn);
			return 0;
		}
	}


};

int main(int argc, char *argv[])
{
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	string updateTime = string(strTime.GetBuffer(), strTime.GetLength());
	std::cout << updateTime << endl;
	vector<datatype> data;
	datatype tmp;
	tmp.classroomId = 1010;
	tmp.time = 10.10;
	tmp.allcount = 100;
	tmp.standing = 20;
	tmp.sitting = 70;
	tmp.lying = 10;
	data.push_back(tmp);
	class database dd;
	int n = dd.insertdata(data);
	if (n == 0)cout << "insert ok,no error" << endl;
	else cout << "error happened,please check your data" << endl;
	getchar();


	
	return 0;
}
