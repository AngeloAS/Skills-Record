#include <string>
#include <iostream>
#include <Winsock2.h>
using namespace std;
#pragma comment(lib,"Ws2_32")

class getHttpResponse()
// UTF-8 转 Unicode
wstring UTF82WCS(const char* szU8);

int main()
{
	// 1、创建SOCKET
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		cout<<"Winsock init error!"<<endl;
		return 0;
	}
	if (wsadata.wVersion != MAKEWORD(2, 2))
	{
		cout<<"Winsock version error!"<<endl;
		return 0;
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == SOCKET_ERROR)
	{
		cout<<"Create Socket Fail!"<<endl;
	}
	string url = "t.weather.sojson.com";
	string get_data = "//api//weather//city//101040100";

	// 2、Client 连接至 Web 服务器（用流式套接字实现）
	hostent *p_hostent = gethostbyname(url.c_str()); // 通过域名获取IP地址
	if (p_hostent == NULL)
	{
		cout<<"url error!"<<endl;
		return 0;
	}
	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	memcpy(&(socketAddr.sin_addr), p_hostent->h_addr_list[0], sizeof(socketAddr.sin_addr));
	socketAddr.sin_port = htons(80);
	int cRes = connect(clientSocket, (SOCKADDR*)& socketAddr, sizeof(SOCKADDR));
	int lens = sizeof(socketAddr);
	if (SOCKET_ERROR == cRes)
	{
		cout<<"connect fail, please try again!"<<endl;
	}
	else
	{
		// 3、Client 发送 HTTP 请求
		string tp = "GET "+ get_data + " HTTP/1.1\r\nHost:" + url + "\r\nConnection:Close\r\n\r\n"; // 构造http请求
		send(clientSocket, tp.c_str(), tp.size(), 0);

		// 4、Client 接收这个响应消息, 并不断从缓冲区读取字符串，直到读完为止
		char recvBuf[8000];
		memset(recvBuf,0,sizeof(recvBuf));
		cout<<"data:"<<endl;
		int res;
		while(1)
		{
			res = recv(clientSocket,recvBuf,sizeof(recvBuf),0);
			if(res<=0)break; // 缓冲区读完或者出现错误则结束
			wstring ap = UTF82WCS(recvBuf);
			wcout.imbue(locale("chs"));
			wcout << ap;
			

		}
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}

// UTF-8 转 Unicode
wstring UTF82WCS(const char* szU8)
{
	// 预转换，得到所需空间的大小;
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	// 分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	// 转换
	MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
	//最后加上'\0'
	wszString[wcsLen] = '\0';
	wstring unicodeString(wszString);

	delete[] wszString;
	wszString = NULL;

	return unicodeString;
}
