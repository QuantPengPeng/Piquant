#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_
#include <string>
#include "IService.h"
using namespace std;


/*
	入参部分
*/
struct RemoveParkedOrderActionParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///预埋撤单编号
	string ParkedOrderActionID;
	///投资单元代码
	string InvestUnitID;
};

struct QryParkedOrderActionParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///投资单元代码
	string InvestUnitID;
};

struct ParkedOrderActionParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///报单操作引用
	int OrderActionRef;
	///报单引用
	string OrderRef;
	///请求编号
	int RequestID;
	///前置编号
	int FrontID;
	///会话编号
	int SessionID;
	///交易所代码
	string ExchangeID;
	///报单编号
	string OrderSysID;
	///操作标志
	char ActionFlag;
	///价格
	double LimitPrice;
	///数量变化
	int VolumeChange;
	///用户代码
	string UserID;
	///合约代码
	string InstrumentID;
	///预埋撤单单编号
	string ParkedOrderActionID;
	///用户类型
	char UserType;
	///预埋撤单状态
	char Status;
	///错误代码
	int ErrorID;
	///错误信息
	string ErrorMsg;
	///投资单元代码
	string InvestUnitID;
	///IP地址
	string IPAddress;
	///Mac地址
	string MacAddress;
};

struct RemoveParkedOrderParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///预埋报单编号
	string ParkedOrderID;
	///投资单元代码
	string InvestUnitID;
};

struct QryParkedOrderParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///投资单元代码
	string InvestUnitID;
};

struct InputParkedOrderParam
{
	///经纪公司代码
	string	BrokerID;
	///投资者代码
	string	InvestorID;
	///合约代码
	string	InstrumentID;
	///报单引用
	string	OrderRef;
	///用户代码
	string	UserID;
	///报单价格条件
	char	OrderPriceType;
	///买卖方向
	char	Direction;
	///组合开平标志
	char	CombOffsetFlag[5];
	///组合投机套保标志
	char	CombHedgeFlag[5];
	///价格
	double	LimitPrice;
	///数量
	int	VolumeTotalOriginal;
	///有效期类型
	char	TimeCondition;
	///GTD日期
	string	GTDDate;
	///成交量类型
	char	VolumeCondition;
	///最小成交量
	int	MinVolume;
	///触发条件
	char	ContingentCondition;
	///止损价
	double	StopPrice;
	///强平原因
	char	ForceCloseReason;//普通用户下单填写THOST_FTDC_FCC_NotForceClose 非强平
	///自动挂起标志
	int	IsAutoSuspend;
	///业务单元
	string	BusinessUnit;
	///请求编号
	int	RequestID;
	///用户强评标志
	int	UserForceClose;
	///互换单标志
	int	IsSwapOrder;//暂不支持互换单，填0
	///交易所代码
	string	ExchangeID;
	///投资单元代码
	string	InvestUnitID;
	///资金账号
	string	AccountID;
	///币种代码
	string	CurrencyID;
	///交易编码
	string	ClientID;
	///IP地址
	string	IPAddress;
	///Mac地址
	string	MacAddress;
	///预埋报单编号
	string ParkedOrderID;
	///用户类型
	char UserType;
	///预埋单状态
	char Status;
	///错误代码
	int ErrorID;
	///错误信息
	string ErrorMsg;
};

struct QryTradingNoticeParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///投资单元代码
	string InvestUnitID;
};

struct QryNoticeParam
{
	///经纪公司代码
	string BrokerID;
};

struct BatchOrderActionParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///报单操作引用
	int OrderActionRef;
	///请求编号
	int RequestID;
	///前置编号
	int FrontID;
	///会话编号
	int SessionID;
	///交易所代码
	string ExchangeID;
	///用户代码
	string UserID;
	///投资单元代码
	string InvestUnitID;
	///IP地址
	string IPAddress;
	///Mac地址
	string MacAddress;
};

struct QryInvestorPositionDetailParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///投资单元代码
	string InvestUnitID;
};

struct QryDepthMarketDataＰaram
{
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
};

struct QryProductParam
{
	///产品代码
	string ProductID;
	///产品类型
	char ProductClass;
	///交易所代码
	string ExchangeID;
};

struct QryInstrumentOrderCommRateParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
};

struct QryExchangeRateParam
{
	///经纪公司代码
	string BrokerID;
	///源币种
	string FromCurrencyID;
	///目标币种
	string ToCurrencyID;
};

struct QryInstrumentMarginRateParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
	///投机套保标志
	char HedgeFlag;
	///交易所代码
	string ExchangeID;
	///投资单元代码
	string InvestUnitID;
};

struct QryOrderParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID; 
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///报单编号
	string OrderSysID;
	///开始时间
	string InsertTimeStart;
	///结束时间
	string InsertTimeEnd;
	///投资单元代码
	string InvestUnitID;
};


struct InstrumentCommissionRateParam
{
	///经纪公司代码
	char	BrokerID[11];
	///投资者代码
	char	InvestorID[13];
	///合约代码
	char	InstrumentID[31];
	///交易所代码
	char	ExchangeID[9];
	///投资单元代码
	char	InvestUnitID[17];
};

struct BankFutureTransParam
{
	///业务功能码
	char	TradeCode[7];
	///银行代码
	char	BankID[4];
	///银行分支机构代码
	char	BankBranchID[5];
	///期商代码
	char	BrokerID[11];
	///期商分支机构代码
	char	BrokerBranchID[31];
	///交易日期
	char	TradeDate[9];
	///交易时间
	char	TradeTime[9];
	///银行流水号
	char	BankSerial[13];
	///交易系统日期 
	char	TradingDay[9];
	///银期平台消息流水号
	int	PlateSerial;
	///最后分片标志
	char	LastFragment;
	///会话号
	int	SessionID;
	///客户姓名
	char	CustomerName[51];
	///证件类型
	char	IdCardType;
	///证件号码
	char	IdentifiedCardNo[51];
	///客户类型
	char	CustType;
	///银行帐号
	char	BankAccount[41];
	///银行密码
	char	BankPassWord[41];
	///投资者帐号
	char	AccountID[13];
	///期货密码
	char	Password[41];
	///安装编号
	int	InstallID;
	///期货公司流水号
	int	FutureSerial;
	///用户标识
	char	UserID[16];
	///验证客户证件号码标志
	char	VerifyCertNoFlag;
	///币种代码
	char	CurrencyID[4];
	///转帐金额
	double	TradeAmount;
	///期货可取金额
	double	FutureFetchAmount;
	///费用支付标志
	char	FeePayFlag;
	///应收客户费用
	double	CustFee;
	///应收期货公司费用
	double	BrokerFee;
	///发送方给接收方的消息
	char	Message[129];
	///摘要
	char	Digest[36];
	///银行帐号类型
	char	BankAccType;
	///渠道标志
	char	DeviceID[3];
	///期货单位帐号类型
	char	BankSecuAccType;
	///期货公司银行编码
	char	BrokerIDByBank[33];
	///期货单位帐号
	char	BankSecuAcc[41];
	///银行密码标志
	char	BankPwdFlag;
	///期货资金密码核对标志
	char	SecuPwdFlag;
	///交易柜员
	char	OperNo[17];
	///请求编号
	int	RequestID;
	///交易ID
	int	TID;
	///转账交易状态
	char	TransferStatus;
	///长客户姓名
	char	LongCustomerName[161];
};

struct PositionParam
{
	int PositionType;//类型，查询的是持仓，持仓明细还是组合持仓
	///经纪公司代码
	char	BrokerID[11];
	///投资者代码
	char	InvestorID[13];
	///合约代码
	char	InstrumentID[31];
	///交易所代码
	char	ExchangeID[9];
	///投资单元代码
	char InvestUnitID[17];
};

struct TradeAccountParam
{
	///经纪公司代码
	char	BrokerID[11];
	///投资者代码
	char	InvestorID[11];
	///币种代码
	char	CurrencyID[4];
	///业务类型
	char BizType;
	///投资者帐号
	char	AccountID[13];
};

struct TradeParam
{
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///合约代码
	string InstrumentID;
	///交易所代码
	string ExchangeID;
	///成交编号
	string TradeID;
	///开始时间
	string TradeTimeStart;
	///结束时间
	string TradeTimeEnd;
	///投资单元代码
	string InvestUnitID;
};

struct ActionOrderParam
{
	///经纪公司代码
	char	BrokerID[11];
	///投资者代码
	char	InvestorID[13];
	///报单操作引用
	int	OrderActionRef;
	///报单引用
	char	OrderRef[13];
	///请求编号
	int	RequestID;
	///前置编号
	int	FrontID;
	///会话编号
	int	SessionID;
	///交易所代码
	char	ExchangeID[9];
	///报单编号
	char	OrderSysID[21];
	///操作标志
	char	ActionFlag;
	///价格
	double	LimitPrice;
	///数量变化
	int	VolumeChange;
	///用户代码
	char	UserID[16];
	///合约代码
	char	InstrumentID[31];
	///投资单元代码
	char	InvestUnitID[17];
	///IP地址
	char	IPAddress[16];
	///Mac地址
	char	MacAddress[21];
};

struct InputOrderParam
{
	///经纪公司代码
	string	BrokerID;
	///投资者代码
	string	InvestorID;
	///合约代码
	string	InstrumentID;
	///报单引用
	string	OrderRef;
	///用户代码
	string	UserID;
	///报单价格条件
	char	OrderPriceType;
	///买卖方向
	char	Direction;
	///组合开平标志
	char	CombOffsetFlag[5];
	///组合投机套保标志
	char	CombHedgeFlag[5];
	///价格
	double	LimitPrice;
	///数量
	int	VolumeTotalOriginal;
	///有效期类型
	char	TimeCondition;
	///GTD日期
	string	GTDDate;
	///成交量类型
	char	VolumeCondition;
	///最小成交量
	int	MinVolume;
	///触发条件
	char	ContingentCondition;
	///止损价
	double	StopPrice;
	///强平原因
	char	ForceCloseReason;//普通用户下单填写THOST_FTDC_FCC_NotForceClose 非强平
	///自动挂起标志
	int	IsAutoSuspend;
	///业务单元
	string	BusinessUnit;
	///请求编号
	int	RequestID;
	///用户强评标志
	int	UserForceClose;
	///互换单标志
	int	IsSwapOrder;//暂不支持互换单，填0
	///交易所代码
	string	ExchangeID;
	///投资单元代码
	string	InvestUnitID;
	///资金账号
	string	AccountID;
	///币种代码
	string	CurrencyID;
	///交易编码
	string	ClientID;
	///IP地址
	string	IPAddress;
	///Mac地址
	string	MacAddress;
};

struct UpdatePasswordParam
{
	int PasswordType;//类型，是资金账户还是交易账户密码更新
	///经纪公司代码
	string	BrokerID;
	///投资者帐号
	string	AccountID;
	///原来的口令
	string	OldPassword;
	///新的口令
	string	NewPassword;
	///币种代码
	string	CurrencyID;
};


/*
	返回值
*/
struct  UserInfo
{//用户信息
	///交易日
	char	TradingDay[9];
	///登录成功时间
	char	LoginTime[9];
	///经纪公司代码
	char	BrokerID[11];
	///用户代码
	char	UserID[16];
	///交易系统名称
	char	SystemName[41];
	///前置编号
	int	FrontID;
	///会话编号
	int	SessionID;
	///最大报单引用
	char	MaxOrderRef[13];
	///交易所时间
	char	ExchangeTime[41];
};

struct MarketData
{//自定义深度行情结构体
	int trade_day;
	///合约代码
	char	code[31];
	///交易所代码
	char 	ExchangeID[9];
	///交易日
	char	TradingDay[9];
	///最新价
	double	LastPrice;
	///今开盘
	double	OpenPrice;
	///最高价
	double	HighestPrice;
	///最低价
	double	LowestPrice;
	///今收盘
	double	ClosePrice;
	///数量
	int	Volume;
	///成交金额
	double	Turnover;
	///上次结算价
	double	PreSettlementPrice;
	///本次结算价
	double	SettlementPrice;
	///昨收盘
	double	PreClosePrice;
	///昨持仓量
	double	PreOpenInterest;
	///持仓量
	double	OpenInterest;
	///最后修改时间
	char	UpdateTime[9];
	///最后修改毫秒
	int	UpdateMillisec;
	///申买一价
	double	BidPrice;
	///申买一量
	int	BidVolume;
	///申卖一价
	double AskPrice;
	///申卖一量
	int	AskVolume;
	///当日均价
	double	AveragePrice;
	///涨停板价
	double	UpperLimitPrice;
	///跌停板价
	double	LowerLimitPrice;
	///昨虚实度
	//TThostFtdcRatioType	PreDelta;
	///今虚实度
	//TThostFtdcRatioType	CurrDelta;
};

struct BaseData
{//合约基本数据结构
	///合约代码
	char	code[31];
	///交易所代码
	char 	ExchangeID[9];
	///合约名称
	char	InstrumentName[21];
	///产品代码
	char	ProductID[33];
	///产品类型
	char	ProductClass;
	///市价单最大下单量
	int	MaxMarketOrderVolume;
	///市价单最小下单量
	int 	MinMarketOrderVolume;
	///限价单最大下单量
	int	MaxLimitOrderVolume;
	///限价单最小下单量
	int    MinLimitOrderVolume;
	///合约数量乘数
	int	VolumeMultiple;
	///最小变动价位
	double	PriceTick;
	///创建日
	char	CreateDate[9];
	///上市日
	char	OpenDate[9];
	///到期日
	char	ExpireDate[9];
	///开始交割日
	char	StartDelivDate[9];
	///结束交割日
	char	EndDelivDate[9];
	///当前是否交易
	int	IsTrading;
	///持仓类型
	char	PositionType;
	///多头保证金率
	double	LongMarginRatio;
	///空头保证金率
	double	ShortMarginRatio;
	///合约在交易所的代码
	char	ExchangeInstID[31];
	///交割年份
	int	DeliveryYear;
	///交割月
	int	DeliveryMonth;
	///合约生命周期状态
	char	InstLifePhase;
	///持仓日期类型
	char	PositionDateType;
	///是否使用大额单边保证金算法
	char	MaxMarginSideAlgorithm;
	///基础商品代码
	char	UnderlyingInstrID[31];
	///执行价
	double	StrikePrice;
	///期权类型
	char	OptionsType;
	///合约基础商品乘数
	double	UnderlyingMultiple;
	///组合类型
	char	CombinationType;
};

//返回的持仓数据
struct InvestorPosition
{
	///合约代码
	string InstrumentID;
	///经纪公司代码
	string BrokerID;
	///投资者代码
	string InvestorID;
	///持仓多空方向
	char PosiDirection;
	///投机套保标志
	char HedgeFlag;
	///持仓日期类型
	char PositionDate;
	///上日持仓
	int YdPosition;
	///今日持仓
	int Position;
	///多头冻结
	int LongFrozen;
	///空头冻结
	int ShortFrozen;
	///开仓冻结金额
	double LongFrozenAmount;
	///开仓冻结金额
	double ShortFrozenAmount;
	///开仓量
	int OpenVolume;
	///平仓量
	int CloseVolume;
	///开仓金额
	double OpenAmount;
	///平仓金额
	double CloseAmount;
	///持仓成本
	double PositionCost;
	///上次占用的保证金
	double PreMargin;
	///占用的保证金
	double UseMargin;
	///冻结的保证金
	double FrozenMargin;
	///冻结的资金
	double FrozenCash;
	///冻结的手续费
	double FrozenCommission;
	///资金差额
	double CashIn;
	///手续费
	double Commission;
	///平仓盈亏
	double CloseProfit;
	///持仓盈亏
	double PositionProfit;
	///上次结算价
	double PreSettlementPrice;
	///本次结算价
	double SettlementPrice;
	///交易日
	string TradingDay;
	///结算编号
	int SettlementID;
	///开仓成本
	double OpenCost;
	///交易所保证金
	double ExchangeMargin;
	///组合成交形成的持仓
	int CombPosition;
	///组合多头冻结
	int CombLongFrozen;
	///组合空头冻结
	int CombShortFrozen;
	///逐日盯市平仓盈亏
	double CloseProfitByDate;
	///逐笔对冲平仓盈亏
	double CloseProfitByTrade;
	///今日持仓
	int TodayPosition;
	///保证金率
	double MarginRateByMoney;
	///保证金率(按手数)
	double MarginRateByVolume;
	///执行冻结
	int StrikeFrozen;
	///执行冻结金额
	double StrikeFrozenAmount;
	///放弃执行冻结
	int AbandonFrozen;
	///交易所代码
	string ExchangeID;
	///执行冻结的昨仓
	int YdStrikeFrozen;
	///投资单元代码
	string InvestUnitID;
};

struct Order
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///合约代码
	char InstrumentID[31];
	///报单引用
	char OrderRef[13];
	///用户代码
	char UserID[16];
	///报单价格条件
	char OrderPriceType;
	///买卖方向
	char Direction;
	///组合开平标志
	char CombOffsetFlag[5];
	///组合投机套保标志
	char CombHedgeFlag[5];
	///价格
	double LimitPrice;
	///数量
	int VolumeTotalOriginal;
	///有效期类型
	char TimeCondition;
	///GTD日期
	char GTDDate[9];
	///成交量类型
	char VolumeCondition;
	///最小成交量
	int MinVolume;
	///触发条件
	char ContingentCondition;
	///止损价
	double StopPrice;
	///强平原因
	char ForceCloseReason;
	///自动挂起标志
	int IsAutoSuspend;
	///业务单元
	char BusinessUnit[21];
	///请求编号
	int RequestID;
	///本地报单编号
	char OrderLocalID[13];
	///交易所代码
	char ExchangeID[9];
	///会员代码
	char ParticipantID[11];
	///客户代码
	char ClientID[11];
	///合约在交易所的代码
	char ExchangeInstID[31];
	///交易所交易员代码
	char TraderID[21];
	///安装编号
	int InstallID;
	///报单提交状态
	char OrderSubmitStatus;
	///报单提示序号
	int NotifySequence;
	///交易日
	char TradingDay[9];
	///结算编号
	int SettlementID;
	///报单编号
	char OrderSysID[21];
	///报单来源
	char OrderSource;
	///报单状态
	char OrderStatus;
	///报单类型
	char OrderType;
	///今成交数量
	int VolumeTraded;
	///剩余数量
	int VolumeTotal;
	///报单日期
	char InsertDate[9];
	///委托时间
	char InsertTime[9];
	///激活时间
	char ActiveTime[9];
	///挂起时间
	char SuspendTime[9];
	///最后修改时间
	char UpdateTime[9];
	///撤销时间
	char CancelTime[9];
	///最后修改交易所交易员代码
	char ActiveTraderID[21];
	///结算会员编号
	char ClearingPartID[11];
	///序号
	int SequenceNo;
	///前置编号
	int FrontID;
	///会话编号
	int SessionID;
	///用户端产品信息
	char UserProductInfo[11];
	///状态信息
	char StatusMsg[81];
	///用户强评标志
	int UserForceClose;
	///操作用户代码
	char ActiveUserID[16];
	///经纪公司报单编号
	int BrokerOrderSeq;
	///相关报单
	char RelativeOrderSysID[21];
	///郑商所成交数量
	int ZCETotalTradedVolume;
	///互换单标志
	int IsSwapOrder;
	///营业部编号
	char BranchID[9];
	///投资单元代码
	char InvestUnitID[17];
	///资金账号
	char AccountID[13];
	///币种代码
	char CurrencyID[4];
	///IP地址
	char IPAddress[16];
	///Mac地址
	char MacAddress[21];
};

struct Trade
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///合约代码
	char InstrumentID[31];
	///报单引用
	char OrderRef[13];
	///用户代码
	char UserID[16];
	///交易所代码
	char ExchangeID[9];
	///成交编号
	char TradeID[21];
	///买卖方向
	char Direction;
	///报单编号
	char OrderSysID[21];
	///会员代码
	char ParticipantID[11];
	///客户代码
	char ClientID[11];
	///交易角色
	char TradingRole;
	///合约在交易所的代码
	char ExchangeInstID[31];
	///开平标志
	char OffsetFlag;
	///投机套保标志
	char HedgeFlag;
	///价格
	double Price;
	///数量
	int Volume;
	///成交时期
	char TradeDate[9];
	///成交时间
	char TradeTime[9];
	///成交类型
	char TradeType;
	///成交价来源
	char PriceSource;
	///交易所交易员代码
	char TraderID[21];
	///本地报单编号
	char OrderLocalID[13];
	///结算会员编号
	char ClearingPartID[11];
	///业务单元
	char BusinessUnit[21];
	///序号
	int SequenceNo;
	///交易日
	char TradingDay[9];
	///结算编号
	int SettlementID;
	///经纪公司报单编号
	int BrokerOrderSeq;
	///成交来源
	char TradeSource;
	///投资单元代码
	char InvestUnitID[17];
};

///合约手续费率
struct InstrumentCommissionRate
{
	///合约代码
	char InstrumentID[31];
	///投资者范围
	char InvestorRange;
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///开仓手续费率
	double OpenRatioByMoney;
	///开仓手续费
	double OpenRatioByVolume;
	///平仓手续费率
	double CloseRatioByMoney;
	///平仓手续费
	double CloseRatioByVolume;
	///平今手续费率
	double CloseTodayRatioByMoney;
	///平今手续费
	double CloseTodayRatioByVolume;
	///交易所代码
	char ExchangeID[9];
	///业务类型
	char BizType;
	///投资单元代码
	char InvestUnitID[17];
};

struct MarginRate
{
	///合约代码
	char InstrumentID[31];
	///投资者范围
	char InvestorRange;
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///投机套保标志
	char HedgeFlag;
	///多头保证金率
	double LongMarginRatioByMoney;
	///多头保证金费
	double LongMarginRatioByVolume;
	///空头保证金率
	double ShortMarginRatioByMoney;
	///空头保证金费
	double ShortMarginRatioByVolume;
	///是否相对交易所收取
	int IsRelative;
	///交易所代码
	char ExchangeID[9];
	///投资单元代码
	char InvestUnitID[17];
};

struct _ExchangeRate
{
	///经纪公司代码
	char BrokerID[11];
	///源币种
	char FromCurrencyID[4];
	///源币种单位数量
	double FromCurrencyUnit;
	///目标币种
	char ToCurrencyID[4];
	///汇率
	double ExchangeRate;
};

struct OrderCommRate
{
	///合约代码
	char InstrumentID[31];
	///经纪公司代码
	char BrokerID[11];
	///报单手续费
	double OrderCommByVolume;
	///撤单手续费
	double OrderActionCommByVolume;
};

struct Product_InvaildInstrument
{
	///产品代码
	char ProductID[31];
	///产品名称
	char ProductName[21];
	///交易所代码
	char ExchangeID[9];
	///产品类型
	char ProductClass;
	///合约数量乘数
	int VolumeMultiple;
	///最小变动价位
	double PriceTick;
	///市价单最大下单量
	int MaxMarketOrderVolume;
	///市价单最小下单量
	int MinMarketOrderVolume;
	///限价单最大下单量
	int MaxLimitOrderVolume;
	///限价单最小下单量
	int MinLimitOrderVolume;
	///持仓类型
	char PositionType;
	///持仓日期类型
	char PositionDateType;
	///平仓处理类型
	char CloseDealType;
	///交易币种类型
	char TradeCurrencyID[4];
	///质押资金可用范围
	char MortgageFundUseRange;
	///交易所产品代码
	char ExchangeProductID[31];
	///合约基础商品乘数
	double UnderlyingMultiple;
};

struct DepthMarketData
{
	///交易日
	char TradingDay[9];
	///合约代码
	char InstrumentID[31];
	///交易所代码
	char ExchangeID[9];
	///合约在交易所的代码
	char ExchangeInstID[31];
	///最新价
	double LastPrice;
	///上次结算价
	double PreSettlementPrice;
	///昨收盘
	double PreClosePrice;
	///昨持仓量
	double PreOpenInterest;
	///今开盘
	double OpenPrice;
	///最高价
	double HighestPrice;
	///最低价
	double LowestPrice;
	///数量
	int Volume;
	///成交金额
	double Turnover;
	///持仓量
	double OpenInterest;
	///今收盘
	double ClosePrice;
	///本次结算价
	double SettlementPrice;
	///涨停板价
	double UpperLimitPrice;
	///跌停板价
	double LowerLimitPrice;
	///昨虚实度
	double PreDelta;
	///今虚实度
	double CurrDelta;
	///最后修改时间
	char UpdateTime[9];
	///最后修改毫秒
	int UpdateMillisec;
	///申买价一
	double BidPrice1;
	///申买量一
	int BidVolume1;
	///申卖价一
	double AskPrice1;
	///申卖量一
	int AskVolume1;
	///当日均价
	double AveragePrice;
	///业务日期
	char ActionDay[9];
};

struct InvestorPositionDetail
{
	///合约代码
	char InstrumentID[31];
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///投机套保标志
	char HedgeFlag;
	///买卖
	char Direction;
	///开仓日期
	char OpenDate[9];
	///成交编号
	char TradeID[21];
	///数量
	int Volume;
	///开仓价
	double OpenPrice;
	///交易日
	char TradingDay[9];
	///结算编号
	int SettlementID;
	///成交类型
	char TradeType;
	///组合合约代码
	char CombInstrumentID[31];
	///交易所代码
	char ExchangeID[9];
	///逐日盯市平仓盈亏
	double CloseProfitByDate;
	///逐笔对冲平仓盈亏
	double CloseProfitByTrade;
	///逐日盯市持仓盈亏
	double PositionProfitByDate;
	///逐笔对冲持仓盈亏
	double PositionProfitByTrade;
	///投资者保证金
	double Margin;
	///交易所保证金
	double ExchMargin;
	///保证金率
	double MarginRateByMoney;
	///保证金率(按手数)
	double MarginRateByVolume;
	///昨结算价
	double LastSettlementPrice;
	///结算价
	double SettlementPrice;
	///平仓量
	int CloseVolume;
	///平仓金额
	double CloseAmount;
	///投资单元代码
	char InvestUnitID[17];
};

struct Notice
{
	///经纪公司代码
	char BrokerID[11];
	///消息正文
	char Content[501];
	///经纪公司通知内容序列号
	char SequenceLabel[2];
};

struct TradingNotice
{
	///经纪公司代码
	char BrokerID[11];
	///投资者范围
	char InvestorRange;
	///投资者代码
	char InvestorID[13];
	///序列系列号
	short SequenceSeries;
	///用户代码
	char UserID[16];
	///发送时间
	char SendTime[9];
	///序列号
	int SequenceNo;
	///消息正文
	char FieldContent[501];
	///投资单元代码
	char InvestUnitID[17];
};

///资金账户
struct TradingAccount
{
	///经纪公司代码
	char	BrokerID[11];
	///投资者帐号
	char	AccountID[13];
	///上次质押金额
	double	PreMortgage;
	///上次信用额度
	double	PreCredit;
	///上次存款额
	double	PreDeposit;
	///上次结算准备金
	double	PreBalance;
	///上次占用的保证金
	double	PreMargin;
	///利息基数
	double	InterestBase;
	///利息收入
	double	Interest;
	///入金金额
	double	Deposit;
	///出金金额
	double	Withdraw;
	///冻结的保证金
	double	FrozenMargin;
	///冻结的资金
	double	FrozenCash;
	///冻结的手续费
	double	FrozenCommission;
	///当前保证金总额
	double	CurrMargin;
	///资金差额
	double	CashIn;
	///手续费
	double	Commission;
	///平仓盈亏
	double	CloseProfit;
	///持仓盈亏
	double	PositionProfit;
	///期货结算准备金
	double	Balance;
	///可用资金
	double	Available;
	///可取资金
	double	WithdrawQuota;
	///基本准备金
	double	Reserve;
	///交易日
	char	TradingDay[9];
	///结算编号
	int	SettlementID;
	///信用额度
	double	Credit;
	///质押金额
	double	Mortgage;
	///交易所保证金
	double	ExchangeMargin;
	///投资者交割保证金
	double	DeliveryMargin;
	///交易所交割保证金
	double	ExchangeDeliveryMargin;
	///保底期货结算准备金
	double	ReserveBalance;
	///币种代码
	char	CurrencyID[4];
	///上次货币质入金额
	double	PreFundMortgageIn;
	///上次货币质出金额
	double	PreFundMortgageOut;
	///货币质入金额
	double	FundMortgageIn;
	///货币质出金额
	double	FundMortgageOut;
	///货币质押余额
	double	FundMortgageAvailable;
	///可质押货币金额
	double	MortgageableFund;
	///特殊产品占用保证金
	double	SpecProductMargin;
	///特殊产品冻结保证金
	double	SpecProductFrozenMargin;
	///特殊产品手续费
	double	SpecProductCommission;
	///特殊产品冻结手续费
	double	SpecProductFrozenCommission;
	///特殊产品持仓盈亏
	double	SpecProductPositionProfit;
	///特殊产品平仓盈亏
	double	SpecProductCloseProfit;
	///根据持仓盈亏算法计算的特殊产品持仓盈亏
	double	SpecProductPositionProfitByAlg;
	///特殊产品交易所保证金
	double	SpecProductExchangeMargin;
	///业务类型
	char BizType;
	///延时换汇冻结金额
	double	FrozenSwap;
	///剩余换汇额度
	double	RemainSwap;
};

struct ParkedOrder
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///合约代码
	char InstrumentID[31];
	///报单引用
	char OrderRef[13];
	///用户代码
	char UserID[16];
	///报单价格条件
	char OrderPriceType;
	///买卖方向
	char Direction;
	///组合开平标志
	char CombOffsetFlag[5];
	///组合投机套保标志
	char CombHedgeFlag[5];
	///价格
	double LimitPrice;
	///数量
	int VolumeTotalOriginal;
	///有效期类型
	char TimeCondition;
	///GTD日期
	char GTDDate[9];
	///成交量类型
	char VolumeCondition;
	///最小成交量
	int MinVolume;
	///触发条件
	char ContingentCondition;
	///止损价
	double StopPrice;
	///强平原因
	char ForceCloseReason;
	///自动挂起标志
	int IsAutoSuspend;
	///业务单元
	char BusinessUnit[21];
	///请求编号
	int RequestID;
	///交易所代码
	char ExchangeID[9];
	///用户强评标志
	int UserForceClose;
	///预埋报单编号
	char ParkedOrderID[13];
	///用户类型
	char UserType;
	///预埋单状态
	char Status;
	///错误代码
	int ErrorID;
	///错误信息
	char ErrorMsg[81];
	///互换单标志
	int IsSwapOrder;
	///客户代码
	char ClientID[11];
	///投资单元代码
	char InvestUnitID[17];
	///资金账号
	char AccountID[13];
	///币种代码
	char CurrencyID[4];
	///IP地址
	char IPAddress[16];
	///Mac地址
	char MacAddress[21];
};

struct Remove_ParkedOrder
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///预埋报单编号
	char ParkedOrderID[13];
	///投资单元代码
	char InvestUnitID[17];
};

struct ParkedOrderAction
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///报单操作引用
	int OrderActionRef;
	///报单引用
	char OrderRef[13];
	///请求编号
	int RequestID;
	///前置编号
	int FrontID;
	///会话编号
	int SessionID;
	///交易所代码
	char ExchangeID[9];
	///报单编号
	char OrderSysID[21];
	///操作标志
	char ActionFlag;
	///价格
	double LimitPrice;
	///数量变化
	int VolumeChange;
	///用户代码
	char UserID[16];
	///合约代码
	char InstrumentID[31];
	///预埋撤单单编号
	char ParkedOrderActionID[13];
	///用户类型
	char UserType;
	///预埋撤单状态
	char Status;
	///错误代码
	int ErrorID;
	///错误信息
	char ErrorMsg[81];
	///投资单元代码
	char InvestUnitID[17];
	///IP地址
	char IPAddress[16];
	///Mac地址
	char MacAddress[21];
};

struct Remove_ParkedOrderAction
{
	///经纪公司代码
	char BrokerID[11];
	///投资者代码
	char InvestorID[13];
	///预埋撤单编号
	char ParkedOrderActionID[13];
	///投资单元代码
	char InvestUnitID[17];
};

//交易所信息结构体
struct Exchange
{
	///交易所代码
	string	ExchangeID;
	///交易所名称
	string	ExchangeName;
	///交易所属性
	char	ExchangeProperty;
};

//错误响应信息
struct ErrorMessage
{
	///错误代码
	int	ErrorID;
	///错误信息
	string	ErrorMsg;
};



enum FIELD_ID_UserInfo
{
	FIELD_code = 100,
	FIELD_name = 101,
	FIELD_TradingDay = 102,
	FIELD_LoginTime = 103,
	FIELD_BrokerID = 104,
	FIELD_UserID = 105,
	FIELD_SystemName = 106,
	FIELD_FrontID = 107,
	FIELD_SessionID = 108,
	FIELD_MaxOrderRef = 109,
	FIELD_ExchangeTime = 110,
};

enum FIELD_ID_BaseData
{
	FIELD_Code = 100,
	FIELD_ExchangeID = 101,
	FIELD_InstrumentName = 102,
	FIELD_ExchangeInstID = 103,
	FIELD_ProductID = 104,
	FIELD_ProductClass = 105,
	FIELD_DeliveryYear = 106,
	FIELD_DeliveryMonth = 107,
	FIELD_MaxMarketOrderVolume = 108,
	FIELD_MinMarketOrderVolume = 109,
	FIELD_MaxLimitOrderVolume = 110,
	FIELD_MinLimitOrderVolume = 111,
	FIELD_VolumeMultiple = 112,
	FIELD_PriceTick = 113,
	FIELD_CreateDate = 114,
	FIELD_OpenDate = 115,
	FIELD_ExpireDate = 116,
	FIELD_StartDelivDate = 117,
	FIELD_EndDelivDate = 118,
	FIELD_InstLifePhase = 119,
	FIELD_IsTrading = 120,
	FIELD_PositionType = 121,
	FIELD_PositionDateType = 122,
	FIELD_LongMarginRatio = 123,
	FIELD_ShortMarginRatio = 124,
	FIELD_MaxMarginSideAlgorithm = 125,
	FIELD_UnderlyingInstrID = 126,
	FIELD_StrikePrice = 127,
	FIELD_OptionsType = 128,
	FIELD_UnderlyingMultiple = 129,
	FIELD_CombinationType = 130,
};

enum FIELD_ID_InvestorPosition
{
	FIELD_InstrumentID_Position = 100,
	FIELD_BrokerID_Position = 101,
	FIELD_InvestorID_Position = 102,
	FIELD_PosiDirection_Position = 103,
	FIELD_HedgeFlag_Position = 104,
	FIELD_PositionDate_Position = 105,
	FIELD_YdPosition_Position = 106,
	FIELD_Position_Position = 107,
	FIELD_LongFrozen_Position = 108,
	FIELD_ShortFrozen_Position = 109,
	FIELD_LongFrozenAmount_Position = 110,
	FIELD_ShortFrozenAmount_Position = 111,
	FIELD_OpenVolume_Position = 112,
	FIELD_CloseVolume_Position = 113,
	FIELD_OpenAmount_Position = 114,
	FIELD_CloseAmount_Position = 115,
	FIELD_PositionCost_Position = 116,
	FIELD_PreMargin_Position = 117,
	FIELD_UseMargin_Position = 118,
	FIELD_FrozenMargin_Position = 119,
	FIELD_FrozenCash_Position = 120,
	FIELD_FrozenCommission_Position = 121,
	FIELD_CashIn_Position = 122,
	FIELD_Commission_Position = 123,
	FIELD_CloseProfit_Position = 124,
	FIELD_PositionProfit_Position = 125,
	FIELD_PreSettlementPrice_Position = 126,
	FIELD_SettlementPrice_Position = 127,
	FIELD_TradingDay_Position = 128,
	FIELD_SettlementID_Position = 129,
	FIELD_OpenCost_Position = 130,
	FIELD_ExchangeMargin_Position = 131,
	FIELD_CombPosition_Position = 132,
	FIELD_CombLongFrozen_Position = 133,
	FIELD_CombShortFrozen_Position = 134,
	FIELD_CloseProfitByDate_Position = 135,
	FIELD_CloseProfitByTrade_Position = 136,
	FIELD_TodayPosition_Position = 137,
	FIELD_MarginRateByMoney_Position = 138,
	FIELD_MarginRateByVolume_Position = 139,
	FIELD_StrikeFrozen_Position = 140,
	FIELD_StrikeFrozenAmount_Position = 141,
	FIELD_AbandonFrozen_Position = 142,
	FIELD_ExchangeID_Position = 143,
	FIELD_YdStrikeFrozen_Position = 144,
	FIELD_InvestUnitID_Position = 145,
	FIELD_FloatProfit_Position = 146, //自定义的
	FIELD_AvgPrice_Position = 147, //自定义的
};

enum FIELD_ID_Order
{
	FIELD_BrokerID_Order = 100,
	FIELD_InvestorID_Order = 101,
	FIELD_InstrumentID_Order = 102,
	FIELD_OrderRef_Order = 103,
	FIELD_UserID_Order = 104,
	FIELD_OrderPriceType_Order = 105,
	FIELD_Direction_Order = 106,
	FIELD_CombOffsetFlag_Order = 107,
	FIELD_CombHedgeFlag_Order = 108,
	FIELD_LimitPrice_Order = 109,
	FIELD_VolumeTotalOriginal_Order = 110,
	FIELD_TimeCondition_Order = 111,
	FIELD_GTDDate_Order = 112,
	FIELD_VolumeCondition_Order = 113,
	FIELD_MinVolume_Order = 114,
	FIELD_ContingentCondition_Order = 115,
	FIELD_StopPrice_Order = 116,
	FIELD_ForceCloseReason_Order = 117,
	FIELD_IsAutoSuspend_Order = 118,
	FIELD_BusinessUnit_Order = 119,
	FIELD_RequestID_Order = 120,
	FIELD_OrderLocalID_Order = 121,
	FIELD_ExchangeID_Order = 122,
	FIELD_ParticipantID_Order = 123,
	FIELD_ClientID_Order = 124,
	FIELD_ExchangeInstID_Order = 125,
	FIELD_TraderID_Order = 126,
	FIELD_InstallID_Order = 127,
	FIELD_OrderSubmitStatus_Order = 128,
	FIELD_NotifySequence_Order = 129,
	FIELD_TradingDay_Order = 130,
	FIELD_SettlementID_Order = 131,
	FIELD_OrderSysID_Order = 132,
	FIELD_OrderSource_Order = 133,
	FIELD_OrderStatus_Order = 134,
	FIELD_OrderType_Order = 135,
	FIELD_VolumeTraded_Order = 136,
	FIELD_VolumeTotal_Order = 137,
	FIELD_InsertDate_Order = 138,
	FIELD_InsertTime_Order = 139,
	FIELD_ActiveTime_Order = 140,
	FIELD_SuspendTime_Order = 141,
	FIELD_UpdateTime_Order = 142,
	FIELD_CancelTime_Order = 143,
	FIELD_ActiveTraderID_Order = 144,
	FIELD_ClearingPartID_Order = 145,
	FIELD_SequenceNo_Order = 146,
	FIELD_FrontID_Order = 147,
	FIELD_SessionID_Order = 148,
	FIELD_UserProductInfo_Order = 149,
	FIELD_StatusMsg_Order = 150,
	FIELD_UserForceClose_Order = 151,
	FIELD_ActiveUserID_Order = 152,
	FIELD_BrokerOrderSeq_Order = 153,
	FIELD_RelativeOrderSysID_Order = 154,
	FIELD_ZCETotalTradedVolume_Order = 155,
	FIELD_IsSwapOrder_Order = 156,
	FIELD_BranchID_Order = 157,
	FIELD_InvestUnitID_Order = 158,
	FIELD_AccountID_Order = 159,
	FIELD_CurrencyID_Order = 160,
	FIELD_IPAddress_Order = 161,
	FIELD_MacAddress_Order = 162,
	FIELD_OrderLocalType_Order = 163,
};

enum FIELD_ID_Trade
{
	FIELD_BrokerID_Trade = 100,
	FIELD_InvestorID_Trade = 101,
	FIELD_InstrumentID_Trade = 102,
	FIELD_OrderRef_Trade = 103,
	FIELD_UserID_Trade = 104,
	FIELD_ExchangeID_Trade = 105,
	FIELD_TradeID_Trade = 106,
	FIELD_Direction_Trade = 107,
	FIELD_OrderSysID_Trade = 108,
	FIELD_ParticipantID_Trade = 109,
	FIELD_ClientID_Trade = 110,
	FIELD_TradingRole_Trade = 111,
	FIELD_ExchangeInstID_Trade = 112,
	FIELD_OffsetFlag_Trade = 113,
	FIELD_HedgeFlag_Trade = 114,
	FIELD_Price_Trade = 115,
	FIELD_Volume_Trade = 116,
	FIELD_TradeDate_Trade = 117,
	FIELD_TradeTime_Trade = 118,
	FIELD_TradeType_Trade = 119,
	FIELD_PriceSource_Trade = 120,
	FIELD_TraderID_Trade = 121,
	FIELD_OrderLocalID_Trade = 122,
	FIELD_ClearingPartID_Trade = 123,
	FIELD_BusinessUnit_Trade = 124,
	FIELD_SequenceNo_Trade = 125,
	FIELD_TradingDay_Trade = 126,
	FIELD_SettlementID_Trade = 127,
	FIELD_BrokerOrderSeq_Trade = 128,
	FIELD_TradeSource_Trade = 129,
	FIELD_InvestUnitID_Trade = 130,
};

enum FIELD_ID_InvestorPositionDetail
{
	FIELD_InstrumentID_PositionDetail = 100,
	FIELD_BrokerID_PositionDetail = 101,
	FIELD_InvestorID_PositionDetail = 102,
	FIELD_Direction_PositionDetail = 103,
	FIELD_HedgeFlag_PositionDetail = 104,
	FIELD_OpenDate_PositionDetail = 105,
	FIELD_TradeID_PositionDetail = 106,
	FIELD_Volume_PositionDetail = 107,
	FIELD_OpenPrice_PositionDetail = 108,
	FIELD_TradingDay_PositionDetail = 109,
	FIELD_SettlementID_PositionDetail = 110,
	FIELD_TradeType_PositionDetail = 111,
	FIELD_CombInstrumentID_PositionDetail = 112,
	FIELD_ExchangeID_PositionDetail = 113,
	FIELD_CloseProfitByDate_PositionDetail = 114,
	FIELD_CloseProfitByTrade_PositionDetail = 115,
	FIELD_PositionProfitByDate_PositionDetail = 116,
	FIELD_PositionProfitByTrade_PositionDetail = 117,
	FIELD_Margin_PositionDetail = 118,
	FIELD_ExchMargin_PositionDetail = 119,
	FIELD_MarginRateByMoney_PositionDetail = 120,
	FIELD_MarginRateByVolume_PositionDetail = 121,
	FIELD_LastSettlementPrice_PositionDetail = 122,
	FIELD_SettlementPrice_PositionDetail = 123,
	FIELD_CloseVolume_PositionDetail = 124,
	FIELD_CloseAmount_PositionDetail = 125,
	FIELD_InvestUnitID_PositionDetail = 126,
};


string SERVICE_API GetTextById(UserInfo& ui, int field_id);							//函数原型声明,SERVICE_API说明该函数是需要导出的
string SERVICE_API GetTextById(BaseData& bd, int field_id);
string SERVICE_API GetTextById(InvestorPosition& ip, int field_id);
string SERVICE_API GetTextById(Order& o, int field_id);
string SERVICE_API GetTextById(Trade& t, int field_id);
string SERVICE_API GetTextById(InvestorPositionDetail& ipd, int field_id);
#endif