#ifndef _ISTRATEGY_H_
#define _ISTRATEGY_H_

#ifdef _WIN32
#ifdef STRATEGY_EXPORTS
#define STRATEGY_API _declspec(dllexport)
#else
#define STRATEGY_API _declspec(dllimport)
#endif
#else
#define STRATEGY_API 
#endif

//策略框架类,未来所有策略都将从这里继承重载

#include "../ServiceLayer/CtpService.h"
#include "../ServiceLayer/CtpMsgDistributor.h"
#include "../ServiceLayer/DataTypes.h"
#include <vector>
#include "StrategyMessage.h" 
#include "DataTypes.h"
using namespace std;

class STRATEGY_API StrategyMsgDistributor
{//策略消息分发器
protected:
	StrategyMsgDistributor();
	virtual ~StrategyMsgDistributor();
public:
	void  OnResponse(int response_id, LPARAM lParam); //策略消息分发
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);	//子类重载用于接收策略消息
	void AddReceiver(StrategyMsgDistributor* receiver);
	void DelReceiver(StrategyMsgDistributor* receiver);
	friend STRATEGY_API StrategyMsgDistributor*  get_strategymsg_distributor();//友元函数
protected:
	vector<StrategyMsgDistributor*> receivers;
};



class STRATEGY_API IStrategy : public CtpMsgReceiver
{
public:
	IStrategy();
	virtual ~IStrategy();

	void set_ApiInterface(IService* pApiInterface); //考虑以后不仅对接CTP,也可对接IB等其它接口
	void start(); //策略启动
	void stop(); //策略停止
	bool isRunning(); //记录策略状态,是正在运行中,还是已停止
	void OnReceiveData(WPARAM wParam, LPARAM lParam); //接收各种回报
	
	//策略类中需重载
	virtual void OnTick(_Tick& market_data);
	virtual void OnOrder(_Order& order);
	virtual void OnTrade(_Trade& trade);
	

	//API函数，提供给策略编写调用，未完待续
	bool make_order(const std::string& instrumentID, //订阅合约ID,此处只支持单品种行情订阅,注意上期所区分平今平昨
							  char Direction,
							  char OpenCloseType,
							  double price,
							  int volume
							  );
	bool cancel_order(const std::string& InstrumentID, const std::string& ExchangeID, const std::string& OrderSysID);
	bool PostMessage_StrategyStatus(StrategyStatus msg); //策略状态消息传递函数
	bool PostMessage_StrategyMsg(StrategyMsg msg);//策略消息传递函数
	std::vector<std::string> split(std::string str, std::string pattern);

	
protected:
	IService* ApiInterface;	    //接口对象
	string     instrument_id;    //合约名,只支持单合约订阅，一个策略对应一个品种
	string StrategyName;
	string StrategyDescription;
	int position_buy;
	int position_sell;
	bool running;	//是否正在运行
};


#endif

