// StandardMakeOrderBoard.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "StandardMakeOrderBoard.h"
#include "../../ServiceLayer/DataTypes.h" //此处需要用到ctp服务层中的数据结构
#include "atlconv.h" //使用USES_CONVERSION需要用到该头文件
#include "../../../sdk/ThostFtdcUserApiDataType.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpMsg.h"


// CStandardMakeOrderBoard

IMPLEMENT_DYNCREATE(CStandardMakeOrderBoard, CFormView)

CStandardMakeOrderBoard::CStandardMakeOrderBoard(CWnd* pParent)
	: CFormView(CStandardMakeOrderBoard::IDD)
	, m_strInstrumentID(_T(""))
	, m_nInstrumentVolume(0)
	, m_strExchange(_T(""))
	, m_strTouchPriceType(_T(""))
	, m_InstrumentPrice(0)
	, m_TouchPrice(0)
	, m_TradePrice(0)
	, m_strTimeCondition(_T(""))
{//在下单板类的构造函数中创建下单板实体
	CCreateContext *pContext = NULL;
	if (!Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 300), pParent, 0, pContext))
	{
		AfxMessageBox(_T("创建下单板失败"));
	}
	OnInitialUpdate();
}

CStandardMakeOrderBoard::~CStandardMakeOrderBoard()
{
}

void CStandardMakeOrderBoard::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_CBString(pDX, COMBO_INSTRUMENTS, m_strInstrumentID);
	DDX_Text(pDX, EDIT_VOLUME, m_nInstrumentVolume);
	DDX_Control(pDX, BUTTON_PRICETYPE, m_btnPriceType);
	DDX_Control(pDX, COMBO_INSTRUMENTS, m_cbInstruments);
	DDX_Control(pDX, COMBO_EXCHANGE, m_cbExchanges);
	DDX_CBString(pDX, COMBO_EXCHANGE, m_strExchange);
	DDX_CBString(pDX, COMBO_TOUCHPRICETYPE, m_strTouchPriceType);
	DDX_Control(pDX, COMBO_TOUCHPRICETYPE, m_cbTouchPriceType);
	DDX_Text(pDX, EDIT_PRICE, m_InstrumentPrice);
	DDX_Text(pDX, EDIT_STOPPRICE, m_TouchPrice);
	DDX_Text(pDX, EDIT_TRADEPRICE, m_TradePrice);
	DDX_Control(pDX, COMBO_TimeCondition, m_cbTimeCondition);
	DDX_CBString(pDX, COMBO_TimeCondition, m_strTimeCondition);
	DDX_Control(pDX, DATETIMEPICKER_GTD, m_GTD);
	DDX_Control(pDX, BUTTON_TOUCHPRICE_PRICETYPE, m_btnPriceType_TouchPrice);
	DDX_Control(pDX, COMBO_PRICETYPE, m_cbPriceType);
	DDX_Control(pDX, SPIN_VOLUME, m_SpinVolumeChange);
	DDX_Control(pDX, COMBO_PRICETYPE_NORMAL, m_cbPriceType_Normal);
	DDX_Control(pDX, SPIN_PRICE, m_SpinPriceChange);
	DDX_Control(pDX, SPIN_TOUCHPRICE, m_SpinTouchPriceChange);
	DDX_Control(pDX, SPIN_PRICE_TOUCH, m_SpinPriceChange_Touch);
	DDX_Control(pDX, STATIC_GROUPBOX, m_gbPriceDetail);
}

BEGIN_MESSAGE_MAP(CStandardMakeOrderBoard, CFormView)
	ON_BN_CLICKED(BUTTON_ORDER_INSERT, &CStandardMakeOrderBoard::OnBnClickedOrderInsert)
	ON_BN_CLICKED(BUTTON_PRICETYPE, &CStandardMakeOrderBoard::OnBnClickedPricetype)
	ON_BN_CLICKED(BUTTON_TOUCHPRICE_PRICETYPE, &CStandardMakeOrderBoard::OnBnClickedTouchpricePricetype)
	ON_NOTIFY(UDN_DELTAPOS, SPIN_VOLUME, &CStandardMakeOrderBoard::OnDeltaposVolume)
	ON_EN_CHANGE(EDIT_VOLUME, &CStandardMakeOrderBoard::OnEnChangeVolume)
	ON_EN_CHANGE(EDIT_PRICE, &CStandardMakeOrderBoard::OnEnChangePrice)
	ON_NOTIFY(UDN_DELTAPOS, SPIN_PRICE, &CStandardMakeOrderBoard::OnDeltaposPrice)
	ON_NOTIFY(UDN_DELTAPOS, SPIN_TOUCHPRICE, &CStandardMakeOrderBoard::OnDeltaposTouchprice)
	ON_NOTIFY(UDN_DELTAPOS, SPIN_PRICE_TOUCH, &CStandardMakeOrderBoard::OnDeltaposPriceTouch)
	ON_EN_CHANGE(EDIT_STOPPRICE, &CStandardMakeOrderBoard::OnEnChangeStopprice)
	ON_EN_CHANGE(EDIT_TRADEPRICE, &CStandardMakeOrderBoard::OnEnChangeTradeprice)
	ON_BN_CLICKED(BUTTON_RETSET, &CStandardMakeOrderBoard::OnBnClickedRetset)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_UPDATETICKDETAILDISP, &CStandardMakeOrderBoard::OnUpdateTickDetailDisp)//将消息与消息映射函数相绑定
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CStandardMakeOrderBoard diagnostics

#ifdef _DEBUG
void CStandardMakeOrderBoard::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CStandardMakeOrderBoard::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CStandardMakeOrderBoard message handlers


void CStandardMakeOrderBoard::OnBnClickedOrderInsert()
{
	UpdateData(TRUE);	//将控件里的数据刷新到绑定的控件变量中
	USES_CONVERSION;
	InputOrderParam order_param;
	InputParkedOrderParam parkedorder_param;
	//填充合约
	order_param.InstrumentID =T2A(m_strInstrumentID);//T2A将mfc的cstring类型转换为char*类型
	parkedorder_param.InstrumentID = T2A(m_strInstrumentID);

	//填充交易所
	order_param.ExchangeID = T2A(m_strExchange);//T2A将mfc的cstring类型转换为char*类型
	parkedorder_param.ExchangeID = T2A(m_strExchange);

	//填充方向
	CButton* pBuyButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_BUY));
	if (pBuyButton != NULL)
	{//检查买按钮是否被选定，通过GetCheck() == BST_CHECKED来比较，因为单选按钮存在默认按钮，不会出现未选的状况，因此对于二选项的单选按钮组合，只需判断其中一个即可
		order_param.Direction = pBuyButton->GetCheck() == BST_CHECKED ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
		parkedorder_param.Direction = pBuyButton->GetCheck() == BST_CHECKED ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	}
	else
	{
		ASSERT(FALSE); //若产生错误，则中断执行
	}

	//填充开平标志
	CButton* pOpenCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_OPEN));
	if (pOpenCloseButton->GetCheck() == BST_CHECKED)
	{
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_Open; //标记为开仓
		parkedorder_param.CombOffsetFlag[0] = THOST_FTDC_OF_Open; //标记为开仓
	}
	pOpenCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_CLOSE));
	if (pOpenCloseButton->GetCheck() == BST_CHECKED)
	{
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_Close; //标记为平仓
		parkedorder_param.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
	}
	pOpenCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_CLOSETODAY));
	if (pOpenCloseButton->GetCheck() == BST_CHECKED)
	{
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;  //标记为平今
		parkedorder_param.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
	}

	//填充手数
	order_param.VolumeTotalOriginal = m_nInstrumentVolume;
	parkedorder_param.VolumeTotalOriginal = m_nInstrumentVolume;

	//填充价格类型
	order_param.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //默认是限价，按钮的默认值也是限价
	parkedorder_param.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	CString priceTypeName;
	m_btnPriceType.GetWindowText(priceTypeName); //获取价格类型按钮的标签值
	if (priceTypeName == "市价")
	{
		order_param.OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //为立即市价下单
		parkedorder_param.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	}

	//填充触发类型
	CButton* pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_COMMISSION));
	if (pCommissionORAdvanceButton->GetCheck() == BST_CHECKED)
	{//若选了委托按钮，则触发类型为立即限价/市价委托
		order_param.ContingentCondition = THOST_FTDC_CC_Immediately; //限价单与市价单的默认触发类型都是立即触发，即这里是立即限价单及立即市价单
	}
	pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_ADVANCE));
	if (pCommissionORAdvanceButton->GetCheck() == BST_CHECKED)
	{//若选了预埋按钮，则触发类型为立即限价/市价预埋
		parkedorder_param.ContingentCondition = THOST_FTDC_CC_Immediately; 
	}

	//记录价格
	order_param.LimitPrice = m_InstrumentPrice; //填充报单价格
	parkedorder_param.LimitPrice = m_InstrumentPrice;

	//处理有效期类型
	order_param.TimeCondition = THOST_FTDC_TC_GFD; //默认当日有效
	parkedorder_param.TimeCondition = THOST_FTDC_TC_GFD;

	//处理触价条件单
	if (BST_CHECKED == IsDlgButtonChecked(CHECK_TOUCHPRICETYPE)) //触价交易是否被选中，若被选中，则为限价类型的同时，也要修改确定其触发条件类型
	{
		order_param.StopPrice = m_TouchPrice; //填充触发价格
		order_param.LimitPrice = m_TradePrice;//填充报单价格
		//处理触发条件
		if (strcmp(T2A(m_strTouchPriceType), "BuyStop") == 0)
		{
			order_param.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
			order_param.Direction = THOST_FTDC_D_Buy;
		}
		if (strcmp(T2A(m_strTouchPriceType), "SellStop") == 0)
		{
			order_param.ContingentCondition = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
			order_param.Direction = THOST_FTDC_D_Sell;
		}
		if (strcmp(T2A(m_strTouchPriceType), "BuyLimit") == 0)
		{
			order_param.ContingentCondition = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
			order_param.Direction = THOST_FTDC_D_Buy;
		}
		if (strcmp(T2A(m_strTouchPriceType), "SellLimit") == 0)
		{
			order_param.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
			order_param.Direction = THOST_FTDC_D_Sell;
		}
		//处理有效期类型
		if (strcmp(T2A(m_strTimeCondition), "当日有效") == 0)
		{
			order_param.TimeCondition = THOST_FTDC_TC_GFD;
		}
		if (strcmp(T2A(m_strTimeCondition), "撤销前有效") == 0)
		{
			order_param.TimeCondition = THOST_FTDC_TC_GTC;
		}
		if (strcmp(T2A(m_strTimeCondition), "指定日期前有效") == 0)
		{
			order_param.TimeCondition = THOST_FTDC_TC_GTD;
			CTime Date;
			m_GTD.GetTime(Date);
			CString strTime = Date.Format("%Y%m%d");
			order_param.GTDDate = T2A(strTime); //T2A将cstring类型转换为char*类型，之后再将char*类型赋给string类型
		}
		//根据了解，ctp目前只支持当日有效，为此先留下给接口，由于默认为当日有效，GTD字段也不需要
		order_param.TimeCondition = THOST_FTDC_TC_GFD;

		//处理价格类型，默认触价后转限价，也可触价后转市价
		order_param.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		CString priceTypeName_TouchPrice;
		m_btnPriceType_TouchPrice.GetWindowText(priceTypeName_TouchPrice); 
		if (priceTypeName_TouchPrice == "市价")
		{
			order_param.OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //触价后转市价
		}

		//此处的条件单仅限开仓，若为平仓，将做成止损止盈单，不在此处处理
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_Open; //标记为开仓
	}
	if (::MessageBox(NULL, _T("是否确认下单"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		if (BST_CHECKED == IsDlgButtonChecked(CHECK_TOUCHPRICETYPE))
		{//触价交易被选中，下委托单
			get_global_mgr().GetService()->request(ctp_msg::MakeOrder, &order_param);
			return;
		}
		pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_COMMISSION));
		if (pCommissionORAdvanceButton->GetCheck() == BST_CHECKED)
		{//若勾选委托，则发委托单
			get_global_mgr().GetService()->request(ctp_msg::MakeOrder, &order_param);
			return;
		}
		pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_ADVANCE));
		if (pCommissionORAdvanceButton->GetCheck() == BST_CHECKED)
		{//若勾选了预埋，则发预埋单
			get_global_mgr().GetService()->request(ctp_msg::Make_ParkedOrder, &parkedorder_param);
			return;
		}
	}
}


void CStandardMakeOrderBoard::OnBnClickedPricetype()
{
	CString priceTypeName;
	m_btnPriceType.GetWindowText(priceTypeName); //将价格类型按钮的值通过m_btnPriceType.GetWindowText获取到priceTypeName变量中
	if (priceTypeName == "限价")
	{//当该按钮按下去后，若先前为限价，则按下后改为市价
		//order_param.OrderPriceType=THOST_FTDC_OPT_AnyPrice;
		m_btnPriceType.SetWindowText(_T("市价"));
	}
	else
	{
		m_btnPriceType.SetWindowText(_T("限价"));
	}
}


void CStandardMakeOrderBoard::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	get_CtpMsgDistributor()->AddReceiver(this);//将CStandardMakeOrderBoard类实例添加进接收者容器中

	//在方向单选按钮组中，默认选定买按钮
	CButton* pBuyButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_BUY));
	pBuyButton->SetCheck(true);

	//在开平单选按钮组中，默认选定开仓按钮
	CButton* pOpenCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_OPEN));
	pOpenCloseButton->SetCheck(true);

	//在委托与预埋单选按钮组中，默认选定委托按钮
	CButton* pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_COMMISSION));
	pCommissionORAdvanceButton->SetCheck(true);

	//借助唯一的get_global_mgr外部接口去获取ctpservice对象, 进而获取交易api对象及行情api对象, 由其来获取相应的信息
	vector<BaseData> instruments;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_basic_data(instruments);
		if (!instruments.empty())
		{//在这里, 把数据填充到组合框中去
			if (m_cbInstruments.GetCount() != 0)
			{//防止二次加载,为此再加载前若不为空,先清除所有先
				m_cbInstruments.ResetContent();
			}
			for (int i = 0; i < instruments.size(); ++i)
			{
				m_cbInstruments.AddString(CString(instruments[i].code));
			}
		}
	}

	vector<Exchange> exchanges;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_exchanges(exchanges);
		if (!exchanges.empty())
		{//在这里, 把数据填充到组合框中去
			if (m_cbExchanges.GetCount() != 0)
			{//防止二次加载,为此再加载前若不为空,先清除所有先
				m_cbExchanges.ResetContent();
			}
			for (int j = 0; j <exchanges.size(); ++j)
			{
				m_cbExchanges.AddString(CString(exchanges[j].ExchangeID.c_str()));
			}
			m_cbExchanges.SetCurSel(0);
		}
	}

	if (m_cbTouchPriceType.GetCount()!=0)
	{//重置为空，防止二次重复加载
		m_cbTouchPriceType.ResetContent();
	}
	m_cbTouchPriceType.AddString(CString("BuyStop"));
	m_cbTouchPriceType.AddString(CString("SellStop"));
	m_cbTouchPriceType.AddString(CString("BuyLimit"));
	m_cbTouchPriceType.AddString(CString("SellLimit"));
	m_cbTouchPriceType.SetCurSel(0);

	if (m_cbTimeCondition.GetCount()!=0)
	{
		m_cbTimeCondition.ResetContent();
	}
	m_cbTimeCondition.AddString(CString("当日有效"));
	m_cbTimeCondition.AddString(CString("指定日期前有效"));
	m_cbTimeCondition.AddString(CString("撤销前有效"));
	m_cbTimeCondition.SetCurSel(1);

	if (m_cbPriceType.GetCount() != 0)
	{
		m_cbPriceType.ResetContent();
	}
	m_cbPriceType.AddString(CString("指定报单价"));
	//m_cbPriceType.AddString(CString("跟盘价"));//条件单没有跟盘价选项
	m_cbPriceType.SetCurSel(0);

	if (m_cbPriceType_Normal.GetCount() != 0)
	{
		m_cbPriceType_Normal.ResetContent();
	}
	m_cbPriceType_Normal.AddString(CString("指定价"));
	m_cbPriceType_Normal.AddString(CString("跟盘价")); 
	m_cbPriceType_Normal.SetCurSel(1);

	CString cstr;
	cstr.Format(_T("%d"), 1);
	GetDlgItem(EDIT_VOLUME)->SetWindowText(cstr);
	GetDlgItem(EDIT_PRICE)->SetWindowText(cstr);
	GetDlgItem(EDIT_STOPPRICE)->SetWindowText(cstr);
	GetDlgItem(EDIT_TRADEPRICE)->SetWindowText(cstr);

	m_SpinVolumeChange.SetBuddy(GetDlgItem(m_nInstrumentVolume)); //绑定文本框与交易量变动按钮的伙伴关系
	m_SpinVolumeChange.SetRange32(0, 1000);//表示数值只能在0到10内变化
	m_SpinVolumeChange.SetBase(10);//设置进制数,只能是10进制和16进制

	m_SpinPriceChange.SetBuddy(GetDlgItem(m_InstrumentPrice)); //绑定价格文本框与价格变动按钮的伙伴关系
	m_SpinPriceChange.SetRange32(0, 999999);//表示数值只能在0到999999内变化
	m_SpinPriceChange.SetBase(10);//设置进制数,只能是10进制和16进制

	m_SpinTouchPriceChange.SetBuddy(GetDlgItem(m_TouchPrice)); //绑定触及价格文本框与价格变动按钮的伙伴关系
	m_SpinTouchPriceChange.SetRange32(0, 999999);//表示数值只能在0到999999内变化
	m_SpinTouchPriceChange.SetBase(10);//设置进制数,只能是10进制和16进制

	m_SpinPriceChange_Touch.SetBuddy(GetDlgItem(m_TradePrice)); //绑定触及报单价格文本框与价格变动按钮的伙伴关系
	m_SpinPriceChange_Touch.SetRange32(0, 999999);//表示数值只能在0到999999内变化
	m_SpinPriceChange_Touch.SetBase(10);//设置进制数,只能是10进制和16进制

	SetWindowTheme(GetDlgItem(STATIC_GROUPBOX)->GetSafeHwnd(), L"", L"");//用于设置Group Box控件用的，具体缘由未深究
	SetWindowTheme(GetDlgItem(STATIC_PRICEDETAIL_SUBBOX1)->GetSafeHwnd(), L"", L""); 
	SetWindowTheme(GetDlgItem(STATIC_PRICEDETAIL_SUBBOX2)->GetSafeHwnd(), L"", L""); 
	SetWindowTheme(GetDlgItem(STATIC_PRICEDETAIL_SUBBOX3)->GetSafeHwnd(), L"", L"");


	CFont font_Ask;
	font_Ask.CreatePointFont(120, L"楷体");
	GetDlgItem(STATIC_ASK)->SetFont(&font_Ask);
	CFont font_Bid;
	font_Bid.CreatePointFont(120, L"楷体");
	GetDlgItem(STATIC_BID)->SetFont(&font_Bid);
	CFont font_AskValue;
	font_AskValue.CreatePointFont(100, L"楷体");
	GetDlgItem(STATIC_ASKVALUE)->SetFont(&font_AskValue);
	CFont font_BidValue;
	font_BidValue.CreatePointFont(100, L"楷体");
	GetDlgItem(STATIC_BIDVALUE)->SetFont(&font_BidValue);
	CFont font_AskVolume;
	font_AskVolume.CreatePointFont(100, L"楷体");
	GetDlgItem(STATIC_ASKVOLUME)->SetFont(&font_AskVolume);
	CFont font_BidVolume;
	font_BidVolume.CreatePointFont(100, L"楷体");
	GetDlgItem(STATIC_BIDVOLUME)->SetFont(&font_BidVolume);

	CFont font;
	font.CreatePointFont(120, L"楷体");
	GetDlgItem(STATIC_LASTPRICE)->SetFont(&font);
	GetDlgItem(STATIC_PreSettlementPrice)->SetFont(&font);
	GetDlgItem(STATIC_SettlementPrice)->SetFont(&font);
	GetDlgItem(STATIC_PreClosePrice)->SetFont(&font);
	GetDlgItem(STATIC_UpperLimitPrice)->SetFont(&font);
	GetDlgItem(STATIC_OpenPrice)->SetFont(&font);
	GetDlgItem(STATIC_LowerLimitPrice)->SetFont(&font);
	GetDlgItem(STATIC_HighestPrice)->SetFont(&font);
	GetDlgItem(STATIC_LowestPrice)->SetFont(&font);
	GetDlgItem(STATIC_RANGE)->SetFont(&font);
	GetDlgItem(STATIC_LASTPRICE_VALUE)->SetFont(&font);
	GetDlgItem(STATIC_PreSettlementPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_SettlementPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_PreClosePrice_value)->SetFont(&font);
	GetDlgItem(STATIC_UpperLimitPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_OpenPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_LowerLimitPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_HighestPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_LowestPrice_value)->SetFont(&font);
	GetDlgItem(STATIC_RANGE_value)->SetFont(&font);
	GetDlgItem(STATIC_tickInstrument)->SetFont(&font);
	GetDlgItem(STATIC_tickExchange)->SetFont(&font);


}

void CStandardMakeOrderBoard::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryInstrument)
	{//当交易api查询到所有合约信息并回调返回的时候, 我们在此发送WM_INITIALUPDATE消息, 让OnInitialUpdate函数去更新复选框
		PostMessage(WM_INITIALUPDATE,  0, 0); //WM_INITIALUPDATE消息的发送会触发CStandardMakeOrderBoard::OnInitialUpdate()函数的调用
	}
	if (wParam == ctp_msg::RspQryexchanges)
	{
		PostMessage(WM_INITIALUPDATE, 0, 0);//发送消息去更新交易所复选框
	}
	if (wParam==ctp_msg::RtnMarketData)
	{
		PostMessage(WM_UPDATETICKDETAILDISP, RtnMarketData, lParam);
	}
}



void CStandardMakeOrderBoard::OnBnClickedTouchpricePricetype()
{
	CString priceTypeName;
	m_btnPriceType_TouchPrice.GetWindowText(priceTypeName); 
	if (priceTypeName == "限价")
	{//当该按钮按下去后，若先前为限价，则按下后改为市价
		m_btnPriceType_TouchPrice.SetWindowText(_T("市价"));
	}
	else
	{
		m_btnPriceType_TouchPrice.SetWindowText(_T("限价"));
	}
}






void CStandardMakeOrderBoard::OnDeltaposVolume(NMHDR *pNMHDR, LRESULT *pResult)
{//智能控制交易量的边界范围
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true); //将数据刷新到值变量中
	USES_CONVERSION;
	UINT  maxVolume = 1000;
	UINT  minVolume = 1;
	char* InstrumentID = T2A(m_strInstrumentID);//T2A将mfc的cstring类型转换为char*类型
	CString priceTypeName;
	m_btnPriceType.GetWindowText(priceTypeName); //获取价格类型按钮的标签值
	bool IsAnyPrice = false; //默认非市价,而是限价
	if (priceTypeName == "市价")
	{
		IsAnyPrice=true;  //为市价下单
	}

	if (strlen(InstrumentID) != 0)
	{
		vector<BaseData> instruments;
		if (NULL != get_global_mgr().GetService())
		{
			get_global_mgr().GetService()->get_basic_data(instruments);
			for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (strcmp(it->code,InstrumentID)==0)
				{
					if (IsAnyPrice==false)
					{
						maxVolume = it->MaxLimitOrderVolume;
						minVolume = it->MinLimitOrderVolume;
						break;
					}
					if (IsAnyPrice==true)
					{
						maxVolume = it->MaxMarketOrderVolume;
						minVolume = it->MinMarketOrderVolume;
						break;
					}
				}
			}
		}
		UINT calculateVolume;
		if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往下的箭头
		{
			calculateVolume = m_nInstrumentVolume - 1;
			if (calculateVolume < minVolume)  calculateVolume = minVolume;
			if (calculateVolume > maxVolume)  calculateVolume = maxVolume;
		}
		else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
		{
			calculateVolume = m_nInstrumentVolume + 1;
			if (calculateVolume > maxVolume)  calculateVolume = maxVolume;
			if (calculateVolume < minVolume)  calculateVolume = minVolume;
		}
		m_nInstrumentVolume = calculateVolume;
		CString volume_cstr;
		volume_cstr.Format(_T("%d"), m_nInstrumentVolume);
		GetDlgItem(EDIT_VOLUME)->SetWindowText(volume_cstr);
	}
	else
	{
		UINT calculateVolume;
		if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往下的箭头
		{
			calculateVolume = m_nInstrumentVolume - 1;
			if (calculateVolume < minVolume)  calculateVolume = minVolume;
			if (calculateVolume > maxVolume)  calculateVolume = maxVolume;
		}
		else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
		{
			calculateVolume = m_nInstrumentVolume + 1;
			if (calculateVolume < minVolume)  calculateVolume = minVolume;
			if (calculateVolume > maxVolume)  calculateVolume = maxVolume;
		}
		m_nInstrumentVolume = calculateVolume;
		CString volume_cstr;
		volume_cstr.Format(_T("%d"), m_nInstrumentVolume);
		GetDlgItem(EDIT_VOLUME)->SetWindowText(volume_cstr);
	}
	//UpdateData(false);
	*pResult = 0;
}


void CStandardMakeOrderBoard::OnEnChangeVolume()
{//处理各种异常输入
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_VOLUME)));
	editHelp->GetWindowText(strTemp);
	int len = strTemp.GetLength();
	for (int i = 0; i < len; i++)
	{
		if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
		{//限定输入必须是数字
			strTemp = strTemp.Left(i);
			editHelp->SetWindowText(strTemp);
			editHelp->SetSel(i, i, TRUE);
			return;
		}
	}
	editHelp->GetWindowText(strTemp);
	if (strTemp==_T("0"))
	{
		editHelp->SetWindowText(_T("1"));
	}
}


void CStandardMakeOrderBoard::OnEnChangePrice()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_PRICE)));
	editHelp->GetWindowText(strTemp);
	int len = strTemp.GetLength();
	for (int i = 0; i < len; i++)
	{
		if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9' )
		{//限定输入必须是数字
			if (strTemp.GetAt(i) == '.')
			{//价格数据要避开小数点
				continue;
			}
			strTemp = strTemp.Left(i);
			editHelp->SetWindowText(strTemp);
			editHelp->SetSel(i, i, TRUE);
			return;
		}
	}
	editHelp->GetWindowText(strTemp);
	if (strTemp == _T("0"))
	{
		editHelp->SetWindowText(_T("1"));
	}
}


void CStandardMakeOrderBoard::OnDeltaposPrice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true); //将数据刷新到值变量中
	USES_CONVERSION;
	double  pricechange_unit = 1;
	char* InstrumentID = T2A(m_strInstrumentID);//T2A将mfc的cstring类型转换为char*类型

	if (strlen(InstrumentID) != 0)
	{
		vector<BaseData> instruments;
		if (NULL != get_global_mgr().GetService())
		{
			get_global_mgr().GetService()->get_basic_data(instruments);
			for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (strcmp(it->code, InstrumentID) == 0)
				{
					pricechange_unit = it->PriceTick;
					break;
				}
			}
		}
		double changedPrice;
		if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往下的箭头
		{
			changedPrice = m_InstrumentPrice - pricechange_unit;
			if (changedPrice <= 0)  changedPrice = pricechange_unit;
		}
		else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
		{
			changedPrice = m_InstrumentPrice + pricechange_unit;
			if (changedPrice <= 0)  changedPrice = pricechange_unit;
		}
		m_InstrumentPrice = changedPrice;
		CString Price_cstr;
		Price_cstr.Format(_T("%0.2f"), m_InstrumentPrice);
		GetDlgItem(EDIT_PRICE)->SetWindowText(Price_cstr);
	}
	else
	{
		//不做任何操作
	}
	//UpdateData(false);
	*pResult = 0;
}


void CStandardMakeOrderBoard::OnDeltaposTouchprice(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true); //将数据刷新到值变量中
	USES_CONVERSION;
	double  touchpricechange_unit = 1;
	char* InstrumentID = T2A(m_strInstrumentID);//T2A将mfc的cstring类型转换为char*类型

	if (strlen(InstrumentID) != 0)
	{
		vector<BaseData> instruments;
		if (NULL != get_global_mgr().GetService())
		{
			get_global_mgr().GetService()->get_basic_data(instruments);
			for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (strcmp(it->code, InstrumentID) == 0)
				{
					touchpricechange_unit = it->PriceTick;
					break;
				}
			}
		}
		double changedPrice;
		if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往下的箭头
		{
			changedPrice = m_TouchPrice - touchpricechange_unit;
			if (changedPrice <= 0)  changedPrice = touchpricechange_unit;
		}
		else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
		{
			changedPrice = m_TouchPrice + touchpricechange_unit;
			if (changedPrice <= 0)  changedPrice = touchpricechange_unit;
		}
		m_TouchPrice = changedPrice;
		CString Price_cstr;
		Price_cstr.Format(_T("%0.2f"), m_TouchPrice);
		GetDlgItem(EDIT_STOPPRICE)->SetWindowText(Price_cstr);
	}
	else
	{
		//不做任何操作
	}
	//UpdateData(false);
	*pResult = 0;
}


void CStandardMakeOrderBoard::OnDeltaposPriceTouch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData(true); //将数据刷新到值变量中
	USES_CONVERSION;
	double  pricechange_touch_unit = 1;
	char* InstrumentID = T2A(m_strInstrumentID);//T2A将mfc的cstring类型转换为char*类型

	if (strlen(InstrumentID) != 0)
	{
		vector<BaseData> instruments;
		if (NULL != get_global_mgr().GetService())
		{
			get_global_mgr().GetService()->get_basic_data(instruments);
			for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (strcmp(it->code, InstrumentID) == 0)
				{
					pricechange_touch_unit = it->PriceTick;
					break;
				}
			}
		}
		double changedPrice;
		if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往下的箭头
		{
			changedPrice = m_TradePrice - pricechange_touch_unit;
			if (changedPrice <= 0)  changedPrice = pricechange_touch_unit;
		}
		else if (pNMUpDown->iDelta == 1) // 如果此值为1, 说明点击了Spin的往上的箭头
		{
			changedPrice = m_TradePrice + pricechange_touch_unit;
			if (changedPrice <= 0)  changedPrice = pricechange_touch_unit;
		}
		m_TradePrice = changedPrice;
		CString Price_cstr;
		Price_cstr.Format(_T("%0.2f"), m_TradePrice);
		GetDlgItem(EDIT_TRADEPRICE)->SetWindowText(Price_cstr);
	}
	else
	{
		//不做任何操作
	}
	//UpdateData(false);
	*pResult = 0;
}


void CStandardMakeOrderBoard::OnEnChangeStopprice()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_STOPPRICE)));
	editHelp->GetWindowText(strTemp);
	int len = strTemp.GetLength();
	for (int i = 0; i < len; i++)
	{
		if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
		{//限定输入必须是数字
			if (strTemp.GetAt(i) == '.')
			{//价格数据要避开小数点
				continue;
			}
			strTemp = strTemp.Left(i);
			editHelp->SetWindowText(strTemp);
			editHelp->SetSel(i, i, TRUE);
			return;
		}
	}
	editHelp->GetWindowText(strTemp);
	if (strTemp == _T("0"))
	{
		editHelp->SetWindowText(_T("1"));
	}
}


void CStandardMakeOrderBoard::OnEnChangeTradeprice()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the __super::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_TRADEPRICE)));
	editHelp->GetWindowText(strTemp);
	int len = strTemp.GetLength();
	for (int i = 0; i < len; i++)
	{
		if (strTemp.GetAt(i) < '0' || strTemp.GetAt(i) > '9')
		{//限定输入必须是数字
			if (strTemp.GetAt(i) == '.')
			{//价格数据要避开小数点
				continue;
			}
			strTemp = strTemp.Left(i);
			editHelp->SetWindowText(strTemp);
			editHelp->SetSel(i, i, TRUE);
			return;
		}
	}
	editHelp->GetWindowText(strTemp);
	if (strTemp == _T("0"))
	{
		editHelp->SetWindowText(_T("1"));
	}
}


void CStandardMakeOrderBoard::OnBnClickedRetset()
{
	// TODO: Add your control notification handler code here
	//在委托与预埋单选按钮组中，默认选定委托按钮
	CButton* pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_COMMISSION));
	pCommissionORAdvanceButton->SetCheck(true);
	pCommissionORAdvanceButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_ADVANCE));
	pCommissionORAdvanceButton->SetCheck(false);

	//重置价格和交易量
	CString cstr;
	cstr.Format(_T("%d"), 1);
	GetDlgItem(EDIT_VOLUME)->SetWindowText(cstr);
	GetDlgItem(EDIT_PRICE)->SetWindowText(cstr);
	GetDlgItem(EDIT_STOPPRICE)->SetWindowText(cstr);
	GetDlgItem(EDIT_TRADEPRICE)->SetWindowText(cstr);
	//触价交易勾选框默认不选中
	((CButton*)GetDlgItem(CHECK_TOUCHPRICETYPE))->SetCheck(false);
}


HBRUSH CStandardMakeOrderBoard::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{//实现GroupBox的上端颜色设置，实际上尚未真正解决问题，取巧而为
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  Change any attributes of the DC here
	//针对不同控件修改颜色
	if (pWnd->GetDlgCtrlID() == STATIC_PRICEDETAIL_SUBBOX1)
	{
	hbr = CreateSolidBrush(RGB(255, 0, 0));//背景颜色
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->SetBkColor(RGB(255, 0, 0));
	}

	if (pWnd->GetDlgCtrlID() == STATIC_PRICEDETAIL_SUBBOX3)
	{
		hbr = CreateSolidBrush(RGB(0, 0, 255));//背景颜色
		pDC->SetTextColor(RGB(0, 0, 255));
		pDC->SetBkColor(RGB(0, 0, 255));
	}

	if (pWnd->GetDlgCtrlID() == STATIC_ASK)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	if (pWnd->GetDlgCtrlID() == STATIC_BID)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	if (pWnd->GetDlgCtrlID() == STATIC_LASTPRICE ||
		pWnd->GetDlgCtrlID() == STATIC_SettlementPrice ||
		pWnd->GetDlgCtrlID() == STATIC_UpperLimitPrice ||
		pWnd->GetDlgCtrlID() == STATIC_LowerLimitPrice ||
		pWnd->GetDlgCtrlID() == STATIC_RANGE)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	if (pWnd->GetDlgCtrlID() == STATIC_PreSettlementPrice  ||
		pWnd->GetDlgCtrlID() == STATIC_PreClosePrice ||
		pWnd->GetDlgCtrlID() == STATIC_OpenPrice ||
		pWnd->GetDlgCtrlID() == STATIC_HighestPrice ||
		pWnd->GetDlgCtrlID() == STATIC_LowestPrice)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	if (pWnd->GetDlgCtrlID() == STATIC_tickInstrument ||
		pWnd->GetDlgCtrlID() == STATIC_tickExchange)
	{
		pDC->SetTextColor(RGB(255, 128, 64));
	}
	
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

LRESULT  CStandardMakeOrderBoard::OnUpdateTickDetailDisp(WPARAM wParam, LPARAM lParam)
{
	CString instrumentID;
	CString InstrumentID_combobox;
	CString cstr;
	if (wParam==ctp_msg::RtnMarketData)
	{
		InstrumentID_combobox = _T("");//重置
		instrumentID = _T("");//重置
		cstr = _T("");//重置
		GetDlgItem(COMBO_INSTRUMENTS)->GetWindowText(InstrumentID_combobox);
		if (InstrumentID_combobox == _T(""))//m_strInstrumentID为合约框的值变量
		{
			return 0;
		}
		else
		{//有选定合约
			MarketData* pmarketData = reinterpret_cast<MarketData*>(lParam);
			MarketData marketData = *pmarketData;
			instrumentID = marketData.code;
			if (InstrumentID_combobox == instrumentID)
			{//此处根据行情推送做更新操作
				cstr = marketData.code;//tick合约
				GetDlgItem(STATIC_tickInstrument)->SetWindowText(cstr);
				cstr = marketData.ExchangeID;//tick交易所
				GetDlgItem(STATIC_tickExchange)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.AskPrice);
				GetDlgItem(STATIC_ASKVALUE)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.BidPrice);
				GetDlgItem(STATIC_BIDVALUE)->SetWindowText(cstr);
				cstr.Format(_T("%d"), marketData.AskVolume);
				GetDlgItem(STATIC_ASKVOLUME)->SetWindowText(cstr);
				cstr.Format(_T("%d"), marketData.BidVolume);
				GetDlgItem(STATIC_BIDVOLUME)->SetWindowText(cstr);

				cstr.Format(_T("%.2f"), marketData.LastPrice);
				GetDlgItem(STATIC_LASTPRICE_VALUE)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.PreSettlementPrice);
				GetDlgItem(STATIC_PreSettlementPrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.LastPrice-marketData.PreClosePrice);
				GetDlgItem(STATIC_changeprice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.PreClosePrice);
				GetDlgItem(STATIC_PreClosePrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.UpperLimitPrice);
				GetDlgItem(STATIC_UpperLimitPrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.OpenPrice);
				GetDlgItem(STATIC_OpenPrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.LowerLimitPrice);
				GetDlgItem(STATIC_LowerLimitPrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.HighestPrice);
				GetDlgItem(STATIC_HighestPrice_value)->SetWindowText(cstr);
				cstr.Format(_T("%.2f"), marketData.LowestPrice);
				GetDlgItem(STATIC_LowestPrice_value)->SetWindowText(cstr);
				double rangevalue;
				if (marketData.PreClosePrice!=0)
				{
					rangevalue = (marketData.LastPrice - marketData.PreClosePrice) / marketData.PreClosePrice *100;//乘以100是为了考虑百分号显示
					cstr.Format(_T("%.2f%%"), rangevalue);
					GetDlgItem(STATIC_RANGE_value)->SetWindowText(cstr);
				}
				else
				{
					GetDlgItem(STATIC_RANGE_value)->SetWindowText(_T("------"));
				}
				//设置跟盘价
				CString SpecORMarkPrice;
				((CComboBox*)GetDlgItem(COMBO_PRICETYPE_NORMAL))->GetWindowText(SpecORMarkPrice);
				if (SpecORMarkPrice==_T("跟盘价"))
				{
					CButton* pBuyButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_BUY));
					if (pBuyButton->GetCheck() == BST_CHECKED)
					{//方向选中为买
						CString price;
						price.Format(_T("%.2f"), marketData.AskPrice);
						GetDlgItem(EDIT_PRICE)->SetWindowText(price);
					}
					else
					{//方向选中为卖
						CString price;
						price.Format(_T("%.2f"), marketData.BidPrice);
						GetDlgItem(EDIT_PRICE)->SetWindowText(price);
					}
				}
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
}





void CStandardMakeOrderBoard::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	__super::OnClose();
}
