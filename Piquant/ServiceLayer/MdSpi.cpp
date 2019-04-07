#include "stdafx.h"
#include "MdSpi.h"
#include "CtpService.h"
#include "CtpMsgDistributor.h"
#include <iostream>
#include <atlstr.h>//使用cstring需要该头文件
#include "CtpMsg.h"
using namespace std;

#pragma warning(disable : 4996)

CMdSpi::CMdSpi()
	:m_iReqestID(0),
	m_bInited(false),
	m_pMarketApi(NULL)
{
}

CMdSpi::~CMdSpi()
{
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}

//******************************************************回调虚函数部分*****************************************************************//
void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast)
{
	//
}

void CMdSpi::OnFrontDisconnected(int nReason)
{//一旦断开连接，将该消息发送给应用层处理
	get_CtpMsgDistributor()->OnResponse(ctp_msg::RspFrontDisconnected_Md, (LPARAM)NULL);
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	//cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{//前置连接成功后主动登录
	ReqUserLogin();
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//告诉应用层登录成功
		static UserInfo user_info_md; //注意这里的user_info必须是static静态的，因为若是局部变量，在函数执行完后会被销毁，而我们的get_service_spi()->OnResponse是将其地址传给应用层的，若被销毁了，则界面显示时会是一堆乱码
		strcpy_s(user_info_md.UserID, pRspUserLogin->UserID);
		strcpy_s(user_info_md.BrokerID, pRspUserLogin->BrokerID);
		strcpy_s(user_info_md.TradingDay, pRspUserLogin->TradingDay);
		strcpy_s(user_info_md.LoginTime, pRspUserLogin->LoginTime);
		strcpy_s(user_info_md.ExchangeTime, pRspUserLogin->SHFETime);
		user_info_md.FrontID = pRspUserLogin->FrontID;
		user_info_md.SessionID = pRspUserLogin->SessionID;
		strcpy_s(user_info_md.MaxOrderRef, pRspUserLogin->MaxOrderRef);
		strcpy_s(user_info_md.SystemName, pRspUserLogin->SystemName);
		strcpy_s(user_info_md.UserID, username.c_str());
		string systemname = "MarketHosting";
		strcpy_s(user_info_md.SystemName, systemname.c_str());
		if (bIsLast)
		{//将登录成功的消息告知应用层
			get_CtpMsgDistributor()->OnResponse(ctp_msg::RspUserLogined_Md, (LPARAM)&user_info_md);
		}
		//理论上，应在登录成功后，开始订阅行情，订阅行情的操作放置到应用层UI了
	}
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	static MarketData marketData;
	memset(&marketData, 0 ,sizeof(marketData));
	if (pDepthMarketData!=NULL)
	{
		strcpy_s(marketData.TradingDay, pDepthMarketData->TradingDay);
		strcpy_s(marketData.code, pDepthMarketData->InstrumentID);
		strcpy_s(marketData.ExchangeID, pDepthMarketData->ExchangeID);
		marketData.LastPrice = pDepthMarketData->LastPrice;
		marketData.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
		marketData.PreClosePrice = pDepthMarketData->PreClosePrice;
		marketData.PreOpenInterest = pDepthMarketData->PreOpenInterest;
		marketData.OpenPrice = pDepthMarketData->OpenPrice;
		marketData.HighestPrice = pDepthMarketData->HighestPrice;
		marketData.LowestPrice = pDepthMarketData->LowestPrice;
		marketData.Volume = pDepthMarketData->Volume;
		marketData.Turnover = pDepthMarketData->Turnover;
		marketData.OpenInterest = pDepthMarketData->OpenInterest;
		marketData.ClosePrice = pDepthMarketData->ClosePrice;
		marketData.SettlementPrice = pDepthMarketData->SettlementPrice;
		marketData.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
		marketData.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
		strcpy_s(marketData.UpdateTime, pDepthMarketData->UpdateTime);
		marketData.UpdateMillisec = pDepthMarketData->UpdateMillisec;
		marketData.AveragePrice = pDepthMarketData->AveragePrice;
		marketData.BidPrice = pDepthMarketData->BidPrice1;
		marketData.BidVolume = pDepthMarketData->BidVolume1;
		marketData.AskPrice = pDepthMarketData->AskPrice1;
		marketData.AskVolume = pDepthMarketData->AskVolume1;
		get_CtpMsgDistributor()->OnResponse(ctp_msg::RtnMarketData, (LPARAM)&marketData);
	}
}

void CMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//::MessageBox(NULL, _T("行情登出"), _T("登出"), MB_OK);//调试用
}

void CMdSpi::Init( const ServerInfo& info )
{
	if (m_bInited)
	{//已连接
		return ;
	}
	m_BrokerInfo=info;	//缓存经纪商信息
	if (NULL==m_pMarketApi)
	{//创建Api对象
		m_pMarketApi=CThostFtdcMdApi::CreateFtdcMdApi(info.name.c_str());//注意con流文件的名称
	}
	m_pMarketApi->RegisterSpi(this);						// 注册spi事件类
	char front_addr[128]={0};
	sprintf(front_addr,"%s%s",m_BrokerInfo.protocol.c_str(),m_BrokerInfo.market_server_front[0].c_str());
	m_pMarketApi->RegisterFront(front_addr);			
	m_bInited = true;		//m_bInited不是指前置连接成功，而是指前置连接前的准备工作已经完成
}

bool CMdSpi::Login( const string& username,const string& password )
{
	if (!m_bInited || username.empty() || password.empty())
	{
		return false;
	}
	this->username=username;		//缓存用户名
	this->password=password;		//缓存密码
	m_pMarketApi->Init();				//申请前置连接
	return true;
}

void CMdSpi::ReqUserLogin()
{
	if (m_pMarketApi == NULL)
	{
		return;
	}
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_BrokerInfo.id.c_str());
	strcpy(req.UserID, username.c_str());
	strcpy(req.Password, password.c_str());
	int iResult = m_pMarketApi->ReqUserLogin(&req, ++m_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = m_pMarketApi->ReqUserLogin(&req, ++m_iReqestID);
	}
}

void CMdSpi::SubscribeMarketData(const vector<string>& instruments)
{
	if (instruments.size() <= 0)
	{//传入需订阅的合约为空
		return;
	}
	char** ppInstrumentID = new char*[instruments.size()];	//动态申请instruments.size()个char*指针，ppInstrumentID为二级指针，是指向指针数组的指针
	for (int i = 0; i < instruments.size(); i++)
	{
		ppInstrumentID[i] = new char[9];	//指针的数组表示法，ppInstrumentID[i]为char*类型
		strcpy(ppInstrumentID[i], instruments[i].c_str());
	}
	int iResult = m_pMarketApi->SubscribeMarketData(ppInstrumentID, instruments.size());
	while (iResult != 0)
	{//直到发送订阅请求成功
		Sleep(2000);
		iResult = m_pMarketApi->SubscribeMarketData(ppInstrumentID, instruments.size());
	}
	//必须先逐个释放ppInstrumentID[i]，其为char数组，之后再释放ppInstrumentID，其为指针数组
	for (int i = 0; i < instruments.size(); i++)
	{
		delete[]ppInstrumentID[i]; //逐个释放存放合约名的char数组，为此应delete[]掉每个ppInstrumentID[i]
	}
	delete[]ppInstrumentID;		//释放指针数组内存，为此应delete[]
	ppInstrumentID = NULL;
}

void CMdSpi::UnSubscribeMarketData(const vector<string>& instruments)
{
	if (instruments.size() <= 0)
	{//传入需退订的合约为空
		return;
	}
	char** ppInstrumentID = new char*[instruments.size()];	//动态申请instruments.size()个char*指针，ppInstrumentID为二级指针，是指向指针数组的指针
	for (int i = 0; i < instruments.size(); i++)
	{
		ppInstrumentID[i] = new char[9];	//指针的数组表示法，ppInstrumentID[i]为char*类型
		strcpy(ppInstrumentID[i], instruments[i].c_str());
	}
	int iResult = m_pMarketApi->UnSubscribeMarketData(ppInstrumentID, instruments.size());
	while (iResult != 0)
	{//直到发送退订请求成功
		Sleep(2000);
		iResult = m_pMarketApi->UnSubscribeMarketData(ppInstrumentID, instruments.size());
	}
	//必须先逐个释放ppInstrumentID[i]，其为char数组，之后再释放ppInstrumentID，其为指针数组
	for (int i = 0; i < instruments.size(); i++)
	{
		delete[]ppInstrumentID[i]; //逐个释放存放合约名的char数组，为此应delete[]掉每个ppInstrumentID[i]
	}
	delete[]ppInstrumentID;		//释放指针数组内存，为此应delete[]
	ppInstrumentID = NULL;
}
