#include "stdafx.h"
#include "IStrategy.h"
#include "../ServiceLayer/CtpMsg.h"

IStrategy::IStrategy()
	:ApiInterface(NULL)
	, instrument_id("")
	, StrategyName("")
	, StrategyDescription("")
	, position_sell(0)
	, position_buy(0)
	, running(false)
{
}

IStrategy::~IStrategy()
{
}


void IStrategy::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	//保存行情;
	//MessageBoxA(NULL, ("收到行情"), ("行情"), MB_OK); //调试用
	if (wParam == ctp_msg::RtnMarketData)
	{//tick数据推送
		MarketData* pMarketData = reinterpret_cast<MarketData*>(lParam);
		if (NULL != pMarketData)
		{
			if (strcmp(pMarketData->code, (this->instrument_id).c_str())==0) //检查是否是所需的行情数据
			{//再对数据做一轮简化封装
				static _Tick tick;
				tick.TradingDay = pMarketData->TradingDay;
				tick.InstrumentID = pMarketData->code;
				tick.ExchangeID = pMarketData->ExchangeID;
				tick.LastPrice = pMarketData->LastPrice;
				tick.PreSettlementPrice = pMarketData->PreSettlementPrice;
				tick.PreClosePrice = pMarketData->PreClosePrice;
				tick.PreOpenInterest = pMarketData->PreOpenInterest;
				tick.OpenPrice = pMarketData->OpenPrice;
				tick.HighestPrice = pMarketData->HighestPrice;
				tick.LowestPrice = pMarketData->LowestPrice;
				tick.Volume = pMarketData->Volume;
				tick.Turnover = pMarketData->Turnover;
				tick.OpenInterest = pMarketData->OpenInterest;
				tick.ClosePrice = pMarketData->ClosePrice;
				tick.SettlementPrice = pMarketData->SettlementPrice;
				tick.UpperLimitPrice = pMarketData->UpperLimitPrice;
				tick.LowerLimitPrice = pMarketData->LowerLimitPrice;
				tick.UpdateTime = pMarketData->UpdateTime;
				tick.UpdateMillisec = pMarketData->UpdateMillisec;
				tick.BidPrice1 = pMarketData->BidPrice;
				tick.BidVolume1 = pMarketData->BidVolume;
				tick.AskPrice1 = pMarketData->AskPrice;
				tick.AskVolume1 = pMarketData->AskVolume;
				OnTick(tick);
			}
		}
	}
	else if (wParam == ctp_msg::RtnOrder)
	{//报单推送
		Order *pOrder = reinterpret_cast<Order*>(lParam);
		if (NULL != pOrder)
		{
			if (strcmp(pOrder->InstrumentID, instrument_id.c_str()) == 0)
			{
				static _Order order;
				order.InstrumentID = pOrder->InstrumentID;

				if (pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice)		order.OrderPriceType ="任意价" ;
				else if (pOrder->OrderPriceType == THOST_FTDC_OPT_LimitPrice)		order.OrderPriceType = "限价";
				else order.OrderPriceType = "其它";		//尚有其它类型,暂不处理

				if (pOrder->Direction == THOST_FTDC_D_Buy)		order.Direction = "多";
				else 	order.Direction = "空";

				if (pOrder->CombOffsetFlag[0] == '0')		order.CombOffsetFlag = "开仓";
				else if (pOrder->CombOffsetFlag[0] == '1')		order.CombOffsetFlag = "平仓";
				else if (pOrder->CombOffsetFlag[0] == '3')		order.CombOffsetFlag = "平今";
				else 	order.CombOffsetFlag = "其它";	//其它暂不处理

				order.LimitPrice = pOrder->LimitPrice;
				order.VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
				order.StopPrice = pOrder->StopPrice;
				order.ExchangeID = pOrder->ExchangeID;
				order.VolumeTraded = pOrder->VolumeTraded;
				order.VolumeTotal = pOrder->VolumeTotal;
				order.InsertDate = pOrder->InsertDate;
				order.InsertTime = pOrder->InsertTime;
				order.StatusMsg = pOrder->StatusMsg;
				order.OrderSysID = pOrder->OrderSysID;
				order.OrderRef = pOrder->OrderRef;

				if (pOrder->ContingentCondition == THOST_FTDC_CC_Immediately)		order.ContingentCondition = "立即";
				else if (pOrder->ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice)		order.ContingentCondition = "最新价大于等于条件价";
				else if (pOrder->ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice)		order.ContingentCondition = "最新价小于等于条件价";
				else order.ContingentCondition = "其它";		//尚有其它类型,暂不处理

				if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted)		order.OrderSubmitStatus = "已经提交";
				else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelSubmitted)		order.OrderSubmitStatus = "撤单已经提交";
				else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_ModifySubmitted)		order.OrderSubmitStatus = "修改已经提交";
				else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted)		order.OrderSubmitStatus = "已经接受";
				else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected)		order.OrderSubmitStatus = "报单已经被拒绝";
				else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected)		order.OrderSubmitStatus = "撤单已经被拒绝";
				else order.OrderSubmitStatus = "改单已经被拒绝";		//尚有其它类型,暂不处理

				if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)		order.OrderStatus = "全部成交";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedQueueing)		order.OrderStatus = "部分成交还在队列中";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)		order.OrderStatus = "部分成交不在队列中";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)		order.OrderStatus = "未成交还在队列中";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing)		order.OrderStatus = "未成交不在队列中";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)		order.OrderStatus = "撤单";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_Unknown)		order.OrderStatus = "未知";
				else if (pOrder->OrderStatus == THOST_FTDC_OST_NotTouched)		order.OrderStatus = "尚未触发";
				else order.OrderStatus = "已触发";		//尚有其它类型,暂不处理

				OnOrder(order);
			}
		}
	}
	else if (wParam == ctp_msg::RtnTrade)
	{//成交推送
		Trade *pTrade = reinterpret_cast<Trade*>(lParam);
		if (NULL != pTrade)
		{
			if (strcmp(pTrade->InstrumentID, instrument_id.c_str()) == 0)
			{
				static _Trade trade;
				trade.InstrumentID = pTrade->InstrumentID;
				trade.ExchangeID = pTrade->ExchangeID;
				trade.TradeID = pTrade->TradeID;
				trade.OrderSysID = pTrade->OrderSysID;
				trade.Price = pTrade->Price;
				trade.Volume = pTrade->Volume;
				trade.TradeDate = pTrade->TradeDate;
				trade.TradeTime = pTrade->TradeTime;
				trade.OrderRef = pTrade->OrderRef;

				if (pTrade->Direction == THOST_FTDC_D_Buy)	trade.Direction = "多";
				else trade.Direction = "空";

				if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)	trade.OffsetFlag = "开仓";
				else if (pTrade->OffsetFlag == THOST_FTDC_OF_Close)	trade.OffsetFlag = "平仓";
				else if (pTrade->OffsetFlag == THOST_FTDC_OF_ForceClose)	trade.OffsetFlag = "强平";
				else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)	trade.OffsetFlag = "平今";
				else if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseYesterday)	trade.OffsetFlag = "平昨";
				else if (pTrade->OffsetFlag == THOST_FTDC_OF_ForceOff)	trade.OffsetFlag = "强减";
				else trade.OffsetFlag = "本地强平";

				if (pTrade->HedgeFlag == THOST_FTDC_HF_Speculation)	trade.HedgeFlag = "投机";
				else if (pTrade->HedgeFlag == THOST_FTDC_HF_Arbitrage)	trade.HedgeFlag = "套利";
				else if (pTrade->HedgeFlag == THOST_FTDC_HF_Hedge)	trade.HedgeFlag = "套保";
				else trade.HedgeFlag = "做市商";

				OnTrade(trade);
			}
		}
	}
	else
	{
		//其余推送数据暂不做处理
	}
}

void IStrategy::OnTick(_Tick& market_data)
{//此虚函数应被策略类继承重载,根据行情执行策略的逻辑代码置于此处
}

void IStrategy::OnOrder(_Order& order)
{//此虚函数应被策略类继承重载
}

void IStrategy::OnTrade(_Trade& trade)
{//此虚函数应被策略类继承重载
}

void IStrategy::start()
{//注册为接收者
	get_CtpMsgDistributor()->AddReceiver(this);
	StrategyStatus msg;
	msg.strategyName = this->StrategyName;
	msg.position_buy = this->position_buy;
	msg.position_sell = this->position_sell;
	msg.status = "运行中";
	msg.description = this->StrategyDescription;
	PostMessage_StrategyStatus(msg);

	StrategyMsg msg_strategy;
	msg_strategy.strategyName = this->StrategyName;
	msg_strategy.loggingLevel = ORDINARY;
	msg_strategy.loggingContent = "策略已启动,正接收行情/报单/成交等";
	PostMessage_StrategyMsg(msg_strategy);

	running = true; //标记正在运行状态
}

void IStrategy::stop()
{//从接收者容器中剔除，不再接收行情、报单及成交回报等
	get_CtpMsgDistributor()->DelReceiver(this);
	StrategyStatus msg;
	msg.strategyName = this->StrategyName;
	msg.position_buy = -1;	//-1意味着该值不更新
	msg.position_sell = -1;	//-1意味着该值不更新
	msg.status = "已停止";
	msg.description = this->StrategyDescription;
	PostMessage_StrategyStatus(msg);

	StrategyMsg msg_strategy;
	msg_strategy.strategyName = this->StrategyName;
	msg_strategy.loggingLevel = ORDINARY;
	msg_strategy.loggingContent = "策略已停止";
	PostMessage_StrategyMsg(msg_strategy);

	running = false; //标记停止运行状态
}

//返回策略状态, 其状态控制由start及stop来切换
bool IStrategy::isRunning()
{
	if (running==true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void IStrategy::set_ApiInterface(IService* pApiInterface)
{
	ApiInterface = pApiInterface;
}

bool IStrategy::PostMessage_StrategyStatus(StrategyStatus msg)
{//msg结构体中若有空字段，说明此次消息发送没有给其赋值，接收消息端需判断并略过该字段
	static StrategyStatus content;
	content = msg;
	get_strategymsg_distributor()->OnResponse(WM_UPDATESTRATEGYSTATUS, (LPARAM)&content);
	return true;
}

bool IStrategy::PostMessage_StrategyMsg(StrategyMsg msg)
{
	static StrategyMsg content;
	content = msg;
	get_strategymsg_distributor()->OnResponse(WM_UPDATESTRATEGYMSG, (LPARAM)&content);
	return true;
}

//C++的string字符串分割函数
std::vector<std::string> IStrategy::split(std::string str, std::string pattern)
{
	//pattern为分割符
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

bool IStrategy::make_order(const std::string& instrumentID, char Direction, char OpenCloseType, double price, int volume)
{//报单
	InputOrderParam order_param;
	if (ApiInterface)
	{
		order_param.InstrumentID = instrumentID;
		order_param.Direction = Direction;
		order_param.CombOffsetFlag[0] = OpenCloseType;
		order_param.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  //价格类型默认是发限价单，五大交易所都支持
		order_param.VolumeTotalOriginal = volume;
		order_param.LimitPrice = price;
		order_param.TimeCondition = THOST_FTDC_TC_GFD;
		order_param.OrderRef = this->StrategyName; //策略接口发单要给OrderRef赋前缀, 以区分策略接口下单与手动下单
		ApiInterface->request(ctp_msg::MakeOrder, &order_param);
	}
	return true;
}


bool IStrategy::cancel_order(const std::string& InstrumentID, const std::string& ExchangeID, const std::string& OrderSysID)
{//撤单
	ActionOrderParam action_order;
	memset(&action_order, 0, sizeof(action_order));
	if (ApiInterface)
	{
		strcpy_s(action_order.InstrumentID, InstrumentID.c_str());
		strcpy_s(action_order.ExchangeID, ExchangeID.c_str());
		strcpy_s(action_order.OrderSysID, OrderSysID.c_str());
		ApiInterface->request(ctp_msg::CancelOrder, &action_order);
	}
	return true;
}



//StrategyDistributor类实现部分
StrategyMsgDistributor::StrategyMsgDistributor()
{

}

StrategyMsgDistributor::~StrategyMsgDistributor()
{

}

void StrategyMsgDistributor::OnResponse(int msgID, LPARAM lParam)
{
	for (int i = 0; i < receivers.size(); ++i)
	{
		if (NULL != receivers[i])
		{//lParam置消息结构体的地址
			receivers[i]->OnReceiveData(msgID, lParam);
		}
	}
}

void StrategyMsgDistributor::OnReceiveData(WPARAM wParam, LPARAM lParam)
{//子类重载
}

void StrategyMsgDistributor::AddReceiver(StrategyMsgDistributor* receiver)
{
	vector<StrategyMsgDistributor*>::iterator it = std::find(receivers.begin(), receivers.end(), receiver);
	if (it == receivers.end())
	{//若等于超尾迭代器,说明没找到,才压入接收器容器中
		receivers.push_back(receiver);
	}
}

void StrategyMsgDistributor::DelReceiver(StrategyMsgDistributor* receiver)
{
	//将某个接收者从vector中剔除出去
	for (vector<StrategyMsgDistributor*>::iterator it = receivers.begin(); it != receivers.end(); ++it)
	{
		if (*it == receiver)//若地址相同
		{
			it = receivers.erase(it);//根据指定的迭代器,删除指定接收者
			break;
		}
	}
}

StrategyMsgDistributor* get_strategymsg_distributor()
{
	static StrategyMsgDistributor* pDistributor = NULL;
	if (NULL == pDistributor)
	{
		pDistributor = new StrategyMsgDistributor();
	}
	return pDistributor;
}