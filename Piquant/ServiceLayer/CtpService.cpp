#include "stdafx.h"
#include "CtpService.h"
#include<ThostFtdcTraderApi.h>
#include<ThostFtdcUserApiDataType.h>
#include<ThostFtdcUserApiStruct.h>
#include <string>
#include "CtpMsg.h"
using namespace std;


CtpService::CtpService()
	:T_iReqestID(0),
	pCtpTradeApi(NULL),
	pCtpMdApi(NULL)
{

}


CtpService::~CtpService()
{
	if (pCtpTradeApi != NULL)
	{
		delete pCtpTradeApi;
		pCtpTradeApi = NULL;
	}
	if (pCtpMdApi != NULL)
	{
		delete pCtpMdApi;
		pCtpMdApi = NULL;
	}
}

bool CtpService::login(const string& user_name, const string& password, const ServerInfo& bs)
{//由登录对话框调用，实现交易及行情的前置连接
	if (user_name.empty() || password.empty() || bs.id.empty() || bs.protocol.empty() || bs.market_server_front.empty() || bs.trade_server_front.empty())
	{//入参检查
		return false;
	}
	/*开始初始化前置连接*/
	if (NULL == pCtpTradeApi)
	{
		pCtpTradeApi = new TradeSpi();
	}
	pCtpTradeApi->Init(bs);	//仅创建api,注册spi，注册交易前置服务器地址而已，尚未init连接

	if (NULL == pCtpMdApi)
	{
		pCtpMdApi = new CMdSpi();
	}
	pCtpMdApi->Init(bs);
	
	this->user_name = user_name;	//记录登录账号
	this->broker = bs;					//记录经纪商信息

	pCtpTradeApi->Login(user_name, password);	//申请交易前置连接Init
	pCtpMdApi->Login(user_name, password);		//申请行情前置连接Init
	return true;
}

bool CtpService::logout()
{
	//CThostFtdcUserLogoutField req = { 0 };
	//strcpy_s(req.BrokerID, broker.id.c_str());
	//strcpy_s(req.UserID, user_name.c_str());
	if (pCtpMdApi == NULL || pCtpTradeApi == NULL)
	{//尚未登录不能登出，做异常检查
		::MessageBox(NULL, _T("尚未登录不可登出"), _T("出错"), MB_OK);
		return false;
	}
	//******************************行情API退出登录**************************************//
	//退订订阅合约先
	if (pCtpMdApi != NULL)
	{
		vector<BaseData> all_instruments = pCtpTradeApi->get_all_instruments();
		vector<string> instruments_unsubscribe;
		for (int i = 0; i < all_instruments.size(); ++i)
		{
			instruments_unsubscribe.push_back(all_instruments[i].code);//提取basedata结构体中的合约名，逐个存取
		}
		pCtpMdApi->UnSubscribeMarketData(instruments_unsubscribe);//退订合约
	}
	//pCtpMdApi->GetMdApi()->ReqUserLogout(&req, 2);	//ctp的退出登录有些问题，直接销毁对象罢了
	if (pCtpMdApi->GetMdApi() != NULL)
	{//清理行情api对象
		pCtpMdApi->GetMdApi()->RegisterSpi(NULL);
		pCtpMdApi->GetMdApi()->Release();
		CThostFtdcMdApi* pMdapi = pCtpMdApi->GetMdApi();
		pMdapi = NULL;
	}
	if (pCtpMdApi != NULL)
	{//清理行情回调spi对象
		delete pCtpMdApi;
		pCtpMdApi = NULL;
	}
	//*****************************交易API退出登录**************************************//
	//pCtpTradeApi->GetTraderApi()->ReqUserLogout(&req, 1);//此处的nRequestID无递增规则,自行维护,ctp的退出登录有些问题，直接销毁对象罢了
	if (pCtpTradeApi->GetTraderApi() != NULL)
	{//清理交易api对象
		pCtpTradeApi->GetTraderApi()->RegisterSpi(NULL);
		pCtpTradeApi->GetTraderApi()->Release();
		CThostFtdcTraderApi* pTdapi = pCtpTradeApi->GetTraderApi();
		pTdapi = NULL;
	}
	if (pCtpTradeApi != NULL)
	{//清理行情回调spi对象
		delete pCtpTradeApi;
		pCtpTradeApi = NULL;
	}
	return(true);
}

bool CtpService::get_basic_data(vector<BaseData>& base_data)
{
	base_data.clear();
	if (NULL==pCtpTradeApi)
	{
		return false;
	}
	base_data = pCtpTradeApi->get_all_instruments();  //通过交易api对象的外部公有接口get_all_instruments()来获取其成员变量all_instruments，即合约信息集合
	return true;
}

bool CtpService::get_exchanges(vector<Exchange>& exchanges)
{
	exchanges.clear();
	if (NULL == pCtpTradeApi)
	{
		return false;
	}
	exchanges = pCtpTradeApi->get_all_exchanges();  //通过交易api对象的外部公有接口get_all_exchanges()来获取其成员变量all_exchanges，即交易所信息集合
	return true;
}

bool CtpService::get_all_InvestorPositionDetails(vector<InvestorPositionDetail>& positiondetails)
{
	positiondetails.clear();
	if (NULL == pCtpTradeApi)
	{
		return false;
	}
	positiondetails = pCtpTradeApi->get_all_InvestorPositionDetails();
	return true;
}

std::string CtpService::get_username()
{
	return user_name;
}

ServerInfo CtpService::get_brokerinfo()
{
	return broker;
}

bool CtpService::islogined()
{
	if (pCtpMdApi != NULL && pCtpTradeApi !=NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CtpService::request(int request_id, void* param)
{//由应用层使用
	bool bRet = false;
	switch (request_id)
	{
		case QryTradingAccount:
		{
			TradeAccountParam* pTradeAccount = reinterpret_cast<TradeAccountParam*>(param);
			if (NULL == pTradeAccount)	return false; //检查转换是否成功,若解引NULL或会崩溃
			bRet = query_account(*pTradeAccount);
			break;
		}
		case QryMarketData:
		{
			vector<string> * pInstruments = reinterpret_cast<vector<string>*>(param);
			if (NULL == pInstruments)	return false;
			bRet = query_market_data(*pInstruments);	
			break;
		}
		case QryInstruments:
		{
			vector<string> * pInstruments = reinterpret_cast<vector<string>*>(param);
			if (NULL == pInstruments)	return false;
			bRet = query_instruments(*pInstruments, true);	//默认订阅所有行情,可通过修改query_instruments的第二个参数实现部分订阅
			break;
		}
		case ctp_msg::MakeOrder:
		{
			InputOrderParam* pInputOrderParam = reinterpret_cast<InputOrderParam*>(param);
			if (NULL == pInputOrderParam)	return false;
			bRet = make_order(*pInputOrderParam);
			break;
		}
		case ctp_msg::Make_ParkedOrder:
		{
			InputParkedOrderParam* pInputParkedOrderParam = reinterpret_cast<InputParkedOrderParam*>(param);
			if (NULL == pInputParkedOrderParam)	return false;
			bRet = make_parkedorder(*pInputParkedOrderParam);
			break;
		}
		case ctp_msg::QryPosition:
		{
			PositionParam* pPositionParam = reinterpret_cast<PositionParam*>(param);
			if (NULL == pPositionParam)	return false;
			bRet = query_position(*pPositionParam);
			break;
		}
		case ctp_msg::QryCommissionRate:
		{
			InstrumentCommissionRateParam* pInstrumentCommissionRate = reinterpret_cast<InstrumentCommissionRateParam*>(param);
			if (NULL == pInstrumentCommissionRate)	return false;
			bRet = query_CommissionRate(*pInstrumentCommissionRate);
			break;
		}
		case ctp_msg::QryOrder:
		{
			QryOrderParam* pQryOrder = reinterpret_cast<QryOrderParam*>(param);
			if (NULL == pQryOrder)	return false;
			bRet = query_Order(*pQryOrder);
			break;
		}
		case ctp_msg::CancelOrder:
		{
			ActionOrderParam* pCancelOrder = reinterpret_cast<ActionOrderParam*>(param);
			if (NULL == pCancelOrder)	return false;
			bRet = cancel_order(*pCancelOrder);
			break;
		}
		case ctp_msg::QryTrade:
		{
			TradeParam* pTrade = reinterpret_cast<TradeParam*>(param);
			if (NULL == pTrade)	return false;
			bRet = query_Trade(*pTrade);
			break;
		}
		case ctp_msg::QryMarginRate:
		{
			QryInstrumentMarginRateParam* pMarginRate = reinterpret_cast<QryInstrumentMarginRateParam*>(param);
			if (NULL == pMarginRate)	return false;
			bRet = query_MarginRate(*pMarginRate);
			break;
		}
		case ctp_msg::QryExchangeRate:
		{
			QryExchangeRateParam* pExchangeRate = reinterpret_cast<QryExchangeRateParam*>(param);
			if (NULL == pExchangeRate)	return false;
			bRet = query_ExchangeRate(*pExchangeRate);
			break;
		}
		case ctp_msg::QryInstrumentOrderCommRate:
		{
			QryInstrumentOrderCommRateParam* pOrderCommRate = reinterpret_cast<QryInstrumentOrderCommRateParam*>(param);
			if (NULL == pOrderCommRate)	return false;
			bRet = query_InstrumentOrderCommRate(*pOrderCommRate);
			break;
		}
		case ctp_msg::QryProduct:
		{
			QryProductParam* pProduct = reinterpret_cast<QryProductParam*>(param);
			if (NULL == pProduct)	return false;
			bRet = query_Product(*pProduct);
			break;
		}
		case ctp_msg::QryDepthMarketData:
		{
			QryDepthMarketDataＰaram* pDepthMarketData = reinterpret_cast<QryDepthMarketDataＰaram*>(param);
			if (NULL == pDepthMarketData)	return false;
			bRet = query_DepthMarketData(*pDepthMarketData);
			break;
		}
		case ctp_msg::QryInvestorPositionDetail:
		{
			QryInvestorPositionDetailParam* pInvestorPositionDetail = reinterpret_cast<QryInvestorPositionDetailParam*>(param);
			if (NULL == pInvestorPositionDetail)	return false;
			bRet = query_InvestorPositionDetail(*pInvestorPositionDetail);
			break;
		}
		case ctp_msg::BatchOrderAction:
		{
			BatchOrderActionParam* pBatchOrderActionParam = reinterpret_cast<BatchOrderActionParam*>(param);
			if (NULL == pBatchOrderActionParam)	return false;
			bRet = batchorder_action(*pBatchOrderActionParam);
			break;
		}
		case ctp_msg::RspQryNotice:
		{
			QryNoticeParam* pQryNotice = reinterpret_cast<QryNoticeParam*>(param);
			if (NULL == pQryNotice)	return false;
			bRet = query_notice(*pQryNotice);
			break;
		}
		case ctp_msg::RspQryTradingNotice:
		{
			QryTradingNoticeParam* pQryTradingNotice = reinterpret_cast<QryTradingNoticeParam*>(param);
			if (NULL == pQryTradingNotice)	return false;
			bRet = query_tradingnotice(*pQryTradingNotice);
			break;
		}
		case ctp_msg::QryParkedOrder:
		{
			QryParkedOrderParam* pQryParkedOrder = reinterpret_cast<QryParkedOrderParam*>(param);
			if (NULL == pQryParkedOrder)	return false;
			bRet = query_parkedorder(*pQryParkedOrder);
			break;
		}
		case ctp_msg::RemoveParkedOrder:
		{
			RemoveParkedOrderParam* pRemoveParkedOrder = reinterpret_cast<RemoveParkedOrderParam*>(param);
			if (NULL == pRemoveParkedOrder)	return false;
			bRet = remove_parkedorder(*pRemoveParkedOrder);
			break;
		}
		case ctp_msg::Cancel_ParkedOrder:
		{
			ParkedOrderActionParam* pParkedOrderAction = reinterpret_cast<ParkedOrderActionParam*>(param);
			if (NULL == pParkedOrderAction)	return false;
			bRet = cancel_parkedorder(*pParkedOrderAction);
			break;
		}
		case ctp_msg::QryParkedOrderAction :
		{
			QryParkedOrderActionParam* pQryParkedOrderAction = reinterpret_cast<QryParkedOrderActionParam*>(param);
			if (NULL == pQryParkedOrderAction)	return false;
			bRet = query_parkedorderaction(*pQryParkedOrderAction);
			break;
		}
		case ctp_msg::RemoveParkedOrderAction:
		{
			RemoveParkedOrderActionParam* pRemoveParkedOrderAction = reinterpret_cast<RemoveParkedOrderActionParam*>(param);
			if (NULL == pRemoveParkedOrderAction)	return false;
			bRet = remove_parkedorderaction(*pRemoveParkedOrderAction);
			break;
		}
		default:
		{
			break;
		}
	}
return bRet;
}

/*
	pCtpTradeApi == NULL说明封装后的交易api对象尚未创建
	pCtpTradeApi->GetTraderApi() == NULL说明CThostFtdcTraderApi类型对象尚未创建
*/
bool CtpService::query_account(TradeAccountParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryTradingAccountField req = { 0 };
	if (strlen(param.BrokerID) != 0)	strcpy_s(req.BrokerID, param.BrokerID);
	else                                          strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID) != 0)  strcpy_s(req.InvestorID, param.InvestorID);
	else                                              strcpy_s(req.InvestorID, user_name.c_str());
	if (strlen(param.CurrencyID) != 0) strcpy_s(req.CurrencyID, param.CurrencyID);
	else                                                strcpy_s(req.CurrencyID, "CNY");
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryTradingAccount(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(1000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryTradingAccount(&req, ++T_iReqestID);
		//::MessageBox(NULL, _T("流控限制,等待续查"), _T("查账户"), MB_OK);//调试用
	}
	return true;
}

bool CtpService::query_instruments(vector<string>& instrument_vector, bool qry_all)
{
	if (pCtpTradeApi==NULL || pCtpTradeApi->GetTraderApi()==NULL)
	{
		return false;
	}
	CThostFtdcQryInstrumentField  instrument_field = { 0 };
	vector<string> Instrument_Exchange_strs;//用于存储切割后的子字符串,分别为合约名和对应交易所名
	if (qry_all == true)
	{//若instrument_field为空,则默认查询所有
		Sleep(1000);//ctp查询每秒一次
		int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrument(&instrument_field, ++T_iReqestID);
		while (ret_code != 0)
		{
			Sleep(1000);
			ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrument(&instrument_field, ++T_iReqestID);
		}
		return true;
	}
	for (int i = 0; i < instrument_vector.size(); ++i)
	{
		memset(&instrument_field, 0, sizeof(instrument_field));
		Instrument_Exchange_strs.clear();//每次新的查询申请前,务必先行清空

		char * strs = new char[instrument_vector[i].length() + 1];
		strcpy(strs, instrument_vector[i].c_str());
		char * delim = "/";
		char *p = strtok(strs, delim);
		while (p) {
			string s = p;			//分割得到的字符串并转换为string类型
			Instrument_Exchange_strs.push_back(s); //存入结果数组
			p = strtok(NULL, delim);
		}

		strcpy_s(instrument_field.InstrumentID, Instrument_Exchange_strs[0].c_str());	//传合约名
		strcpy_s(instrument_field.ExchangeID, Instrument_Exchange_strs[1].c_str());		//传对应交易所名
		int ret_code=pCtpTradeApi->GetTraderApi()->ReqQryInstrument(&instrument_field, ++T_iReqestID);
		while (ret_code !=0)
		{
			Sleep(1000);
			ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrument(&instrument_field, ++T_iReqestID);
		}
	}
	return true;
}

bool CtpService::query_exchanges(const string& eid)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	//查询所有交易所信息
	CThostFtdcQryExchangeField  req = { 0 };//在ctp约定中，若传空结构体进去则默认查询所有
	strcpy_s(req.ExchangeID, eid.c_str());
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryExchange(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(1000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryExchange(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_position(PositionParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	if (strlen(param.BrokerID) !=0)	strcpy_s(req.BrokerID, param.BrokerID);
	else                                          strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID) != 0)  strcpy_s(req.InvestorID, param.InvestorID);
	else                                              strcpy_s(req.InvestorID, user_name.c_str());
	if (strlen(param.InstrumentID) != 0)		strcpy_s(req.InstrumentID, param.InstrumentID);
	else                                                      strcpy_s(req.InstrumentID, "");//不填写合约则返回所有持仓
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInvestorPosition(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(1000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInvestorPosition(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_CommissionRate(InstrumentCommissionRateParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryInstrumentCommissionRateField a = { 0 };
	if (strlen(param.BrokerID) != 0)		strcpy(a.BrokerID, param.BrokerID);
	else                                              strcpy(a.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID) != 0)	strcpy(a.InvestorID, param.InvestorID);
	else                                              strcpy(a.InvestorID, user_name.c_str());
	if (strlen(param.InstrumentID) != 0)	strcpy(a.InstrumentID, param.InstrumentID);
	else                                                  strcpy(a.InstrumentID, "");//如果InstrumentID填空，则返回持仓对应的合约手续费率。
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentCommissionRate(&a, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(2000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentCommissionRate(&a, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_Order(QryOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryOrderField req = {0};
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)	strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else														strcpy_s(req.InvestorID, user_name.c_str());
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryOrder(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(2000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryOrder(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::cancel_order(ActionOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcInputOrderActionField req = { 0 };
	if (strlen(param.BrokerID) != 0)		strcpy_s(req.BrokerID, param.BrokerID);
	else												strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID) != 0)	strcpy_s(req.InvestorID, param.InvestorID);
	else												strcpy_s(req.InvestorID, user_name.c_str());
	if (strlen(param.UserID) != 0)	strcpy_s(req.UserID, param.UserID);
	else											strcpy_s(req.UserID, user_name.c_str());
	strcpy_s(req.OrderSysID, param.OrderSysID);  //对应要撤单的OrderSysID
	strcpy_s(req.ExchangeID, param.ExchangeID);
	strcpy_s(req.InstrumentID, param.InstrumentID);
	req.ActionFlag = THOST_FTDC_AF_Delete; //指明撤单
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqOrderAction(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(2000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqOrderAction(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_Trade(TradeParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryTradeField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryTrade(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(1000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryTrade(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_MarginRate(QryInstrumentMarginRateParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryInstrumentMarginRateField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	if (strlen(param.InstrumentID.c_str()) != 0)	strcpy(req.InstrumentID, param.InstrumentID.c_str());
	else                                                  strcpy(req.InstrumentID, "");//如果InstrumentID填空，则返回持仓对应的保证金率
	req.HedgeFlag=param.HedgeFlag;
	int ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentMarginRate(&req, ++T_iReqestID);
	while (ret_code != 0)
	{
		Sleep(1000);
		ret_code = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentMarginRate(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_market_data(vector<string>& instrumentid /*= vector<string>()*/)
{
	//1.可以主动向交易所请求查询行情数据，但很可能当我们接收到数据时，行情数据已经发生了更新
	//int ret_code = pCtpApi->GetTraderApi()->ReqQryDepthMarketData(&a, ++T_iReqestID);
	//2.最好方法是通过行情api来订阅数据
	if (pCtpMdApi == NULL)
	{
		return false;
	}
	pCtpMdApi->SubscribeMarketData(instrumentid);
	return true;
}

bool CtpService::make_order(InputOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}

	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	//相关用户信息
	if (param.BrokerID.empty())	param.BrokerID = this->pCtpTradeApi->get_brokerinfo().id;
	strcpy_s(req.BrokerID, param.BrokerID.c_str());
	if (param.InvestorID.empty())	param.InvestorID = this->pCtpTradeApi->get_username();
	strcpy_s(req.InvestorID, param.InvestorID.c_str());
	strcpy_s(req.UserID, param.InvestorID.c_str());
	//相关合约信息
	strcpy_s(req.ExchangeID, param.ExchangeID.c_str());
	strcpy_s(req.InstrumentID, param.InstrumentID.c_str());
	//下单信息
	req.Direction = param.Direction;	//开仓方向
	req.CombOffsetFlag[0] = param.CombOffsetFlag[0];	 //开仓类型，是开仓/平仓还是平今
	req.VolumeTotalOriginal = param.VolumeTotalOriginal; //开仓数量
	req.MinVolume = 1; //最小成交量默认是1
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		 //组合投机套保标志, 默认就选投机吧
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;  //强平原因: 默认非强平
	req.IsAutoSuspend = 0; //自动挂起标志: 默认为否
	req.UserForceClose = 0;  //用户强评标志: 默认为否
	//必须实现报单引用的递增, 实现订单的本地唯一性管理
	int cur_order_ref = atoi(this->pCtpTradeApi->get_userinfo().MaxOrderRef); //从登录回报信息结构体中获得最大报单引用
	cur_order_ref++;
	if (param.OrderRef=="")
	{//若OrderRef为空,说明是手动下单的
		sprintf(req.OrderRef, "%d", cur_order_ref);	//将递增的报单引用填进req.OrderRef中去
	}
	else
	{//若不为空,说明是策略接口下单的
		sprintf(req.OrderRef, "%s_%d", param.OrderRef, cur_order_ref);
	}
	strcpy_s(this->pCtpTradeApi->get_userinfo().MaxOrderRef, to_string(cur_order_ref).c_str());	//修改登录信息结构体中的最大报单引用,  实现下次使用时确保是递增过的
	if (param.OrderPriceType==THOST_FTDC_OPT_LimitPrice)	 
	{//四大交易所都支持限价单，但不都支持市价单，为此建议全部填限价THOST_FTDC_OPT_LimitPrice
		//限价
		if (param.ContingentCondition == THOST_FTDC_CC_Immediately)//为立即限价单
		{
			req.ContingentCondition = THOST_FTDC_CC_Immediately; //立即触发;
			req.TimeCondition = THOST_FTDC_TC_GFD;//当日有效
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			req.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量
			req.LimitPrice = param.LimitPrice;
		}
		else //非立即限价单，即限价触价单
		{
			req.ContingentCondition = param.ContingentCondition;
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			req.TimeCondition = param.TimeCondition;
			req.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量
			req.LimitPrice = param.LimitPrice; //报单价格，即是触发条件生效后，以怎样的价格成交
			req.StopPrice = param.StopPrice; //此为触发价格
		}
	}
	if (param.OrderPriceType == THOST_FTDC_OPT_AnyPrice)
	{
		//市价;
		if (param.ContingentCondition == THOST_FTDC_CC_Immediately) 
		{//为立即市价单
			req.ContingentCondition = THOST_FTDC_CC_Immediately;		//立即触发;
			req.TimeCondition = THOST_FTDC_TC_IOC;								//立即完成，否则撤销, 符合市价单特点
			req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			req.VolumeCondition = THOST_FTDC_VC_AV;							//任意数量
			req.LimitPrice = 0;																	//市价单, 价格填0即可
		}
		else //非立即市价单，即触价后转市价的触价单
		{
			req.ContingentCondition = param.ContingentCondition;
			req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			req.TimeCondition = param.TimeCondition;
			req.VolumeCondition = THOST_FTDC_VC_AV;							//任意数量
			req.LimitPrice = 0;																	//报单价格为市价，填0即可
			req.StopPrice = param.StopPrice;												//此为触发价格
		}
	}
	//req.StopPrice = 0; //止损价设定
	int iResult = pCtpTradeApi->GetTraderApi()->ReqOrderInsert(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(2000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqOrderInsert(&req, ++T_iReqestID);
	}
	return true;
}


bool CtpService::query_ExchangeRate(QryExchangeRateParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryExchangeRateField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	strcpy_s(req.FromCurrencyID, param.FromCurrencyID.c_str());
	strcpy_s(req.ToCurrencyID, param.ToCurrencyID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryExchangeRate(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryExchangeRate(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_InstrumentOrderCommRate(QryInstrumentOrderCommRateParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryInstrumentOrderCommRateField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	strcpy_s(req.InstrumentID, param.InstrumentID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentOrderCommRate(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryInstrumentOrderCommRate(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_Product(QryProductParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryProductField req = { 0 };
	strcpy_s(req.ProductID, param.ProductID.c_str());
	req.ProductClass = THOST_FTDC_PC_Futures;
	strcpy_s(req.ExchangeID, param.ExchangeID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryProduct(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryProduct(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_DepthMarketData(QryDepthMarketDataＰaram& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryDepthMarketDataField req = { 0 };
	strcpy_s(req.InstrumentID, param.InstrumentID.c_str());
	strcpy_s(req.ExchangeID, param.ExchangeID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryDepthMarketData(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryDepthMarketData(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_InvestorPositionDetail(QryInvestorPositionDetailParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryInvestorPositionDetailField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryInvestorPositionDetail(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryInvestorPositionDetail(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::batchorder_action(BatchOrderActionParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcInputBatchOrderActionField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqBatchOrderAction(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqBatchOrderAction(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_notice(QryNoticeParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryNoticeField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryNotice(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryNotice(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_tradingnotice(QryTradingNoticeParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryTradingNoticeField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryTradingNotice(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryTradingNotice(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::make_parkedorder(InputParkedOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}

	CThostFtdcParkedOrderField req;
	memset(&req, 0, sizeof(req));
	//相关用户信息
	if (param.BrokerID.empty())	param.BrokerID = this->pCtpTradeApi->get_brokerinfo().id;
	strcpy_s(req.BrokerID, param.BrokerID.c_str());
	if (param.InvestorID.empty())	param.InvestorID = this->pCtpTradeApi->get_username();
	strcpy_s(req.InvestorID, param.InvestorID.c_str());
	strcpy_s(req.UserID, param.InvestorID.c_str());
	//相关合约信息
	strcpy_s(req.ExchangeID, param.ExchangeID.c_str());
	strcpy_s(req.InstrumentID, param.InstrumentID.c_str());
	//下单信息
	req.Direction = param.Direction;	//开仓方向
	req.CombOffsetFlag[0] = param.CombOffsetFlag[0];	 //开仓类型，是开仓/平仓还是平今
	req.VolumeTotalOriginal = param.VolumeTotalOriginal; //开仓数量
	req.MinVolume = 1; //最小成交量默认是1
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;		 //组合投机套保标志, 默认投机
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;  //强平原因: 默认非强平
	req.IsAutoSuspend = 0;   //自动挂起标志: 默认为否
	req.UserForceClose = 0;  //用户强评标志: 默认为否
	//必须实现报单引用的递增, 但是并未实现订单的本地唯一性管理
	int cur_order_ref = atoi(this->pCtpTradeApi->get_userinfo().MaxOrderRef); //从登录回报信息结构体中获得最大报单引用
	cur_order_ref++;
	sprintf(req.OrderRef, "%d", cur_order_ref);	//将递增的报单引用填进req.OrderRef中去
	strcpy_s(this->pCtpTradeApi->get_userinfo().MaxOrderRef, req.OrderRef);	//修改登录信息结构体中的最大报单引用,  实现下次使用时确保是递增过的,当然重新连接会话后MaxOrderRef会被重置为0
	if (param.OrderPriceType == THOST_FTDC_OPT_LimitPrice)	 
	{//四大交易所都支持限价单，但不都支持市价单，为此建议全部填限价THOST_FTDC_OPT_LimitPrice
		//限价
		if (param.ContingentCondition == THOST_FTDC_CC_Immediately)
		{//为立即限价单
			req.ContingentCondition = THOST_FTDC_CC_Immediately; //立即触发;
			req.TimeCondition = THOST_FTDC_TC_GFD;//当日有效
			strcpy_s(req.GTDDate, "");
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			req.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量
			req.LimitPrice = param.LimitPrice;
		}
		else //非立即限价单
		{
			req.ContingentCondition = param.ContingentCondition;
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			req.TimeCondition = param.TimeCondition;	//ctp目前只支持当日有效,为此UI应用层此处默认是THOST_FTDC_TC_GFD
			strcpy(req.GTDDate, param.GTDDate.c_str());
			req.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量
			req.LimitPrice = param.LimitPrice; //报单价格，即是触发条件生效后，以怎样的价格成交
			req.StopPrice = param.StopPrice; //此为触发价格
		}
	}
	if (param.OrderPriceType == THOST_FTDC_OPT_AnyPrice)
	{
		//市价;
		if (param.ContingentCondition == THOST_FTDC_CC_Immediately) 
		{//为立即市价单
			req.ContingentCondition = THOST_FTDC_CC_Immediately;	//立即触发;
			req.TimeCondition = THOST_FTDC_TC_IOC;							//立即完成，否则撤销, 符合市价单特点
			strcpy_s(req.GTDDate, "");
			req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			req.VolumeCondition = THOST_FTDC_VC_AV;						//任意数量
			req.LimitPrice = 0;																//市价单, 价格填0即可
		}
		else //非立即市价单
		{
			req.ContingentCondition = param.ContingentCondition;
			req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
			req.TimeCondition = param.TimeCondition;
			strcpy(req.GTDDate, param.GTDDate.c_str());
			req.VolumeCondition = THOST_FTDC_VC_AV;			//任意数量
			req.LimitPrice = 0;													//报单价格为市价，填0即可
			req.StopPrice = param.StopPrice;								//此为触发价格
		}
	}
	int iResult = pCtpTradeApi->GetTraderApi()->ReqParkedOrderInsert(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(2000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqParkedOrderInsert(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_parkedorder(QryParkedOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryParkedOrderField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryParkedOrder(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryParkedOrder(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::remove_parkedorder(RemoveParkedOrderParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcRemoveParkedOrderField  req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	strcpy_s(req.ParkedOrderID, param.ParkedOrderID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqRemoveParkedOrder(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(2000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqRemoveParkedOrder(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::cancel_parkedorder(ParkedOrderActionParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcParkedOrderActionField  req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)
	{
		strcpy_s(req.InvestorID, param.InvestorID.c_str());
		strcpy_s(req.UserID, param.InvestorID.c_str());
	}
	else
	{
		strcpy_s(req.InvestorID, user_name.c_str());
		strcpy_s(req.UserID, param.InvestorID.c_str());
	}
	strcpy_s(req.ExchangeID, param.ExchangeID.c_str());
	strcpy_s(req.InstrumentID, param.InstrumentID.c_str());
	strcpy_s(req.OrderSysID, param.OrderSysID.c_str());
	req.ActionFlag = THOST_FTDC_AF_Delete;
	int iResult = pCtpTradeApi->GetTraderApi()->ReqParkedOrderAction(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(2000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqParkedOrderAction(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::query_parkedorderaction(QryParkedOrderActionParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcQryParkedOrderActionField  req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqQryParkedOrderAction(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(1000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqQryParkedOrderAction(&req, ++T_iReqestID);
	}
	return true;
}

bool CtpService::remove_parkedorderaction(RemoveParkedOrderActionParam& param)
{
	if (pCtpTradeApi == NULL || pCtpTradeApi->GetTraderApi() == NULL)
	{
		return false;
	}
	CThostFtdcRemoveParkedOrderActionField req = { 0 };
	if (strlen(param.BrokerID.c_str()) != 0)		strcpy_s(req.BrokerID, param.BrokerID.c_str());
	else															strcpy_s(req.BrokerID, broker.id.c_str());
	if (strlen(param.InvestorID.c_str()) != 0)		strcpy_s(req.InvestorID, param.InvestorID.c_str());
	else															strcpy_s(req.InvestorID, user_name.c_str());
	strcpy_s(req.ParkedOrderActionID, param.ParkedOrderActionID.c_str());
	int iResult = pCtpTradeApi->GetTraderApi()->ReqRemoveParkedOrderAction(&req, ++T_iReqestID);
	while (iResult != 0)
	{
		Sleep(2000);
		iResult = pCtpTradeApi->GetTraderApi()->ReqRemoveParkedOrderAction(&req, ++T_iReqestID);
	}
	return true;
}



