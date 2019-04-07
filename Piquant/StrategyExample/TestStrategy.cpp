#include "stdafx.h"
#include "TestStrategy.h"
#include "../Tool/rwJson.h"


TestStrategy::TestStrategy()
{
	//策略编写模板自带(必须)
	this->StrategyDescription = "策略样例(测试用)";
	this->StrategyName = "StrategyExample";
	this->instrument_id = "cu1905";//设置订阅的品种, 注意区分大小写
	//从json中读取持仓数据
	get_rwJson().getPosition(this->StrategyName, this->position_buy, this->position_sell);
	
	//初始化策略参数
	count = 0;
	lastDirection = 0;
	lastPrice = 0;
	vol = 1;//默认开一手
}


TestStrategy::~TestStrategy()
{
	//在此处写持仓是无效的，软件关闭时并不会执行该析构
}

void TestStrategy::OnTick(_Tick& market_data)
{
	//::MessageBox(NULL, _T("OnTick推送"), _T("测试"), MB_OK);//调试用
	if (lastPrice==0)
	{//记录最新价
		lastPrice = market_data.LastPrice;
		lastDirection = 0;//因无前价比对,所以不明方向，多头强势为1，空头强势为-1
	}
	else
	{
		if (market_data.LastPrice>lastPrice)
		{//最新价>上一最新价
			if (lastDirection!=0)
			{
				if (lastDirection == 1)
				{//可持续的多头强势
					count++;
					lastPrice = market_data.LastPrice;
				}
				else
				{//上一为空头强势，如今为多头强势，重置计数
					count = 1;
					lastDirection = 1;
					lastPrice = market_data.LastPrice;
				}
			}
			else
			{//首次初始化强势方向
				count++;
				lastDirection = 1;
				lastPrice = market_data.LastPrice;
			}
		}

		if (market_data.LastPrice < lastPrice)
		{//最新价<上一最新价
			if (lastDirection != 0)
			{
				if (lastDirection == -1)
				{//可持续的空头强势
					count++;
					lastPrice = market_data.LastPrice;
				}
				else
				{//上一为多头强势，如今为空头强势，重置计数
					count = 1;
					lastDirection = -1;
					lastPrice = market_data.LastPrice;
				}
			}
			else
			{//首次初始化强势方向
				count++;
				lastDirection = -1;
				lastPrice = market_data.LastPrice;
			}
		}
	}

	if (position_buy==0 && position_sell==0)
	{
		if (count >= 3 && lastDirection == 1)
		{//持续3个tick保持多头强势，则开仓，但只开一次
			make_order(
				market_data.InstrumentID,
				THOST_FTDC_D_Buy,
				THOST_FTDC_OF_Open,
				market_data.AskPrice1, vol);
		}

		if (count >= 3 && lastDirection == -1)
		{//持续3个tick保持空头强势，则开仓，但只开一次
			make_order(
				market_data.InstrumentID,
				THOST_FTDC_D_Sell,
				THOST_FTDC_OF_Open,
				market_data.BidPrice1, vol);
		}
	}
}


void TestStrategy::OnOrder(_Order& order)
{//提取所有带有报单编号的订单，并将报单编号一致的按最新的覆盖，提取状态信息，对订单进行分类本地缓存
	std::vector<std::string> orderref_strs=split(order.OrderRef, "_");	//注意分割符为"_",是由CTP封装层的make_order()设定的
	if (orderref_strs[0]==this->StrategyName)
	{//若前缀匹配,说明是由该策略发出的委托产生的回调
		if (order.OrderStatus == "部分成交还在队列中" ||
			order.OrderStatus == "未成交还在队列中")
		{//报单状态为以上类型的应本地缓存这些未成委托信息, 关键留意其报单编号，依靠报单编号来撤单
			for (vector<_Order>::iterator it = UnfilledOrderVector.begin(); it != UnfilledOrderVector.end();)
			{//注意部分成交情况下，多条报单依次依时回报，但未成委托的报单编号是一直一致的，只是分批成交下成交编号各次不同, 为此应覆盖并更新委托信息
				if (strcmp((it->OrderSysID).c_str(), (order.OrderSysID).c_str()) == 0)
				{//覆盖，即是删除后再压入
					it = UnfilledOrderVector.erase(it); //删除迭代器指向的元素,erase后迭代器即广义指针会无效，此处必须重置
					UnfilledOrderVector.push_back(order);
					return; //刷新完后直接return
				}
				else
				{
					++it;
				}
			}
			UnfilledOrderVector.push_back(order);//不存在则无需更新,压入即可
		}

		if (order.OrderStatus == "撤单")
		{//若已经撤单，则需从vector中剔除
			for (vector<_Order>::iterator it = UnfilledOrderVector.begin(); it != UnfilledOrderVector.end();)
			{
				if (strcmp((it->OrderSysID).c_str(), (order.OrderSysID).c_str()) == 0)
				{//匹配报单编号
					it = UnfilledOrderVector.erase(it);
					return;
				}
				else
				{
					++it;
				}
			}
		}

		if (order.OrderStatus == "全部成交")
		{//若全部成交，则需从vector中剔除
			for (vector<_Order>::iterator it = UnfilledOrderVector.begin(); it != UnfilledOrderVector.end();)
			{
				if (strcmp((it->OrderSysID).c_str(), (order.OrderSysID).c_str()) == 0)
				{//匹配报单编号
					it = UnfilledOrderVector.erase(it);
					return;
				}
				else
				{
					++it;
				}
			}
		}
	}
}

void TestStrategy::OnTrade(_Trade& trade)
{
	std::vector<std::string> orderref_strs = split(trade.OrderRef, "_");	//注意分割符为"_",是由CTP封装层的make_order()设定的
	if (orderref_strs[0] == this->StrategyName)
	{//若前缀匹配,说明是由该策略发出的委托产生的成交回调
		//区分多开/多平/空开/空平以刷新持仓缓存及持久化
		if (trade.Direction == "多")
		{
			if (trade.OffsetFlag == "开仓")
			{
				position_buy += 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
			if (trade.OffsetFlag == "平仓")
			{
				position_buy -= 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
			if (trade.OffsetFlag == "平今")
			{
				position_buy -= 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
		}
		if (trade.Direction == "空")
		{
			if (trade.OffsetFlag == "开仓")
			{
				position_sell += 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
			if (trade.OffsetFlag == "平仓")
			{
				position_sell -= 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
			if (trade.OffsetFlag == "平今")
			{
				position_sell -= 1;
				get_rwJson().storePosition(this->StrategyName, this->position_buy, this->position_sell);	//给json中写入持仓数据
			}
		}
	}
}


#ifdef __cplusplus
extern "C"
{
#endif
	STRATEGYEXAMPLE_API void* CreateStrategy()
	{
		static TestStrategy* pStrategy;
		if (pStrategy==NULL)
		{
			pStrategy = new TestStrategy();
		}
		return pStrategy;
	}
#ifdef __cplusplus
}
#endif

