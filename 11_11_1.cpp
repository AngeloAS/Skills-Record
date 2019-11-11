#include <iostream>
#include <vector>
#include <mysql.h>
#include <string>
#include <atltime.h>

using namespace std;

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
class database{
private:
	char* servername = "localhost"; // 服务器名 localhost是指本地
	char* username = "root"; // 用户名
	char* password = "123456"; // 密码
	char* basename = "classroom"; // 数据库名
public:
	int insertdata(vector<personDataType>&datatmp)
	{
		MYSQL conn;
		int res;
		mysql_init(&conn);
		if (mysql_real_connect(&conn, servername, username, password, basename, 0, NULL, CLIENT_FOUND_ROWS))
		{
			cout << "connect database ok" << endl;
			for (vector<personDataType>::iterator it = datatmp.begin(); it != datatmp.end(); it++)
			{
				string pre_str = "insert into CquptPersonCount(classroomId,time,personCount) values('";
				string t = "', '";
				string str = std::to_string(it->classroomId) + t + it->time + t + std::to_string(it->personCount);
				string end_str = "')";
				string sstr = pre_str + str + end_str;
				const char* p = sstr.data();
				cout << p << endl;
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
	vector<personDataType> data;
	personDataType tmp;
	tmp.classroomId = 1010;
	tmp.time = updateTime;
	tmp.personCount = 11;
	data.push_back(tmp);
	class database dd;
	int n = dd.insertdata(data);
	if (n == 0)cout << "insert ok,no error" << endl;
	else cout << "error happened,please check your data" << endl;
	getchar();


	
	return 0;
}
