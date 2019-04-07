#ifndef _TEST_STRATEGY_H_
#define _TEST_STRATEGY_H_
#include "../Interface/IStrategy.h"
#include <set>

#ifdef _WIN32
#ifdef STRATEGYEXAMPLE_EXPORTS
#define STRATEGYEXAMPLE_API _declspec(dllexport)
#else
#define STRATEGYEXAMPLE_API _declspec(dllimport)
#endif
#else
#define STRATEGYEXAMPLE_API 
#endif


class STRATEGYEXAMPLE_API TestStrategy :public IStrategy
{
public:
	TestStrategy();
	virtual ~TestStrategy();

	virtual void OnTick(_Tick& market_data);
	virtual void OnOrder(_Order& order);
	virtual void OnTrade(_Trade& trade);

protected:
	vector<_Order> UnfilledOrderVector;                   //用于记录与该策略相关的未成委托, 留意ordersysID、orderstatus

	//策略相关变量，未来考虑以读取配置文件的形式获取
protected:
	int count;				//强势计数
	int lastDirection;	//上一强势方向
	double lastPrice;	//上一最新价
	int vol;					//默认开仓量
};

#endif