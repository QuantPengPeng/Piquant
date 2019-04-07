#include "stdafx.h"
#include "rwJson.h"


rwJson::rwJson()
{
}


rwJson::~rwJson()
{
}

rwJson& get_rwJson()
{//友元函数
	static rwJson g_rwJson;
	return g_rwJson;
}

/*
	以下为将账户数据写入json文件的示例函数
	可在主框架MainFrm.cpp中加入如下代码实现读写json：
	TradingAccount* pTradingAccount = reinterpret_cast<TradingAccount*>(lParam);
	get_rwJson().storeAccountData(pTradingAccount->Available, pTradingAccount->Balance, pTradingAccount->CloseProfit, pTradingAccount->PositionProfit, pTradingAccount->Commission);
	参考文献：https://www.jianshu.com/p/62fa72a1ae18
	AccountData.json中内容如下所示：
	{
	"可用资金" : 846378.650,
	"权益" : {
			"持仓权益" : -3640.0,
			"动态权益" : 903514.750,
			"平仓权益" : 0.0
				},
	"手续费" : 0.0,
	"信息" : [ "可用资金", "权益", "手续费" ]
	}
*/
bool rwJson::storeAccountData(double avaiable, double profitloss, double closeprofit, double positionprofit, double commission)
{
	using std::ofstream;

	Json::Value root;	//根节点

	root["可用资金"] = Json::Value(avaiable);	//根节点属性
	Json::Value partner;	//子节点
	//子节点属性
	partner["动态权益"] = Json::Value(profitloss);
	partner["平仓权益"] = Json::Value(closeprofit);
	partner["持仓权益"] = Json::Value(positionprofit);
	root["权益"] = Json::Value(partner);	//子节点挂到权益根节点上

	root["手续费"] = Json::Value(commission);		//根节点属性

	//数组形式
	root["信息"].append("可用资金");
	root["信息"].append("权益");
	root["信息"].append("手续费");

	//输出到文件
	Json::StyledWriter sw;
	ofstream os;
	os.open("AccountData.json");
	os << sw.write(root);
	os.close();

	return 0;
}

bool rwJson::getAccountData(double& avaiable, double& profitloss, double& closeprofit, double& positionprofit, double& commission)
{
	using namespace std;

	Json::Reader reader;
	Json::Value root;
	//从文件中读取
	ifstream is;
	is.open("AccountData.json", ios::binary);

	if (reader.parse(is, root))
	{
		//读取根节点信息
		avaiable = root["可用资金"].asDouble();
		commission = root["手续费"].asDouble();

		//读取子节点信息
		profitloss = root["权益"]["持仓权益"].asDouble();
		closeprofit = root["权益"]["动态权益"].asDouble();
		positionprofit = root["权益"]["平仓权益"].asDouble();

		//读取数组信息
		for (int i = 0; i < root["信息"].size(); i++)
		{
			string msg = root["信息"][i].asString();
			cout << msg << '\t';
		}
		cout << endl;
	}
	is.close();
	return 0;
}

bool rwJson::storePosition(std::string strategyName, int position_buy, int position_sell)
{
	using namespace std;
	Json::Value root;	//根节点
	root["多仓"] = Json::Value(position_buy);	
	root["空仓"] = Json::Value(position_sell);		
	//输出到文件
	Json::StyledWriter sw;
	ofstream os;
	std::string json_filename;
	if (os)
	{//若文件打开成功则进行写操作
		json_filename = strategyName + ".json";
		os.open(json_filename);
		os << sw.write(root);
		os.close();
	}
	else
	{//否则新建文件并写入
		fstream file_position;
		file_position.open(json_filename, fstream::out);
		Json::Value root;	//根节点
		root["多仓"] = Json::Value(position_buy);
		root["空仓"] = Json::Value(position_sell);
		//输出到文件
		Json::StyledWriter sw;
		file_position << sw.write(root);
		file_position.close();
	}
	return 0;
}

bool rwJson::getPosition(std::string strategyName, int& position_buy, int& position_sell)
{
	using namespace std;
	Json::Reader reader;
	Json::Value root;
	//从文件中读取
	ifstream is;
	std::string json_filename = strategyName + ".json";
	is.open(json_filename, ios::binary);
	if (is)
	{//若打开文件成功，则进行获取操作
		if (reader.parse(is, root))
		{
			//读取根节点信息
			position_buy = root["多仓"].asInt();
			position_sell = root["空仓"].asInt();
		}
		is.close();
	}
	else
	{//若并不存在该文件，则新建
		fstream file_position;
		file_position.open(json_filename, fstream::out);
		Json::Value root;	//根节点
		root["多仓"] = Json::Value(0);
		root["空仓"] = Json::Value(0);
		//输出到文件
		Json::StyledWriter sw;
		file_position << sw.write(root);
		file_position.close();
	}
	return 0;
}
