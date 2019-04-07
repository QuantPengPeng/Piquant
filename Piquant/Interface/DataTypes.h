#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include "../../sdk/ThostFtdcUserApiDataType.h"
#include <string>
using namespace std;

///深度行情
struct _Tick
{
	///交易日
	string   TradingDay;
	///合约代码
	string  InstrumentID;
	///交易所代码
	string ExchangeID;
	///最新价
	double  LastPrice;
	///上次结算价
	double  PreSettlementPrice;
	///昨收盘
	double  PreClosePrice;
	///昨持仓量
	double   PreOpenInterest;
	///今开盘
	double  OpenPrice;
	///最高价
	double  HighestPrice;
	///最低价
	double  LowestPrice;
	///数量
	int Volume;
	///成交金额
	double  Turnover;
	///持仓量
	double   OpenInterest;
	///今收盘
	double  ClosePrice;
	///本次结算价
	double  SettlementPrice;
	///涨停板价
	double  UpperLimitPrice;
	///跌停板价
	double  LowerLimitPrice;
	///最后修改时间
	string   UpdateTime;
	///最后修改毫秒
	int   UpdateMillisec;
	///申买价一
	double  BidPrice1;
	///申买量一
	int BidVolume1;
	///申卖价一
	double  AskPrice1;
	///申卖量一
	int AskVolume1;
};

///报单
struct _Order
{
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///报单引用
	string OrderRef;
	///报单日期
	string InsertDate;
	///委托时间
	string InsertTime;
	///报单价格条件
	string OrderPriceType;
	///买卖方向
	string Direction;
	///组合开平标志
	string CombOffsetFlag;
	///价格
	double LimitPrice;
	///数量
	int VolumeTotalOriginal;
	///今成交数量
	int VolumeTraded;
	///剩余数量
	int VolumeTotal;
	///触发条件
	string ContingentCondition;
	///触发价
	double StopPrice;
	///报单提交状态, 为交易所接收的订单会有报单编号,触价单的报单编号是ctp的,这点要区分
	string OrderSubmitStatus;
	///报单状态
	string OrderStatus;
	///状态信息
	string StatusMsg;
	///报单编号由交易所反馈,其非常重要,通过记录及比对报单编号,可以确定某单是否由相应策略接口发出
	string OrderSysID;
};

///成交
struct _Trade
{
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///报单引用
	string OrderRef;
	///成交编号
	string TradeID;
	///买卖方向
	string Direction;
	///报单编号具有唯一性,非常重要
	string OrderSysID;
	///开平标志
	string OffsetFlag;
	///投机套保标志
	string HedgeFlag;
	///价格
	double Price;
	///数量
	int  Volume;
	///成交时期
	string TradeDate;
	///成交时间
	string TradeTime;
};

#endif