#include "stdafx.h"
#include"TradeSpi.h"
#include"CtpMsgDistributor.h"
#include "CtpService.h"
#include <iostream>
#include <atlstr.h>//使用cstring需要该头文件
#include <string>
#include "CtpMsg.h"
using namespace std;


extern vector<BaseData> instruments_data;//引入DataTypes.cpp中的全局变量，给其赋值

TradeSpi::TradeSpi()
{
	m_pTradeApi = NULL;
	m_bInited = false;
	m_iReqestID = 0;

	m_AlreadyQryExchange = false;
	m_AlreadyQryCommissionRate = false;  
	m_AlreadyQryOrder = false;
	m_AlreadyQryPosition = false;
	m_AlreadyQryParkedOrder = false;
	m_AlreadyQryParkedOrderAction = false;
};

TradeSpi::~TradeSpi(){};

CThostFtdcTraderApi* TradeSpi::GetTraderApi()
{
	return m_pTradeApi;
}

void TradeSpi::OnFrontConnected()
{
	get_CtpMsgDistributor()->OnResponse(ctp_msg::RspFrontConnected, 100);//消息号后面携带的应为信息,为给对话框的进度条传递进度数据，由于已经连接成功，因此填100；
	CThostFtdcReqUserLoginField login_field;
	memset(&login_field, 0, sizeof(login_field));
	strcpy_s(login_field.BrokerID, m_BrokerInfo.id.c_str());
	strcpy_s(login_field.UserID, username.c_str());
	strcpy_s(login_field.Password, password.c_str());
	int ret_code = m_pTradeApi->ReqUserLogin(&login_field, ++m_iReqestID);//申请登录
	while (ret_code !=0)
	{
		Sleep(1000);
		ret_code = m_pTradeApi->ReqUserLogin(&login_field, ++m_iReqestID);
	}
}
void TradeSpi::OnFrontDisconnected(int nReason)
{
	//将断开连接的消息告知应用层
	m_AlreadyQryExchange = false;
	m_AlreadyQryCommissionRate = false;  //断线后会自动重连，此时要重新一轮查询循环，为此置为false，能再次查询手续费
	m_AlreadyQryOrder=false;
	m_AlreadyQryParkedOrder = false;
	::MessageBox(NULL, _T("断开连接"), _T("断开连接"), MB_OK);//调试用
	get_CtpMsgDistributor()->OnResponse(ctp_msg::RspFrontDisconnected, (LPARAM)NULL);
}

void TradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
	//cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	//cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;//输出心跳超时多少
}

void TradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	tradingday = pRspUserLogin->TradingDay;//记录当前交易日，为了确认是否结算过的比较
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("ServiceLayer"), MB_OK);//调试用
		return;//若登录不成功,回调返回非空的错误信息结构体pRspInfo,则直接return掉,不要再做之下的操作
	}
	static UserInfo user_info;//声明为static确保其不是临时变量
	strcpy_s(user_info.UserID, pRspUserLogin->UserID);
	strcpy_s(user_info.BrokerID, pRspUserLogin->BrokerID);
	strcpy_s(user_info.TradingDay, pRspUserLogin->TradingDay);
	strcpy_s(user_info.LoginTime, pRspUserLogin->LoginTime);
	strcpy_s(user_info.ExchangeTime, pRspUserLogin->SHFETime);
	user_info.FrontID = pRspUserLogin->FrontID;
	user_info.SessionID = pRspUserLogin->SessionID;
	strcpy_s(user_info.MaxOrderRef, pRspUserLogin->MaxOrderRef);
	strcpy_s(user_info.SystemName, pRspUserLogin->SystemName);
	this->user_info = user_info; //给类成员变量user_info赋值
	if (bIsLast)
	{
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspUserLogined, (LPARAM)&user_info);
	}

	//请求查询结算信息确认，就是看有没有确认结算过
	CThostFtdcQrySettlementInfoConfirmField req = { 0 };
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
	strcpy_s(req.InvestorID, username.c_str());
	int iResult=m_pTradeApi->ReqQrySettlementInfoConfirm(&req, ++m_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = m_pTradeApi->ReqQrySettlementInfoConfirm(&req, ++m_iReqestID);
	}
}

void TradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//::MessageBox(NULL, _T("交易登出"), _T("登出"), MB_OK);//调试用
}

void TradeSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{//查询是否确认过结算信息的回调
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
	}
	else
	{
		if (bIsLast)//如果是最后一次返回
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQrySettlementInfoConfirm, (LPARAM)NULL);
		}
		if (pSettlementInfoConfirm->BrokerID != NULL && strcmp(pSettlementInfoConfirm->ConfirmDate, tradingday.c_str()) == 0)
		{//尚未结算前，pSettlementInfoConfirm->BrokerID为NULL，此时要结算；而结算后pSettlementInfoConfirm->BrokerID不为空且ConfirmDate与tradingday一致
			if (bIsLast)//如果是最后一次返回
			{
				if (m_AlreadyQryExchange == false)
				{
					//查询所有交易所
					Sleep(1000);
					CThostFtdcQryExchangeField  req = { 0 };//在ctp约定中，若传非空但初始化为0的结构体进去，则默认查询所有
					int Qryexchanges_ret = m_pTradeApi->ReqQryExchange(&req, ++m_iReqestID);
					while (Qryexchanges_ret != 0)
					{
						Sleep(1000);
						Qryexchanges_ret = m_pTradeApi->ReqQryExchange(&req, ++m_iReqestID);
					}
				}
			}
		}
		else
		{
			//请求结算信息确认
			CThostFtdcSettlementInfoConfirmField req;
			memset(&req, 0, sizeof(req));
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			int ret_code = m_pTradeApi->ReqSettlementInfoConfirm(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(1000);
				ret_code = m_pTradeApi->ReqSettlementInfoConfirm(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{//确认结算信息后的回调函数
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("反馈"), MB_OK);//调试用
		return;
	}

	if (bIsLast)
	{
		//确认结算成功后，将该消息告知上层应用
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspSettlementInfoConfirm, (LPARAM)NULL);
		if (m_AlreadyQryExchange == false)
		{
			//查询所有交易所
			CThostFtdcQryExchangeField  req = { 0 };//在ctp约定中，若传非空但初始化为0的结构体进去，则默认查询所有
			int Qryexchanges_ret = m_pTradeApi->ReqQryExchange(&req, ++m_iReqestID);
			while (Qryexchanges_ret != 0)
			{
				Sleep(1000);
				Qryexchanges_ret = m_pTradeApi->ReqQryExchange(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	//保存
	if (NULL != pExchange)
	{
		Exchange exchange;
		memset(&exchange, 0, sizeof(exchange));
		exchange.ExchangeID = pExchange->ExchangeID;
		exchange.ExchangeName = pExchange->ExchangeName;
		exchange.ExchangeProperty = pExchange->ExchangeProperty;
		all_exchanges.push_back(exchange);
	}
	else
	{
		return;
	}
	if (bIsLast)
	{//查询完交易所的信息并保存进成员变量exchanges这个vector容器后, 我们可进行下一步的操作,
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryexchanges, (LPARAM)&all_exchanges);
		m_AlreadyQryExchange = true;
		/*//////////////////////////////////////////////

		UI界面层在得知查询交易所信息成功后, 从xml配置文件中读取所需查询的合约集合, 向ctp发送查询部分合约基础数据的请求
		vector<string> my_instruments = get_config_reader().GetMyInstruments();
		get_global_mgr().GetService()->request(QryInstruments, &my_instruments);

		*///////////////////////////////////////////////
	}
}

void TradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (NULL != pInstrument)
	{
		//将接收到的合约基础数据存储起来,为了应用层的视图显示用
		BaseData instrument_data;
		strcpy_s(instrument_data.code, pInstrument->InstrumentID);
		strcpy_s(instrument_data.ExchangeID, pInstrument->ExchangeID);
		strcpy_s(instrument_data.InstrumentName, pInstrument->InstrumentName);
		strcpy_s(instrument_data.ExchangeInstID, pInstrument->ExchangeInstID);
		strcpy_s(instrument_data.ProductID, pInstrument->ProductID);
		instrument_data.ProductClass=pInstrument->ProductClass;
		instrument_data.DeliveryYear = pInstrument->DeliveryYear;
		instrument_data.DeliveryMonth = pInstrument->DeliveryMonth;
		instrument_data.MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume;
		instrument_data.MinMarketOrderVolume = pInstrument->MinMarketOrderVolume;
		instrument_data.MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume;
		instrument_data.MinLimitOrderVolume = pInstrument->MinLimitOrderVolume;
		instrument_data.VolumeMultiple = pInstrument->VolumeMultiple;
		instrument_data.PriceTick = pInstrument->PriceTick;
		strcpy_s(instrument_data.CreateDate, pInstrument->CreateDate);
		strcpy_s(instrument_data.OpenDate, pInstrument->OpenDate);
		strcpy_s(instrument_data.ExpireDate, pInstrument->ExpireDate);
		strcpy_s(instrument_data.StartDelivDate, pInstrument->StartDelivDate);
		strcpy_s(instrument_data.EndDelivDate, pInstrument->EndDelivDate);
		strcpy_s(instrument_data.EndDelivDate, pInstrument->EndDelivDate);
		instrument_data.InstLifePhase = pInstrument->InstLifePhase;
		instrument_data.IsTrading = pInstrument->IsTrading;
		instrument_data.PositionType = pInstrument->PositionType;
		instrument_data.PositionDateType = pInstrument->PositionDateType;
		instrument_data.LongMarginRatio = pInstrument->LongMarginRatio;
		instrument_data.ShortMarginRatio = pInstrument->ShortMarginRatio;
		instrument_data.MaxMarginSideAlgorithm = pInstrument->MaxMarginSideAlgorithm;
		strcpy_s(instrument_data.UnderlyingInstrID, pInstrument->UnderlyingInstrID);
		instrument_data.StrikePrice = pInstrument->StrikePrice;
		instrument_data.OptionsType = pInstrument->OptionsType;
		instrument_data.UnderlyingMultiple = pInstrument->UnderlyingMultiple;
		instrument_data.CombinationType = pInstrument->CombinationType;
		all_instruments.push_back(instrument_data);//先存储到一个临时的本地缓冲区中去
	}
	if (bIsLast)
	{
		//注意每个品种合约信息是单独返回,也就是说每次返回都是bIsLast最后返回,除非是查询所有
		//CString instrument_msg = pInstrument->InstrumentID;   //调试用
		//::MessageBox(NULL, instrument_msg, _T("合约返回"), MB_OK);//调试用
		//一环扣一环地跟随回调去查
		CThostFtdcQryTradingAccountField req = { 0 };
		strcpy_s(req.InvestorID, username.c_str());
		strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
		strcpy_s(req.CurrencyID, "CNY");
		int ret_code=m_pTradeApi->ReqQryTradingAccount(&req, ++m_iReqestID);
		while (ret_code != 0)
		{
			Sleep(1000);
			ret_code = m_pTradeApi->ReqQryTradingAccount(&req, ++m_iReqestID);
		}
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryInstrument, (LPARAM)&all_instruments);
		instruments_data = all_instruments; //给全局变量赋值
	}
}

void TradeSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast && pTradingAccount!=NULL)
	{
		static TradingAccount tradingaccount = { 0 };
		strcpy_s(tradingaccount.BrokerID, pTradingAccount->BrokerID);
		strcpy_s(tradingaccount.AccountID, pTradingAccount->AccountID);
		tradingaccount.PreMortgage = pTradingAccount->PreMortgage;
		tradingaccount.PreCredit = pTradingAccount->PreCredit;
		tradingaccount.PreDeposit = pTradingAccount->PreDeposit;
		tradingaccount.PreBalance = pTradingAccount->PreBalance;
		tradingaccount.PreMargin = pTradingAccount->PreMargin;
		tradingaccount.InterestBase = pTradingAccount->InterestBase;
		tradingaccount.Interest = pTradingAccount->Interest;
		tradingaccount.Deposit = pTradingAccount->Deposit;
		tradingaccount.Withdraw = pTradingAccount->Withdraw;
		tradingaccount.FrozenMargin = pTradingAccount->FrozenMargin;
		tradingaccount.FrozenCash = pTradingAccount->FrozenCash;
		tradingaccount.FrozenCommission = pTradingAccount->FrozenCommission;
		tradingaccount.CurrMargin = pTradingAccount->CurrMargin;
		tradingaccount.CashIn = pTradingAccount->CashIn;
		tradingaccount.Commission = pTradingAccount->Commission;
		tradingaccount.CloseProfit = pTradingAccount->CloseProfit;
		tradingaccount.PositionProfit = pTradingAccount->PositionProfit;
		tradingaccount.Balance = pTradingAccount->Balance;
		tradingaccount.Available = pTradingAccount->Available;
		tradingaccount.WithdrawQuota = pTradingAccount->WithdrawQuota;
		tradingaccount.Reserve = pTradingAccount->Reserve;
		strcpy_s(tradingaccount.TradingDay, pTradingAccount->TradingDay);
		tradingaccount.SettlementID = pTradingAccount->SettlementID;
		tradingaccount.Mortgage = pTradingAccount->Mortgage;
		tradingaccount.ExchangeMargin = pTradingAccount->ExchangeMargin;
		tradingaccount.DeliveryMargin = pTradingAccount->DeliveryMargin;
		tradingaccount.ReserveBalance = pTradingAccount->ReserveBalance;
		strcpy_s(tradingaccount.CurrencyID, pTradingAccount->CurrencyID);
		tradingaccount.PreFundMortgageIn = pTradingAccount->PreFundMortgageIn;
		tradingaccount.PreFundMortgageOut = pTradingAccount->PreFundMortgageOut;
		tradingaccount.FundMortgageIn = pTradingAccount->FundMortgageIn;
		tradingaccount.FundMortgageOut = pTradingAccount->FundMortgageOut;
		tradingaccount.FundMortgageAvailable = pTradingAccount->FundMortgageAvailable;
		tradingaccount.MortgageableFund = pTradingAccount->MortgageableFund;
		tradingaccount.SpecProductMargin = pTradingAccount->SpecProductMargin;
		tradingaccount.SpecProductFrozenMargin = pTradingAccount->SpecProductFrozenMargin;
		tradingaccount.SpecProductCommission = pTradingAccount->SpecProductCommission;
		tradingaccount.SpecProductFrozenCommission = pTradingAccount->SpecProductFrozenCommission;
		tradingaccount.SpecProductPositionProfit = pTradingAccount->SpecProductPositionProfit;
		tradingaccount.SpecProductCloseProfit = pTradingAccount->SpecProductCloseProfit;
		tradingaccount.SpecProductPositionProfitByAlg = pTradingAccount->SpecProductPositionProfitByAlg;
		tradingaccount.SpecProductExchangeMargin = pTradingAccount->SpecProductExchangeMargin;
		tradingaccount.BizType = pTradingAccount->BizType;
		tradingaccount.FrozenSwap = pTradingAccount->FrozenSwap;
		tradingaccount.RemainSwap = pTradingAccount->RemainSwap;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryTradingAccount, (LPARAM)&tradingaccount);
		//接下来查账户持仓
		if (m_AlreadyQryPosition == false)
		{
			Sleep(2000);
			CThostFtdcQryInvestorPositionField req;
			memset(&req, 0, sizeof(req));
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			strcpy_s(req.InstrumentID, "");//不填写合约则返回所有持仓
			int ret_code = m_pTradeApi->ReqQryInvestorPosition(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(2000);
				m_pTradeApi->ReqQryInvestorPosition(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	static vector<InvestorPosition> all_InvestorPosition; 
	InvestorPosition Position;
	if (pInvestorPosition!=NULL)
	{
		memset(&Position, 0, sizeof(Position));
		Position.InstrumentID = pInvestorPosition->InstrumentID;
		Position.BrokerID = pInvestorPosition->BrokerID;
		Position.InvestorID = pInvestorPosition->InvestorID;
		Position.PosiDirection = pInvestorPosition->PosiDirection;
		Position.HedgeFlag = pInvestorPosition->HedgeFlag;
		Position.PositionDate = pInvestorPosition->PositionDate;
		Position.YdPosition = pInvestorPosition->YdPosition;
		Position.Position = pInvestorPosition->Position;
		Position.LongFrozen = pInvestorPosition->LongFrozen;
		Position.ShortFrozen = pInvestorPosition->ShortFrozen;
		Position.LongFrozenAmount = pInvestorPosition->LongFrozenAmount;
		Position.ShortFrozenAmount = pInvestorPosition->ShortFrozenAmount;
		Position.OpenVolume = pInvestorPosition->OpenVolume;
		Position.CloseVolume = pInvestorPosition->CloseVolume;
		Position.OpenAmount = pInvestorPosition->OpenAmount;
		Position.CloseAmount = pInvestorPosition->CloseAmount;
		Position.PositionCost = pInvestorPosition->PositionCost;
		Position.PreMargin = pInvestorPosition->PreMargin;
		Position.UseMargin = pInvestorPosition->UseMargin;
		Position.FrozenCash = pInvestorPosition->FrozenCash;
		Position.FrozenMargin = pInvestorPosition->FrozenMargin;
		Position.FrozenCommission = pInvestorPosition->FrozenCommission;
		Position.CashIn = pInvestorPosition->CashIn;
		Position.Commission = pInvestorPosition->Commission;
		Position.CloseProfit = pInvestorPosition->CloseProfit;
		Position.PositionProfit = pInvestorPosition->PositionProfit;
		Position.PreSettlementPrice = pInvestorPosition->PreSettlementPrice;
		Position.SettlementPrice = pInvestorPosition->SettlementPrice;
		Position.TradingDay = pInvestorPosition->TradingDay;
		Position.SettlementID = pInvestorPosition->SettlementID;
		Position.OpenCost = pInvestorPosition->OpenCost;
		Position.ExchangeMargin = pInvestorPosition->ExchangeMargin;
		Position.CombPosition = pInvestorPosition->CombPosition;
		Position.CombLongFrozen = pInvestorPosition->CombLongFrozen;
		Position.CombShortFrozen = pInvestorPosition->CombShortFrozen;
		Position.CloseProfitByDate = pInvestorPosition->CloseProfitByDate;
		Position.CloseProfitByTrade = pInvestorPosition->CloseProfitByTrade;
		Position.TodayPosition = pInvestorPosition->TodayPosition;
		Position.MarginRateByMoney = pInvestorPosition->MarginRateByMoney;
		Position.MarginRateByVolume = pInvestorPosition->MarginRateByVolume;
		Position.StrikeFrozen = pInvestorPosition->StrikeFrozen;
		Position.StrikeFrozenAmount = pInvestorPosition->StrikeFrozenAmount;
		Position.AbandonFrozen = pInvestorPosition->AbandonFrozen;
		Position.ExchangeID = pInvestorPosition->ExchangeID;
		Position.YdStrikeFrozen = pInvestorPosition->YdStrikeFrozen;
		Position.InvestUnitID = pInvestorPosition->InvestUnitID;
		all_InvestorPosition.push_back(Position);
	}
	if (bIsLast)
	{
		this->all_InvestorPositions = all_InvestorPosition;
		m_AlreadyQryPosition = true;  //持仓查询过后请置为true，防止后续再次查找造成阻塞
		Sleep(1000);
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryInvestorPosition, (LPARAM)&this->all_InvestorPositions);
		all_InvestorPosition.clear();
		if (m_AlreadyQryCommissionRate==false)
		{
			CThostFtdcQryInstrumentCommissionRateField req = { 0 };
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			strcpy_s(req.InstrumentID, "");//如果InstrumentID填空，则返回持仓对应的合约手续费率
			int ret_code = m_pTradeApi->ReqQryInstrumentCommissionRate(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(2000);
				ret_code = m_pTradeApi->ReqQryInstrumentCommissionRate(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast)
	{
		if (m_AlreadyQryCommissionRate==true)
		{
			//由于查询手续费是做成针对合约的查询,而不是成批持仓的手续费组合返回,因此,一定是blsLast
			static InstrumentCommissionRate commission_rate = { 0 };
			strcpy_s(commission_rate.InstrumentID, pInstrumentCommissionRate->InstrumentID);
			commission_rate.InvestorRange = pInstrumentCommissionRate->InvestorRange;
			strcpy_s(commission_rate.BrokerID, pInstrumentCommissionRate->BrokerID);
			strcpy_s(commission_rate.InvestorID, pInstrumentCommissionRate->InvestorID);
			commission_rate.OpenRatioByMoney = pInstrumentCommissionRate->OpenRatioByMoney;
			commission_rate.OpenRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;
			commission_rate.CloseRatioByMoney = pInstrumentCommissionRate->CloseRatioByMoney;
			commission_rate.OpenRatioByVolume = pInstrumentCommissionRate->OpenRatioByVolume;
			commission_rate.CloseTodayRatioByMoney = pInstrumentCommissionRate->CloseTodayRatioByMoney;
			commission_rate.CloseTodayRatioByVolume = pInstrumentCommissionRate->CloseTodayRatioByVolume;
			strcpy_s(commission_rate.ExchangeID, pInstrumentCommissionRate->ExchangeID);
			commission_rate.BizType = pInstrumentCommissionRate->BizType;
			strcpy_s(commission_rate.InvestUnitID, pInstrumentCommissionRate->InvestUnitID);
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryCommissionRate, (LPARAM)&commission_rate);
			return;
		}
		m_AlreadyQryCommissionRate = true;  //手续费查询过后请置为true，防止后续再次查找造成阻塞
		if (m_AlreadyQryOrder==false)
		{
			CThostFtdcQryOrderField req = { 0 };
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			int ret_code = m_pTradeApi->ReqQryOrder(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(2000);
				ret_code = m_pTradeApi->ReqQryOrder(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	static vector<Order> all_Order; 
	Order order;
	if (pOrder != NULL)
	{
		strcpy_s(order.BrokerID,  pOrder->BrokerID);
		strcpy_s(order.InvestorID, pOrder->InvestorID);
		strcpy_s(order.InstrumentID, pOrder->InstrumentID);
		strcpy_s(order.OrderRef, pOrder->OrderRef);
		strcpy_s(order.UserID, pOrder->UserID);
		order.OrderPriceType=pOrder->OrderPriceType;
		order.Direction = pOrder->Direction;
		strcpy_s(order.CombOffsetFlag, pOrder->CombOffsetFlag);
		strcpy_s(order.CombHedgeFlag, pOrder->CombHedgeFlag);
		order.LimitPrice = pOrder->LimitPrice;
		order.VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
		order.TimeCondition = pOrder->TimeCondition;
		strcpy_s(order.GTDDate, pOrder->GTDDate);
		order.VolumeCondition = pOrder->VolumeCondition;
		order.MinVolume = pOrder->MinVolume;
		order.ContingentCondition = pOrder->ContingentCondition;
		order.StopPrice = pOrder->StopPrice;
		order.ForceCloseReason = pOrder->ForceCloseReason;
		order.IsAutoSuspend = pOrder->IsAutoSuspend;
		strcpy_s(order.BusinessUnit, pOrder->BusinessUnit);
		order.RequestID = pOrder->RequestID;
		strcpy_s(order.OrderLocalID, pOrder->OrderLocalID);
		strcpy_s(order.ExchangeID, pOrder->ExchangeID);
		strcpy_s(order.ParticipantID, pOrder->ParticipantID);
		strcpy_s(order.ClientID, pOrder->ClientID);
		strcpy_s(order.ExchangeInstID, pOrder->ExchangeInstID);
		strcpy_s(order.TraderID, pOrder->TraderID);
		order.InstallID = pOrder->InstallID;
		order.OrderSubmitStatus = pOrder->OrderSubmitStatus;
		order.NotifySequence = pOrder->NotifySequence;
		strcpy_s(order.TradingDay, pOrder->TradingDay);
		order.SettlementID = pOrder->SettlementID;
		strcpy_s(order.OrderSysID, pOrder->OrderSysID);
		order.OrderSource = pOrder->OrderSource;
		order.OrderStatus = pOrder->OrderStatus;
		order.OrderType = pOrder->OrderType;
		order.VolumeTraded = pOrder->VolumeTraded;
		order.VolumeTotal = pOrder->VolumeTotal;
		strcpy_s(order.InsertDate, pOrder->InsertDate);
		strcpy_s(order.InsertTime, pOrder->InsertTime);
		strcpy_s(order.ActiveTime, pOrder->ActiveTime);
		strcpy_s(order.SuspendTime, pOrder->SuspendTime);
		strcpy_s(order.UpdateTime, pOrder->UpdateTime);
		strcpy_s(order.CancelTime, pOrder->CancelTime);
		strcpy_s(order.ActiveTraderID, pOrder->ActiveTraderID);
		strcpy_s(order.ClearingPartID, pOrder->ClearingPartID);
		order.SequenceNo = pOrder->SequenceNo;
		order.FrontID = pOrder->FrontID;
		order.SessionID = pOrder->SessionID;
		strcpy_s(order.UserProductInfo ,pOrder->UserProductInfo);
		strcpy_s(order.StatusMsg, pOrder->StatusMsg);
		order.UserForceClose = pOrder->UserForceClose;
		strcpy_s(order.ActiveUserID, pOrder->ActiveUserID);
		order.BrokerOrderSeq = pOrder->BrokerOrderSeq;
		strcpy_s(order.RelativeOrderSysID, pOrder->RelativeOrderSysID);
		order.ZCETotalTradedVolume = pOrder->ZCETotalTradedVolume;
		order.IsSwapOrder = pOrder->IsSwapOrder;
		strcpy_s(order.BranchID,pOrder->BranchID);
		strcpy_s(order.InvestUnitID, pOrder->InvestUnitID);
		strcpy_s(order.AccountID, pOrder->AccountID);
		strcpy_s(order.CurrencyID, pOrder->CurrencyID);
		strcpy_s(order.IPAddress, pOrder->IPAddress);
		strcpy_s(order.MacAddress, pOrder->MacAddress);
		all_Order.push_back(order);
	}
	if (bIsLast)
	{
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryOrder, (LPARAM)&all_Order);
		m_AlreadyQryOrder = true; //委托查询过了,记录下,防止再次查询
		//接下来查询成交
		CThostFtdcQryTradeField req = { 0 };
		strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
		strcpy_s(req.InvestorID, username.c_str());
		int ret_code = m_pTradeApi->ReqQryTrade(&req, ++m_iReqestID);
		while (ret_code != 0)
		{
			Sleep(2000);
			ret_code = m_pTradeApi->ReqQryTrade(&req, ++m_iReqestID);
		}
	}
}

void TradeSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	static vector<Trade> all_Trade; //注意要是static，防止临时变量
	Trade trade;
	if (pTrade != NULL)
	{
		strcpy_s(trade.BrokerID, pTrade->BrokerID);
		strcpy_s(trade.InvestorID, pTrade->InvestorID);
		strcpy_s(trade.InstrumentID, pTrade->InstrumentID);
		strcpy_s(trade.OrderRef, pTrade->OrderRef);
		strcpy_s(trade.UserID, pTrade->UserID);
		strcpy_s(trade.ExchangeID, pTrade->ExchangeID);
		strcpy_s(trade.TradeID, pTrade->TradeID);
		trade.Direction = pTrade->Direction;
		strcpy_s(trade.OrderSysID, pTrade->OrderSysID);
		strcpy_s(trade.ParticipantID, pTrade->ParticipantID);
		strcpy_s(trade.ClientID, pTrade->ClientID);
		trade.TradingRole = pTrade->TradingRole;
		strcpy_s(trade.ExchangeInstID, pTrade->ExchangeInstID);
		trade.OffsetFlag = pTrade->OffsetFlag; 
		trade.HedgeFlag = pTrade->HedgeFlag;
		trade.Price = pTrade->Price;
		trade.Volume = pTrade->Volume;
		strcpy_s(trade.TradeDate, pTrade->TradeDate);
		strcpy_s(trade.TradeTime, pTrade->TradeTime);
		trade.TradeType = pTrade->TradeType;
		trade.PriceSource = pTrade->PriceSource;
		strcpy_s(trade.TradeID, pTrade->TradeID);
		strcpy_s(trade.OrderLocalID, pTrade->OrderLocalID);
		strcpy_s(trade.ClearingPartID, pTrade->ClearingPartID);
		strcpy_s(trade.BusinessUnit, pTrade->BusinessUnit);
		trade.SequenceNo = pTrade->SequenceNo;
		strcpy_s(trade.TradingDay, pTrade->TradingDay);
		trade.SettlementID = pTrade->SettlementID;
		trade.BrokerOrderSeq = pTrade->BrokerOrderSeq;
		trade.TradeSource = pTrade->TradeSource;
		strcpy_s(trade.InvestUnitID, pTrade->InvestUnitID);
		all_Trade.push_back(trade);
	}
	if (bIsLast)
	{
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryTrade, (LPARAM)&all_Trade);
		//查持仓明细
		CThostFtdcQryInvestorPositionDetailField req = { 0 };
		strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
		strcpy_s(req.InvestorID, username.c_str());
		int ret_code = m_pTradeApi->ReqQryInvestorPositionDetail(&req, ++m_iReqestID);
		while (ret_code != 0)
		{
			Sleep(2000);
			ret_code = m_pTradeApi->ReqQryInvestorPositionDetail(&req, ++m_iReqestID);
		}
	}
}

void TradeSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	static vector<InvestorPositionDetail> all_PositionDetail; //注意要是static，防止临时变量
	InvestorPositionDetail positionDetail;
	if (pInvestorPositionDetail != NULL)
	{
		memset(&positionDetail, 0, sizeof(positionDetail));
		strcpy_s(positionDetail.BrokerID, pInvestorPositionDetail->BrokerID);
		strcpy_s(positionDetail.InvestorID, pInvestorPositionDetail->InvestorID);
		strcpy_s(positionDetail.InstrumentID, pInvestorPositionDetail->InstrumentID);
		positionDetail.HedgeFlag = pInvestorPositionDetail->HedgeFlag;
		positionDetail.Direction = pInvestorPositionDetail->Direction;
		strcpy_s(positionDetail.OpenDate, pInvestorPositionDetail->OpenDate);
		strcpy_s(positionDetail.TradeID, pInvestorPositionDetail->TradeID);
		positionDetail.Volume = pInvestorPositionDetail->Volume;
		positionDetail.OpenPrice = pInvestorPositionDetail->OpenPrice;
		strcpy_s(positionDetail.TradingDay, pInvestorPositionDetail->TradingDay);
		positionDetail.SettlementID = pInvestorPositionDetail->SettlementID;
		positionDetail.TradeType = pInvestorPositionDetail->TradeType;
		strcpy_s(positionDetail.CombInstrumentID , pInvestorPositionDetail->CombInstrumentID);
		strcpy_s(positionDetail.ExchangeID, pInvestorPositionDetail->ExchangeID);
		positionDetail.CloseProfitByDate = pInvestorPositionDetail->CloseProfitByDate;
		positionDetail.CloseProfitByTrade = pInvestorPositionDetail->CloseProfitByTrade;
		positionDetail.PositionProfitByDate = pInvestorPositionDetail->PositionProfitByDate;
		positionDetail.PositionProfitByTrade = pInvestorPositionDetail->PositionProfitByTrade;
		positionDetail.Margin = pInvestorPositionDetail->Margin;
		positionDetail.ExchMargin = pInvestorPositionDetail->ExchMargin;
		positionDetail.MarginRateByMoney = pInvestorPositionDetail->MarginRateByMoney;
		positionDetail.MarginRateByVolume = pInvestorPositionDetail->MarginRateByVolume;
		positionDetail.LastSettlementPrice = pInvestorPositionDetail->LastSettlementPrice;
		positionDetail.SettlementPrice = pInvestorPositionDetail->SettlementPrice;
		positionDetail.CloseVolume = pInvestorPositionDetail->CloseVolume;
		positionDetail.CloseAmount = pInvestorPositionDetail->CloseAmount;
		strcpy_s(positionDetail.InvestUnitID, pInvestorPositionDetail->InvestUnitID);
		all_PositionDetail.push_back(positionDetail);
	}
	if (bIsLast)
	{
		this->all_InvestorPositionsDetail = all_PositionDetail;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryInvestorPositionDetail, (LPARAM)&all_PositionDetail);
		//接下去可做其它查询
		if (m_AlreadyQryParkedOrder==false)
		{
			CThostFtdcQryParkedOrderField req = {0};
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			int ret_code = m_pTradeApi->ReqQryParkedOrder(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(2000);
				ret_code = m_pTradeApi->ReqQryParkedOrder(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	static vector<ParkedOrder> parkedorder_vector;
	ParkedOrder order;
	if (pParkedOrder!=NULL)
	{
		strcpy_s(order.BrokerID, pParkedOrder->BrokerID);
		strcpy_s(order.InvestorID, pParkedOrder->InvestorID);
		strcpy_s(order.InstrumentID, pParkedOrder->InstrumentID);
		strcpy_s(order.OrderRef, pParkedOrder->OrderRef);
		strcpy_s(order.UserID, pParkedOrder->UserID);
		order.OrderPriceType = pParkedOrder->OrderPriceType;
		order.Direction = pParkedOrder->Direction;
		strcpy_s(order.CombOffsetFlag, pParkedOrder->CombOffsetFlag);
		strcpy_s(order.CombHedgeFlag, pParkedOrder->CombHedgeFlag);
		order.LimitPrice = pParkedOrder->LimitPrice;
		order.VolumeTotalOriginal = pParkedOrder->VolumeTotalOriginal;
		order.TimeCondition = pParkedOrder->TimeCondition;
		strcpy_s(order.GTDDate, pParkedOrder->GTDDate);
		order.VolumeCondition = pParkedOrder->VolumeCondition;
		order.MinVolume = pParkedOrder->MinVolume;
		order.ContingentCondition = pParkedOrder->ContingentCondition;
		order.StopPrice = pParkedOrder->StopPrice;
		order.ForceCloseReason = pParkedOrder->ForceCloseReason;
		order.IsAutoSuspend = pParkedOrder->IsAutoSuspend;
		strcpy_s(order.BusinessUnit, pParkedOrder->BusinessUnit);
		order.RequestID = pParkedOrder->RequestID;
		strcpy_s(order.ExchangeID, pParkedOrder->ExchangeID);
		strcpy_s(order.ClientID, pParkedOrder->ClientID);
		order.UserForceClose = pParkedOrder->UserForceClose;
		strcpy_s(order.InvestUnitID, pParkedOrder->InvestUnitID);
		strcpy_s(order.AccountID, pParkedOrder->AccountID);
		strcpy_s(order.CurrencyID, pParkedOrder->CurrencyID);
		strcpy_s(order.IPAddress, pParkedOrder->IPAddress);
		strcpy_s(order.MacAddress, pParkedOrder->MacAddress);
		strcpy_s(order.ParkedOrderID, pParkedOrder->ParkedOrderID);
		order.UserType = pParkedOrder->UserType;
		order.Status = pParkedOrder->Status;
		order.ErrorID = pParkedOrder->ErrorID;
		strcpy_s(order.ErrorMsg, pParkedOrder->ErrorMsg);
		parkedorder_vector.push_back(order);
	}
	if (bIsLast)
	{
		m_AlreadyQryParkedOrder = true;//查询成功回调后,置为true;
		//若预埋单的errorID为0,则是成功的预埋报单,等待成交
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryParkedOrder, (LPARAM)&parkedorder_vector);
		//接下去可做其它操作
		if (m_AlreadyQryParkedOrderAction == false)
		{
			CThostFtdcQryParkedOrderActionField req = { 0 };
			strcpy_s(req.BrokerID, m_BrokerInfo.id.c_str());
			strcpy_s(req.InvestorID, username.c_str());
			int ret_code = m_pTradeApi->ReqQryParkedOrderAction(&req, ++m_iReqestID);
			while (ret_code != 0)
			{
				Sleep(2000);
				ret_code = m_pTradeApi->ReqQryParkedOrderAction(&req, ++m_iReqestID);
			}
		}
	}
}

void TradeSpi::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		string msg_str = string(pRspInfo->ErrorMsg);
		CString msg = msg_str.c_str();
		::MessageBox(NULL, msg, _T("出错"), MB_OK);//调试用
		return;
	}
	static vector<ParkedOrderAction> order_vector;
	ParkedOrderAction order;
	if (pParkedOrderAction != NULL)
	{
		strcpy_s(order.BrokerID, pParkedOrderAction->BrokerID);
		strcpy_s(order.InvestorID, pParkedOrderAction->InvestorID);
		order.OrderActionRef = pParkedOrderAction->OrderActionRef;
		strcpy_s(order.OrderRef, pParkedOrderAction->OrderRef);
		order.RequestID = pParkedOrderAction->RequestID;
		order.FrontID = pParkedOrderAction->FrontID;
		order.SessionID = pParkedOrderAction->SessionID;
		strcpy_s(order.ExchangeID, pParkedOrderAction->ExchangeID);
		strcpy_s(order.OrderSysID, pParkedOrderAction->OrderSysID);
		order.ActionFlag = pParkedOrderAction->ActionFlag;
		order.LimitPrice = pParkedOrderAction->LimitPrice;
		order.VolumeChange = pParkedOrderAction->VolumeChange;
		strcpy_s(order.UserID, pParkedOrderAction->UserID);
		strcpy_s(order.InstrumentID, pParkedOrderAction->InstrumentID);
		strcpy_s(order.ParkedOrderActionID, pParkedOrderAction->ParkedOrderActionID);
		order.UserType = pParkedOrderAction->UserType;
		order.Status = pParkedOrderAction->Status;
		order.ErrorID = pParkedOrderAction->ErrorID;
		strcpy_s(order.ErrorMsg, pParkedOrderAction->ErrorMsg);
		strcpy_s(order.InvestUnitID, pParkedOrderAction->InvestUnitID);
		strcpy_s(order.IPAddress, pParkedOrderAction->IPAddress);
		strcpy_s(order.MacAddress, pParkedOrderAction->MacAddress);
		order_vector.push_back(order);
	}
	if (bIsLast)
	{
		m_AlreadyQryParkedOrderAction = true;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryParkedOrderAction, (LPARAM)&order_vector);
		//接下去可做其它操作
		return;
	}
}

void TradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{//当执行ReqOrderInsert后有字段填写不对之类的CTP报错则通过此接口返回
	string msg_str = string(pInputOrder->InstrumentID) + "(" + string(pInputOrder->ExchangeID) + ")" + ":" + "   " + string(pRspInfo->ErrorMsg);
	CString msg = msg_str.c_str();
	::MessageBox(NULL, msg, _T("委托出错"), MB_OK);//调试用
}

void TradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{//当执行ReqOrderAction撤单操作时，有字段填写不对之类的CTP报错则通过此接口返回
	string msg_str = string(pInputOrderAction->InstrumentID) + "(" + string(pInputOrderAction->OrderSysID) + ")" + ":" + "   " + string(pRspInfo->ErrorMsg);
	CString msg = msg_str.c_str();
	::MessageBox(NULL, msg, _T("撤单出错"), MB_OK);//调试用
}


void TradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//
}

void TradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{//报单通知，当执行ReqOrderInsert后并且报出后，收到返回则调用此接口，私有流回报。
	//CString msg = pOrder->StatusMsg;
	//::MessageBox(NULL, msg, _T("OnRtnOrder"), MB_OK);//调试用
	static Order order;
	if (pOrder != NULL)
	{
		strcpy_s(order.BrokerID, pOrder->BrokerID);
		strcpy_s(order.InvestorID, pOrder->InvestorID);
		strcpy_s(order.InstrumentID, pOrder->InstrumentID);
		strcpy_s(order.OrderRef, pOrder->OrderRef);
		strcpy_s(order.UserID, pOrder->UserID);
		order.OrderPriceType = pOrder->OrderPriceType;
		order.Direction = pOrder->Direction;
		strcpy_s(order.CombOffsetFlag, pOrder->CombOffsetFlag);
		strcpy_s(order.CombHedgeFlag, pOrder->CombHedgeFlag);
		order.LimitPrice = pOrder->LimitPrice;
		order.VolumeTotalOriginal = pOrder->VolumeTotalOriginal;
		order.TimeCondition = pOrder->TimeCondition;
		strcpy_s(order.GTDDate, pOrder->GTDDate);
		order.VolumeCondition = pOrder->VolumeCondition;
		order.MinVolume = pOrder->MinVolume;
		order.ContingentCondition = pOrder->ContingentCondition;
		order.StopPrice = pOrder->StopPrice;
		order.ForceCloseReason = pOrder->ForceCloseReason;
		order.IsAutoSuspend = pOrder->IsAutoSuspend;
		strcpy_s(order.BusinessUnit, pOrder->BusinessUnit);
		order.RequestID = pOrder->RequestID;
		strcpy_s(order.OrderLocalID, pOrder->OrderLocalID);
		strcpy_s(order.ExchangeID, pOrder->ExchangeID);
		strcpy_s(order.ParticipantID, pOrder->ParticipantID);
		strcpy_s(order.ClientID, pOrder->ClientID);
		strcpy_s(order.ExchangeInstID, pOrder->ExchangeInstID);
		strcpy_s(order.TraderID, pOrder->TraderID);
		order.InstallID = pOrder->InstallID;
		order.OrderSubmitStatus = pOrder->OrderSubmitStatus;
		order.NotifySequence = pOrder->NotifySequence;
		strcpy_s(order.TradingDay, pOrder->TradingDay);
		order.SettlementID = pOrder->SettlementID;
		strcpy_s(order.OrderSysID, pOrder->OrderSysID);
		order.OrderSource = pOrder->OrderSource;
		order.OrderStatus = pOrder->OrderStatus;
		order.OrderType = pOrder->OrderType;
		order.VolumeTraded = pOrder->VolumeTraded;
		order.VolumeTotal = pOrder->VolumeTotal;
		strcpy_s(order.InsertDate, pOrder->InsertDate);
		strcpy_s(order.InsertTime, pOrder->InsertTime);
		strcpy_s(order.ActiveTime, pOrder->ActiveTime);
		strcpy_s(order.SuspendTime, pOrder->SuspendTime);
		strcpy_s(order.UpdateTime, pOrder->UpdateTime);
		strcpy_s(order.CancelTime, pOrder->CancelTime);
		strcpy_s(order.ActiveTraderID, pOrder->ActiveTraderID);
		strcpy_s(order.ClearingPartID, pOrder->ClearingPartID);
		order.SequenceNo = pOrder->SequenceNo;
		order.FrontID = pOrder->FrontID;
		order.SessionID = pOrder->SessionID;
		strcpy_s(order.UserProductInfo, pOrder->UserProductInfo);
		strcpy_s(order.StatusMsg, pOrder->StatusMsg);
		order.UserForceClose = pOrder->UserForceClose;
		strcpy_s(order.ActiveUserID, pOrder->ActiveUserID);
		order.BrokerOrderSeq = pOrder->BrokerOrderSeq;
		strcpy_s(order.RelativeOrderSysID, pOrder->RelativeOrderSysID);
		order.ZCETotalTradedVolume = pOrder->ZCETotalTradedVolume;
		order.IsSwapOrder = pOrder->IsSwapOrder;
		strcpy_s(order.BranchID, pOrder->BranchID);
		strcpy_s(order.InvestUnitID, pOrder->InvestUnitID);
		strcpy_s(order.AccountID, pOrder->AccountID);
		strcpy_s(order.CurrencyID, pOrder->CurrencyID);
		strcpy_s(order.IPAddress, pOrder->IPAddress);
		strcpy_s(order.MacAddress, pOrder->MacAddress);
	}
	get_CtpMsgDistributor()->OnResponse(ctp_msg::RtnOrder, (LPARAM)&order);
}

void TradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{//成交通知，报单发出后有成交则通过此接口返回。私有流
	static Trade trade; //注意要是static，防止临时变量
	if (pTrade != NULL)
	{
		strcpy_s(trade.BrokerID, pTrade->BrokerID);
		strcpy_s(trade.InvestorID, pTrade->InvestorID);
		strcpy_s(trade.InstrumentID, pTrade->InstrumentID);
		strcpy_s(trade.OrderRef, pTrade->OrderRef);
		strcpy_s(trade.UserID, pTrade->UserID);
		strcpy_s(trade.ExchangeID, pTrade->ExchangeID);
		strcpy_s(trade.TradeID, pTrade->TradeID);
		trade.Direction = pTrade->Direction;
		strcpy_s(trade.OrderSysID, pTrade->OrderSysID);
		strcpy_s(trade.ParticipantID, pTrade->ParticipantID);
		strcpy_s(trade.ClientID, pTrade->ClientID);
		trade.TradingRole = pTrade->TradingRole;
		strcpy_s(trade.ExchangeInstID, pTrade->ExchangeInstID);
		trade.OffsetFlag = pTrade->OffsetFlag;
		trade.HedgeFlag = pTrade->HedgeFlag;
		trade.Price = pTrade->Price;
		trade.Volume = pTrade->Volume;
		strcpy_s(trade.TradeDate, pTrade->TradeDate);
		strcpy_s(trade.TradeTime, pTrade->TradeTime);
		trade.TradeType = pTrade->TradeType;
		trade.PriceSource = pTrade->PriceSource;
		strcpy_s(trade.TradeID, pTrade->TradeID);
		strcpy_s(trade.OrderLocalID, pTrade->OrderLocalID);
		strcpy_s(trade.ClearingPartID, pTrade->ClearingPartID);
		strcpy_s(trade.BusinessUnit, pTrade->BusinessUnit);
		trade.SequenceNo = pTrade->SequenceNo;
		strcpy_s(trade.TradingDay, pTrade->TradingDay);
		trade.SettlementID = pTrade->SettlementID;
		trade.BrokerOrderSeq = pTrade->BrokerOrderSeq;
		trade.TradeSource = pTrade->TradeSource;
		strcpy_s(trade.InvestUnitID, pTrade->InvestUnitID);
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RtnTrade, (LPARAM)&trade);
	}
}

void TradeSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{//在ctp检查委托发现错误时，会向投资者发出onRspInsertOrder，同时发出OnErrRtnOrder给相关交易员，作为投资者可不关心此处
	//CString msg = pRspInfo->ErrorMsg;
	//::MessageBox(NULL, msg, _T("OnErrRtnOrderInsert"), MB_OK);//调试用
}

void TradeSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	string msg_str = string(pOrderAction->InstrumentID) + "(" + string(pOrderAction->OrderSysID) + ")" + ":" + "   " + string(pRspInfo->ErrorMsg);
	CString msg = msg_str.c_str();
	::MessageBox(NULL, msg, _T("撤单出错"), MB_OK);//调试用
}


void TradeSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast && pInstrumentMarginRate!=NULL)
	{//由于是按合约查询, 并不按持仓组合返回,为此一定是bIsLast
		static MarginRate margin_rate = { 0 };
		strcpy_s(margin_rate.BrokerID, pInstrumentMarginRate->BrokerID);
		strcpy_s(margin_rate.InstrumentID, pInstrumentMarginRate->InstrumentID);
		strcpy_s(margin_rate.InvestorID, pInstrumentMarginRate->InvestorID);
		margin_rate.InvestorRange = pInstrumentMarginRate->InvestorRange;
		margin_rate.HedgeFlag = pInstrumentMarginRate->HedgeFlag;
		margin_rate.LongMarginRatioByMoney = pInstrumentMarginRate->LongMarginRatioByMoney;
		margin_rate.LongMarginRatioByVolume = pInstrumentMarginRate->LongMarginRatioByVolume;
		margin_rate.ShortMarginRatioByMoney = pInstrumentMarginRate->ShortMarginRatioByMoney;
		margin_rate.ShortMarginRatioByVolume = pInstrumentMarginRate->ShortMarginRatioByVolume;
		margin_rate.IsRelative = pInstrumentMarginRate->IsRelative;
		strcpy_s(margin_rate.ExchangeID, pInstrumentMarginRate->ExchangeID);
		strcpy_s(margin_rate.InvestUnitID, pInstrumentMarginRate->InvestUnitID);
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryMarginRate, (LPARAM)&margin_rate);
		return;
	}
}

void TradeSpi::OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast && pExchangeRate!=NULL)
	{
		static _ExchangeRate exchange_rate = { 0 };
		if (pExchangeRate->BrokerID == NULL)
		{//若pExchangeRate->BrokerID为NULL,说明返回的信息结构体有问题,此处做异常处理,否则软件会崩溃
			strcpy_s(exchange_rate.BrokerID, "---");
			strcpy_s(exchange_rate.FromCurrencyID, "---");
			exchange_rate.FromCurrencyUnit = 0;
			strcpy_s(exchange_rate.ToCurrencyID, "---");
			exchange_rate.ExchangeRate = 0.00;
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryExchangeRate, (LPARAM)&exchange_rate);
			return;
		}
		strcpy_s(exchange_rate.BrokerID, pExchangeRate->BrokerID);
		strcpy_s(exchange_rate.FromCurrencyID, pExchangeRate->FromCurrencyID);
		exchange_rate.FromCurrencyUnit = pExchangeRate->FromCurrencyUnit;
		strcpy_s(exchange_rate.ToCurrencyID, pExchangeRate->ToCurrencyID);
		exchange_rate.ExchangeRate = pExchangeRate->ExchangeRate;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryExchangeRate, (LPARAM)&exchange_rate);
		return;
	}
}

void TradeSpi::OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast && pInstrumentOrderCommRate!=NULL)
	{
		static OrderCommRate order_commrate = { 0 };
		if (pInstrumentOrderCommRate->InstrumentID == NULL)
		{
			strcpy_s(order_commrate.BrokerID, "");
			strcpy_s(order_commrate.InstrumentID, "");
			order_commrate.OrderCommByVolume = 0;
			order_commrate.OrderActionCommByVolume = 0;
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryInstrumentOrderCommRate, (LPARAM)&order_commrate);//消息号后面携带的应为要提供的信息
			return;
		}
		strcpy_s(order_commrate.BrokerID, pInstrumentOrderCommRate->BrokerID);
		strcpy_s(order_commrate.InstrumentID, pInstrumentOrderCommRate->InstrumentID);
		order_commrate.OrderCommByVolume = pInstrumentOrderCommRate->OrderCommByVolume;
		order_commrate.OrderActionCommByVolume = pInstrumentOrderCommRate->OrderActionCommByVolume;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryInstrumentOrderCommRate, (LPARAM)&order_commrate);//消息号后面携带的应为要提供的信息
		return;
	}
}

void TradeSpi::OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (pProduct!=NULL)
	{
		static vector<Product_InvaildInstrument> all_product_invaildInstrument;
		Product_InvaildInstrument invaildInstrument = { 0 };
		strcpy_s(invaildInstrument.ProductID, pProduct->ProductID);
		strcpy_s(invaildInstrument.ProductName, pProduct->ProductName);
		strcpy_s(invaildInstrument.ExchangeID, pProduct->ExchangeID);
		invaildInstrument.ProductClass = pProduct->ProductClass;
		invaildInstrument.VolumeMultiple = pProduct->VolumeMultiple;
		invaildInstrument.PriceTick = pProduct->PriceTick;
		invaildInstrument.MaxMarketOrderVolume = pProduct->MaxMarketOrderVolume;
		invaildInstrument.MinMarketOrderVolume = pProduct->MinMarketOrderVolume;
		invaildInstrument.MaxLimitOrderVolume = pProduct->MaxLimitOrderVolume;
		invaildInstrument.MinLimitOrderVolume = pProduct->MinLimitOrderVolume;
		invaildInstrument.PositionType = pProduct->PositionType;
		invaildInstrument.PositionDateType = pProduct->PositionDateType;
		invaildInstrument.CloseDealType = pProduct->CloseDealType;
		strcpy_s(invaildInstrument.TradeCurrencyID, pProduct->TradeCurrencyID);
		invaildInstrument.MortgageFundUseRange = pProduct->MortgageFundUseRange;
		strcpy_s(invaildInstrument.ExchangeProductID, pProduct->ExchangeProductID);
		invaildInstrument.UnderlyingMultiple = pProduct->UnderlyingMultiple;
		all_product_invaildInstrument.push_back(invaildInstrument);
		if (bIsLast)
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryProduct, (LPARAM)&all_product_invaildInstrument);
			return;
		}
	}
}

void TradeSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (bIsLast && pDepthMarketData!=NULL)
	{
		static DepthMarketData data = { 0 };
		strcpy_s(data.InstrumentID, pDepthMarketData->InstrumentID);
		data.OpenInterest = pDepthMarketData->OpenInterest;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryDepthMarketData, (LPARAM)&data);
		return;
	}
}

void TradeSpi::OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{//有字段填写不对之类的CTP报错则通过此接口返回
	/*
	string msg_str = string(pRspInfo->ErrorMsg);
	CString msg = msg_str.c_str();
	::MessageBox(NULL, msg, _T("全撤出错"), MB_OK);//调试用
	*/
}

void TradeSpi::OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo)
{//交易所回馈
	string msg_str = string(pBatchOrderAction->ActionDate) + "  " + string(pBatchOrderAction->ActionTime) + ":" + "   " + string(pBatchOrderAction->StatusMsg);
	CString msg = msg_str.c_str();
	::MessageBox(NULL, msg, _T("全撤出错"), MB_OK);//调试用
}

void TradeSpi::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	if (pNotice!=NULL)
	{
		static vector<Notice> notice_vector;
		Notice notice;
		strcpy_s(notice.BrokerID, pNotice->BrokerID);
		strcpy_s(notice.Content, pNotice->Content);
		strcpy_s(notice.SequenceLabel, pNotice->SequenceLabel);
		notice_vector.push_back(notice);
		if (bIsLast)
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryNotice, (LPARAM)&notice_vector);//消息号后面携带的应为要提供的信息
		}
	}
}


void TradeSpi::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		SetLastError(pRspInfo);//将错误信息传给上层
		CString error_msg = pRspInfo->ErrorMsg;
		::MessageBox(NULL, error_msg, _T("错误"), MB_OK);//调试用
		return;
	}
	//注意有可能pTradingNotice为混乱且不可读的消息,可通过pTradingNotice->BrokerID来判断
	if (pTradingNotice!=NULL)
	{
		static vector<TradingNotice> tradingnotice_vector;
		TradingNotice tradingnotice;
		strcpy_s(tradingnotice.BrokerID, pTradingNotice->BrokerID);
		tradingnotice.InvestorRange = pTradingNotice->InvestorRange;
		strcpy_s(tradingnotice.InvestorID, pTradingNotice->InvestorID);
		tradingnotice.SequenceSeries = pTradingNotice->SequenceSeries;
		strcpy_s(tradingnotice.UserID, pTradingNotice->UserID);
		strcpy_s(tradingnotice.SendTime, pTradingNotice->SendTime);
		tradingnotice.SequenceNo = pTradingNotice->SequenceNo;
		strcpy_s(tradingnotice.FieldContent, pTradingNotice->FieldContent);
		strcpy_s(tradingnotice.InvestUnitID, pTradingNotice->InvestUnitID);
		tradingnotice_vector.push_back(tradingnotice);
		if (bIsLast)
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspQryTradingNotice, (LPARAM)&tradingnotice_vector);
		}
	}
}

void TradeSpi::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		string msg_str = string(pRspInfo->ErrorMsg);
		CString msg = msg_str.c_str();
		::MessageBox(NULL, msg, _T("预埋出错"), MB_OK);//调试用
		return;
	}
	static ParkedOrder order;
	if (bIsLast && pParkedOrder!=NULL)
	{
		strcpy_s(order.BrokerID, pParkedOrder->BrokerID);
		strcpy_s(order.InvestorID, pParkedOrder->InvestorID);
		strcpy_s(order.InstrumentID, pParkedOrder->InstrumentID);
		strcpy_s(order.OrderRef, pParkedOrder->OrderRef);
		strcpy_s(order.UserID, pParkedOrder->UserID);
		order.OrderPriceType = pParkedOrder->OrderPriceType;
		order.Direction = pParkedOrder->Direction;
		strcpy_s(order.CombOffsetFlag, pParkedOrder->CombOffsetFlag);
		strcpy_s(order.CombHedgeFlag, pParkedOrder->CombHedgeFlag);
		order.LimitPrice = pParkedOrder->LimitPrice;
		order.VolumeTotalOriginal = pParkedOrder->VolumeTotalOriginal;
		order.TimeCondition = pParkedOrder->TimeCondition;
		strcpy_s(order.GTDDate, pParkedOrder->GTDDate);
		order.VolumeCondition = pParkedOrder->VolumeCondition;
		order.MinVolume = pParkedOrder->MinVolume;
		order.ContingentCondition = pParkedOrder->ContingentCondition;
		order.StopPrice = pParkedOrder->StopPrice;
		order.ForceCloseReason = pParkedOrder->ForceCloseReason;
		order.IsAutoSuspend = pParkedOrder->IsAutoSuspend;
		strcpy_s(order.BusinessUnit, pParkedOrder->BusinessUnit);
		order.RequestID = pParkedOrder->RequestID;
		strcpy_s(order.ExchangeID, pParkedOrder->ExchangeID);
		strcpy_s(order.ClientID, pParkedOrder->ClientID);
		order.UserForceClose = pParkedOrder->UserForceClose;
		strcpy_s(order.InvestUnitID, pParkedOrder->InvestUnitID);
		strcpy_s(order.AccountID, pParkedOrder->AccountID);
		strcpy_s(order.CurrencyID, pParkedOrder->CurrencyID);
		strcpy_s(order.IPAddress, pParkedOrder->IPAddress);
		strcpy_s(order.MacAddress, pParkedOrder->MacAddress);
		strcpy_s(order.ParkedOrderID, pParkedOrder->ParkedOrderID);
		order.UserType = pParkedOrder->UserType;
		order.Status = pParkedOrder->Status;
		order.ErrorID = pParkedOrder->ErrorID;
		strcpy_s(order.ErrorMsg, pParkedOrder->ErrorMsg);
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RspParkedOrderInsert, (LPARAM)&order);
	}
}

void TradeSpi::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		string msg_str = string(pRspInfo->ErrorMsg);
		CString msg = msg_str.c_str();
		::MessageBox(NULL, msg, _T("预埋撤单出错"), MB_OK);//调试用
		return;
	}
	if (pRemoveParkedOrder!=NULL)
	{
		static Remove_ParkedOrder  order;
		strcpy_s(order.BrokerID, pRemoveParkedOrder->BrokerID);
		strcpy_s(order.InvestorID, pRemoveParkedOrder->InvestorID);
		strcpy_s(order.InvestUnitID, pRemoveParkedOrder->InvestUnitID);
		strcpy_s(order.ParkedOrderID, pRemoveParkedOrder->ParkedOrderID);
		if (bIsLast)
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspRemoveParkedOrder, (LPARAM)&order);
		}
	}
}

void TradeSpi::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		string msg_str = string(pRspInfo->ErrorMsg);
		CString msg = msg_str.c_str();
		::MessageBox(NULL, msg, _T("预埋撤单出错"), MB_OK);//调试用
		return;
	}
	static ParkedOrderAction order;
	if (pParkedOrderAction != NULL)
	{
		if (bIsLast)
		{
			strcpy_s(order.BrokerID, pParkedOrderAction->BrokerID);
			strcpy_s(order.InvestorID, pParkedOrderAction->InvestorID);
			order.OrderActionRef = pParkedOrderAction->OrderActionRef;
			strcpy_s(order.OrderRef, pParkedOrderAction->OrderRef);
			order.RequestID = pParkedOrderAction->RequestID;
			order.FrontID = pParkedOrderAction->FrontID;
			order.SessionID = pParkedOrderAction->SessionID;
			strcpy_s(order.ExchangeID, pParkedOrderAction->ExchangeID);
			strcpy_s(order.OrderSysID, pParkedOrderAction->OrderSysID);
			order.ActionFlag = pParkedOrderAction->ActionFlag;
			order.LimitPrice = pParkedOrderAction->LimitPrice;
			order.VolumeChange = pParkedOrderAction->VolumeChange;
			strcpy_s(order.UserID, pParkedOrderAction->UserID);
			strcpy_s(order.InstrumentID, pParkedOrderAction->InstrumentID);
			strcpy_s(order.ParkedOrderActionID, pParkedOrderAction->ParkedOrderActionID);
			order.UserType = pParkedOrderAction->UserType;
			order.Status = pParkedOrderAction->Status;
			order.ErrorID = pParkedOrderAction->ErrorID;
			strcpy_s(order.ErrorMsg, pParkedOrderAction->ErrorMsg);
			strcpy_s(order.InvestUnitID, pParkedOrderAction->InvestUnitID);
			strcpy_s(order.IPAddress, pParkedOrderAction->IPAddress);
			strcpy_s(order.MacAddress, pParkedOrderAction->MacAddress);
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspParkedOrderAction, (LPARAM)&order);
			return;
		}
	}
}

void TradeSpi::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (IsErrorRspInfo(pRspInfo))
	{
		string msg_str = string(pRspInfo->ErrorMsg);
		CString msg = msg_str.c_str();
		::MessageBox(NULL, msg, _T("出错"), MB_OK);//调试用
		return;
	}
	if (pRemoveParkedOrderAction!=NULL)
	{
		static Remove_ParkedOrderAction  order;
		strcpy_s(order.BrokerID, pRemoveParkedOrderAction->BrokerID);
		strcpy_s(order.InvestorID, pRemoveParkedOrderAction->InvestorID);
		strcpy_s(order.InvestUnitID, pRemoveParkedOrderAction->InvestUnitID);
		strcpy_s(order.ParkedOrderActionID, pRemoveParkedOrderAction->ParkedOrderActionID);
		if (bIsLast)
		{
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspRemoveParkedOrderAction, (LPARAM)&order);
		}
	}
}


void TradeSpi::Init(const ServerInfo& info)//在前置连接前的准备工作
{
	if (m_bInited)
	{
		return;
	}
	m_BrokerInfo = info;//将经纪商信息保存进该类的受保护成员m_BrokerInfo中
	if (NULL == m_pTradeApi)
	{
		m_pTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi(info.name.c_str()); //根据不同的serverinfo生成有差别的流文件，这很重要，
	}
	m_pTradeApi->RegisterSpi(this);
	//订阅流的模式颇为重要
	m_pTradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);//注册私有流
	m_pTradeApi->SubscribePublicTopic(THOST_TERT_QUICK);//注册公有流
	char trade_front[128];
	sprintf_s(trade_front, "%s%s", m_BrokerInfo.protocol.c_str(), 
	m_BrokerInfo.trade_server_front[0].c_str());//假设该经纪商下有多个交易服务器配置，此处仅选择第一个交易服务器连接
	m_pTradeApi->RegisterFront(trade_front);//注册前端交易服务器地址
	m_bInited = true;
}

bool TradeSpi::Login(const string& username, const string& password)
{
	if (!m_bInited || username.empty() || password.empty() )
	{
		return false;
	}
	this->username = username;
	this->password = password;
	m_pTradeApi->Init();//申请前置连接
	return true;
}

bool TradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo)
{//检查回调是否返回错误，以此明确主动函数的操作是否成功,为通用型函数
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID !=0));//回调响应返回的错误信息结构体pRspInfo不为空且存在错误,即有错误号
	return bResult;
}

void TradeSpi::SetLastError(CThostFtdcRspInfoField *pRspInfo)
{
	if (NULL == pRspInfo)//若pRspInfo错误信息体为空,说明没有错误
	{
		return;
	}
	last_error.ErrorID = pRspInfo->ErrorID;
	last_error.ErrorMsg = pRspInfo->ErrorMsg;
	get_CtpMsgDistributor()->OnResponse(ctp_msg::RspError, (LPARAM)&last_error);//将错误传到上层去
}









