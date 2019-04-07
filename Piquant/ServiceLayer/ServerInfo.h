#ifndef _SERVER_INFO_H_
#define _SERVER_INFO_H_
#include<string>
#include<vector>
using namespace std;
struct ServerInfo
{
	string id;			//经纪商ID
	string name;		//经纪商名
	string protocol; //协议: tcp; udp等
	vector<string> trade_server_front;		//交易前置
	vector<string> market_server_front;	//行情前置
	int trade_server_port;		//交易前置端口
	int market_server_port;		//行情前置端口
}; 
#endif
