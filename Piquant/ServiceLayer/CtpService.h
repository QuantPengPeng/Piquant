#ifndef _CTP_SERVICE_H_
#define _CTP_SERVICE_H_
#include "IService.h"
#include "TradeSpi.h"
#include "MdSpi.h"
#include "DataTypes.h"

class TradeSpi;//前向声明
class CMdSpi;

/*
	CtpService是UI界面层唯一可调用的类对象，并由该类对象去操作TradeSpi* pCtpTradeApi交易api对象及CMdSpi* pCtpMdApi行情api对象
	简言之如下:
	UI界面层全局管理器GlobalMgr->CtpService
	CtpService承载主动函数请求部分,并内含pCtpTradeApi及pCtpMdApi两个保护成员来处理ctp的回调部分
*/
class SERVICE_API CtpService : public IService
{
public:
	CtpService();
	virtual ~CtpService();
	virtual bool login(const string& user_name, const string& password, const ServerInfo& bs);
	virtual bool logout();
	virtual bool islogined();
	virtual bool request(int request_id, void* param);//通用请求方法
	virtual bool get_basic_data(vector<BaseData>& base_data); //用于获取全部合约的基础数据
	virtual bool get_exchanges(vector<Exchange>& exchanges);//用于获取全部交易所的信息数据
	virtual bool get_all_InvestorPositionDetails(vector<InvestorPositionDetail>& positiondetails);//用于获取全部持仓明细数据
	virtual std::string get_username();
	virtual ServerInfo get_brokerinfo();

protected:
	//修改交易账户或资金账户的密码，内部调用ReqUserPasswordUpdate更新交易账户密码、ReqTradingAccountPasswordUpdate更新资金账户密码
	bool update_password(UpdatePasswordParam& param);

	//下单,内部调用ReqOrderInsert，可下限价单、市价单、条件单等交易所支持的指令
	bool make_order(InputOrderParam& param);

	//当报单尚未成交前撤单,内部调用ReqOrderAction
	bool cancel_order(ActionOrderParam& param);

	//查询账户资金,内部调用ReqQryTradingAccount
	bool query_account(TradeAccountParam& param);

	//查询行情,内部调用ReqQryDepthMarketData
	bool query_market_data(vector<string>& instrumentid = vector<string>());

	//查询持仓，内部调用ReqQryInvestorPosition查询持仓
	bool query_position(PositionParam& param);

	//查余额，内部调用ReqQueryBankAccountMoneyByFuture
	bool query_money();

	//银期转账，内部调用ReqFromBankToFutureByFuture发起银行资金转期货请求、ReqFromFutureToBankByFuture发起期货资金转银行请求
	bool Bank_Future_Transfer(BankFutureTransParam& param);

	//查合约
	bool query_instruments(vector<string>& instrument_vector, bool qry_all=false);

	//查询特定交易所的信息,形参为交易所ID
	bool query_exchanges(const string& eid);

	//查特定品种的手续费,内部调用ReqQryInstrumentCommissionRate
	bool query_CommissionRate(InstrumentCommissionRateParam& param);

	//查询委托,内部调用ReqQryOrder
	bool query_Order(QryOrderParam& param);

	//查询成交,内部调用ReqQryTrade
	bool query_Trade(TradeParam& param);

	//查询保证金率
	bool query_MarginRate(QryInstrumentMarginRateParam& param);

	//查询汇率，内部调用ReqQryExchangeRate
	bool query_ExchangeRate(QryExchangeRateParam& param);

	//查询报单手续费,内部调用ReqQryInstrumentOrderCommRate
	bool query_InstrumentOrderCommRate(QryInstrumentOrderCommRateParam& param);

	//查询产品,内部调用ReqQryProduct
	bool query_Product(QryProductParam& param);

	//查询产品行情
	bool query_DepthMarketData(QryDepthMarketDataＰaram& param);

	//查持仓明细
	bool query_InvestorPositionDetail(QryInvestorPositionDetailParam& param);

	//批量撤单
	bool batchorder_action(BatchOrderActionParam& param);

	//查询经纪商客户通知
	bool query_notice(QryNoticeParam& param);

	//查询交易通知
	bool query_tradingnotice(QryTradingNoticeParam& param);

	//下预埋单,内部调用ReqParkedOrderInsert
	bool make_parkedorder(InputParkedOrderParam& param);

	//查询预埋单
	bool query_parkedorder(QryParkedOrderParam& param);

	//预埋单撤单
	bool remove_parkedorder(RemoveParkedOrderParam& param);

	//预埋撤单
	bool cancel_parkedorder(ParkedOrderActionParam& param);

	//查询预埋撤单
	bool query_parkedorderaction(QryParkedOrderActionParam& param);

	//预埋撤单撤单
	bool remove_parkedorderaction(RemoveParkedOrderActionParam& param);

protected:
	int T_iReqestID;
	TradeSpi* pCtpTradeApi; //交易api对象当login时创建
	CMdSpi* pCtpMdApi;     //行情api对象当login时创建
	string user_name;
	ServerInfo broker;
};


#endif
