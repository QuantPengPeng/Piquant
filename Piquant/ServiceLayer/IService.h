#ifndef _ISERVICE_H_
#define _ISERVICE_H_
#include "stdafx.h"
#include<string>
#include"ServerInfo.h"
using namespace std;

struct BaseData;  //用前向声明代替include "Datatypes.h", 防止重复包含
struct Exchange;
struct InvestorPositionDetail;

class SERVICE_API IService
{
public:
	IService();
	virtual ~IService();
	virtual bool login(const string& user_name, const string& password, const ServerInfo& broker_id)=0;
	virtual bool logout()=0;
	virtual bool islogined() = 0;
	virtual bool request(int request_id,  void* param)=0;
	virtual bool get_basic_data(vector<BaseData>& base_data)=0;	//获取合约基础数据
	virtual bool get_exchanges(vector<Exchange>& exchanges)=0;	//获取交易所信息
	virtual bool get_all_InvestorPositionDetails(vector<InvestorPositionDetail>& positiondetails)=0;	//获取持仓明细数据
	virtual std::string get_username() = 0;		//获取用户名
	virtual ServerInfo get_brokerinfo() = 0;		//获取经纪商信息
};

#endif

