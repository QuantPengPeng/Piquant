#ifndef _TRADE_SPI_H_
#define _TRADE_SPI_H_
#include "stdafx.h"
#include <tchar.h>//用于处理unicode字符集中的_T()的头文件
#include "../../sdk/ThostFtdcTraderApi.h"
#include "../../sdk/ThostFtdcUserApiDataType.h"
#include "../../sdk/ThostFtdcUserApiStruct.h"
#include "DataTypes.h"
#include "ServerInfo.h"
#include "MdSpi.h"

//TradeSpi继承自CThostFtdcTraderSpi，并对其中的回调虚函数进行重载，
class TradeSpi : public CThostFtdcTraderSpi
{
public:
	TradeSpi();
	virtual ~TradeSpi();

public://主动函数部分
	void Init(const ServerInfo& info);
	bool Login(const string& username, const string& password);
	CThostFtdcTraderApi* GetTraderApi();//获取m_pTradeApi用的外部接口
	const ErrorMessage& LastError() const{ return last_error; }//获取last_error用的外部接口,为内联函数
	vector<BaseData>& get_all_instruments(){ return all_instruments; } //获取查询合约的外部接口
	vector<Exchange>& get_all_exchanges(){ return all_exchanges; }	//获取查询交易所的外部接口
	ServerInfo& get_brokerinfo(){ return m_BrokerInfo; } //获取经纪商信息的外部接口
	string& get_username(){ return username; }//获取用户名的外部接口
	UserInfo& get_userinfo(){ return user_info; }	//获取登录信息结构体
	vector<InvestorPosition>& get_all_InvestorPositions(){ return all_InvestorPositions; } //获取所有持仓的外部接口
	vector<InvestorPositionDetail>& get_all_InvestorPositionDetails(){ return all_InvestorPositionsDetail; } //获取所有持仓明细的外部接口

protected:
	bool IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo);//检查回调是否返回错误，以此明确主动函数的操作是否成功,为通用型函数
	void SetLastError(CThostFtdcRspInfoField *pRspInfo);

public:
	virtual void OnFrontConnected();
	virtual void OnFrontDisconnected(int nReason);
	virtual void OnHeartBeatWarning(int nTimeLapse);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);
	////投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单录入请求响应，录入错误时对应响应，当执行ReqOrderInsert后有字段填写不对之类的CTP报错则通过此接口返回
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单通知,报单正确时响应
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	//成交通知，报单正确时响应
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	//是否确认过结算信息的反馈
	virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询交易所后的反馈
	virtual void OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询某交易品种的费率
	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//报单录入请求响应，录入错误时对应响应
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	//查询委托响应
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询成交响应
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//返回给交易员的撤单错误响应，不处理
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
	//查询保证金返回
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询汇率返回
	virtual void OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询报单手续费返回
	virtual void OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询产品有效合约返回
	virtual void OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询深度行情
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询持仓明细返回
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//批量撤单错误，柜台返回
	virtual void OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//批量撤单错误，交易所返回
	virtual void OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo);
	//查询期货公司客户通知
	virtual void OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询交易通知
	virtual void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//下预埋回调
	virtual void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询预埋单回调
	virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//预埋撤单
	virtual void OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//预埋撤单回调
	virtual void OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//查询预埋撤单回调
	virtual void OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//预埋撤单撤单
	virtual void OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//登出回调
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);


protected:
	int m_iReqestID;//请求编号
	CThostFtdcTraderApi* m_pTradeApi;
	bool m_bInited;//是否被初始化过的记录变量
	UserInfo user_info; //保存登录后返回的登录信息结构体
	ServerInfo m_BrokerInfo;//将经纪公司信息保存起来
	string username;
	string password;
	string tradingday;//用于比较是否结算确认过
	ErrorMessage last_error;//最近的错误消息结构体
	vector<Exchange> all_exchanges;//用于保存交易所详细信息的vector容器
	vector<BaseData> all_instruments; //用于保存合约的vector容器
	vector<InvestorPosition> all_InvestorPositions; //用于保存持仓的vector容器
	vector<InvestorPositionDetail> all_InvestorPositionsDetail; //用于保存持仓明细的vector容器

	//是否已查询过，由于存在主动查询，主动查询可能会触发又一轮的被动连环查询，通过标记控制被动连环查询的中断，防止资源浪费
	bool m_AlreadyQryExchange;
	bool m_AlreadyQryPosition;
	bool m_AlreadyQryCommissionRate;  
	bool m_AlreadyQryOrder; 
	bool m_AlreadyQryParkedOrder; 
	bool m_AlreadyQryParkedOrderAction; 
};
#endif // _TRADE_SPI_H_

