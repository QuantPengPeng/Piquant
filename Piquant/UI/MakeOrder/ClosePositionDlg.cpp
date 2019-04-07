// ClosePositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "ClosePositionDlg.h"
#include "afxdialogex.h"
#include "../resource.h"
#include "atlconv.h" //使用USES_CONVERSION需要用到该头文件
#include "../../ServiceLayer/DataTypes.h"
#include "../../../sdk/ThostFtdcUserApiDataType.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsg.h"

// CClosePositionDlg dialog

IMPLEMENT_DYNAMIC(CClosePositionDlg, CDialogEx)

CClosePositionDlg::CClosePositionDlg(CString InstrumentID,
														CString Direction,
														CString Position,
														CString PrePosition,
														CString TodayPosition,
														CString SettlementPrice,
														CWnd* pParent /*=NULL*/)
	: CDialogEx(CClosePositionDlg::IDD, pParent)
	, m_strInstrument(_T(""))
	, m_strDirection(_T(""))
	, m_strVolume(_T(""))
	, m_Price(0)
	, m_strExchange(_T(""))
{
	m_strInstrument = InstrumentID;
	m_strDirection = Direction;
	m_strVolume = Position;//此处传进来的是持仓手数
	this->SettlementPrice = SettlementPrice;
}

CClosePositionDlg::~CClosePositionDlg()
{
}

void CClosePositionDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RtnMarketData)
	{
		MarketData* pMarketData = reinterpret_cast<MarketData*>(lParam);
		MarketData marketData = *pMarketData;
		CString InstrumentID_Dlg;
		GetDlgItem(EDIT_INSTRUMENT)->GetWindowText(InstrumentID_Dlg);
		CString InstrumentID(marketData.code);
		if (InstrumentID_Dlg == InstrumentID)
		{
			CString SpecORMarkPrice;
			((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->GetWindowText(SpecORMarkPrice);
			if (SpecORMarkPrice == _T("跟盘报单价"))
			{
				CString CloseDirection;
				GetDlgItem(EDIT_DIRECTION)->GetWindowText(CloseDirection);
				if (CloseDirection==_T("多"))
				{//平仓方向选中为买
					CString price;
					price.Format(_T("%.2f"), marketData.AskPrice);
					GetDlgItem(EDIT_PRICE)->SetWindowText(price);
				}
				else
				{//平仓方向选中为卖
					CString price;
					price.Format(_T("%.2f"), marketData.BidPrice);
					GetDlgItem(EDIT_PRICE)->SetWindowText(price);
				}
			}
		}
	}
}

void CClosePositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_INSTRUMENT, m_strInstrument);
	DDX_Text(pDX, EDIT_DIRECTION, m_strDirection);
	DDX_Text(pDX, EDIT_VOLUME, m_strVolume);
	DDX_Control(pDX, BUTTON_PRICETYPE, m_bnPriceType);
	DDX_Text(pDX, EDIT_PRICE, m_Price);
	DDX_CBString(pDX, COMBO_EXCHANGE, m_strExchange);
	DDX_Control(pDX, COMBO_EXCHANGE, m_cbExchange);
}


BEGIN_MESSAGE_MAP(CClosePositionDlg, CDialogEx)
	ON_BN_CLICKED(BUTTON_PRICETYPE, &CClosePositionDlg::OnBnClickedPricetype)
	ON_BN_CLICKED(BUTTON_CLOSE, &CClosePositionDlg::OnBnClickedClose)
	ON_EN_CHANGE(EDIT_VOLUME, &CClosePositionDlg::OnEnChangeVolume)
	ON_EN_CHANGE(EDIT_PRICE, &CClosePositionDlg::OnEnChangePrice)
	ON_EN_CHANGE(EDIT_TOUCHPRICE, &CClosePositionDlg::OnEnChangeTouchprice)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CClosePositionDlg::OnInitDialog()
{
	get_CtpMsgDistributor()->AddReceiver(this);
	GetDlgItem(EDIT_INSTRUMENT)->SetWindowText(m_strInstrument); //给合约框设默认值
	GetDlgItem(EDIT_PRICE)->SetWindowText(_T("0")); //给交易量框设置默认值
	USES_CONVERSION;
	string opendirection = CT2A(m_strDirection.GetBuffer());
	if (opendirection == "多")
	{
		GetDlgItem(EDIT_DIRECTION)->SetWindowText(_T("空")); //给方向框设默认值
	}
	if (opendirection == "空")   //_T()可以实现由C++字符串到cstring的转换
	{
		GetDlgItem(EDIT_DIRECTION)->SetWindowText(_T("多")); //给方向框设默认值
	}
	GetDlgItem(EDIT_VOLUME)->SetWindowText(m_strVolume); //给交易量框设默认值
	//默认选平仓按钮
	CButton* pCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_CLOSE));
	pCloseButton->SetCheck(true);
	//填充交易所复选框
	if (((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->GetCount() != 0)
	{//重置为空，防止二次重复加载
		((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->ResetContent();
	}
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(_T("SHFE"));
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(_T("DCE"));
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(_T("CZCE"));
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(_T("CFFEX"));
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(_T("INE"));
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->SetCurSel(0);//默认选择第一项上期所
	//填充限价类型复选框
	if (((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->GetCount() != 0)
	{//重置为空，防止二次重复加载
		((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->ResetContent();
	}
	((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->AddString(_T("指定报单价"));
	((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->AddString(_T("跟盘报单价"));
	((CComboBox*)GetDlgItem(COMBO_PRICETYPE))->SetCurSel(1);//默认选择第一项指定价
	//若选定限价及跟盘价，则底层要根据开单方向传送tick 价过来

	//止盈损勾选框默认不选中
	((CButton*)GetDlgItem(CHECK_PROFITLOSS))->SetCheck(false);

	//止盈损按钮，默认选止盈
	CButton* pProfitORLossButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_PROFIT));
	pProfitORLossButton->SetCheck(true);
	pProfitORLossButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_LOSS));
	pProfitORLossButton->SetCheck(false);

	//默认价格填1
	CString cstr;
	cstr.Format(_T("%d"), 1);
	GetDlgItem(EDIT_TOUCHPRICE)->SetWindowText(cstr);

	return true;
}


// CClosePositionDlg message handlers


void CClosePositionDlg::OnBnClickedPricetype()
{
	CString priceTypeName;
	GetDlgItem(BUTTON_PRICETYPE)->GetWindowTextW(priceTypeName);
	if (priceTypeName == "限价")
	{//当该按钮按下去后，若先前为限价，则按下后改为市价
		GetDlgItem(BUTTON_PRICETYPE)->SetWindowText(_T("市价"));
	}
	else
	{
		GetDlgItem(BUTTON_PRICETYPE)->SetWindowText(_T("限价"));
	}
}


void CClosePositionDlg::OnBnClickedClose()
{
	//UpdateData(TRUE);	//将控件里的数据刷新到绑定的控件变量中,这句话不能要,否则程序会崩溃
	USES_CONVERSION;
	InputOrderParam order_param;
	//填充合约
	order_param.InstrumentID = T2A(m_strInstrument);//T2A将mfc的cstring类型转换为char*类型
	//填充交易所
	order_param.ExchangeID = T2A(m_strExchange);//T2A将mfc的cstring类型转换为char*类型
	//填充方向
	string direction_stringtype = CT2A(m_strDirection.GetBuffer()); //将cstring转换为string
	if (direction_stringtype == "空")
	{
		order_param.Direction = THOST_FTDC_D_Buy;
	}
	if (direction_stringtype == "多")
	{
		order_param.Direction = THOST_FTDC_D_Sell;
	}
	//填充平仓类型
	CButton* pCloseTodayButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_CLOSETODAY));
	if (pCloseTodayButton->GetCheck() == BST_CHECKED)
	{
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday; //平今
	}
	CButton* pCloseButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_CLOSE));
	if (pCloseButton->GetCheck() == BST_CHECKED)
	{
		order_param.CombOffsetFlag[0] = THOST_FTDC_OF_Close; //平仓
	}
	//填充手数
	GetDlgItem(EDIT_VOLUME)->GetWindowText(m_strVolume);
	order_param.VolumeTotalOriginal = _ttoi(m_strVolume);//可以将Cstring类型转换为int类型

	//填充价格类型
	order_param.OrderPriceType = THOST_FTDC_OPT_LimitPrice; //默认是限价，按钮的默认值也是限价
	order_param.ContingentCondition = THOST_FTDC_CC_Immediately; //限价单与市价单的默认触发类型都是立即触发，即这里是立即限价单及立即市价单
	CString priceTypeName;
	GetDlgItem(BUTTON_PRICETYPE)->GetWindowText(priceTypeName); //获取价格类型按钮的标签值
	if (priceTypeName == "市价")
	{
		order_param.OrderPriceType = THOST_FTDC_OPT_AnyPrice;  //为立即市价下单
	}
	//记录价格
	CString text;
	GetDlgItemText(EDIT_PRICE, text);
	string price = CT2A(text.GetBuffer());
	order_param.LimitPrice = stod(price);               //填充报单价格

	if (BST_CHECKED == IsDlgButtonChecked(CHECK_PROFITLOSS)) //下止盈损条件单是否选中
	{
		double settlement_price = _ttof(this->SettlementPrice);//实现从CString到double的转换

		CString touchPrice_cstr;
		GetDlgItem(EDIT_TOUCHPRICE)->GetWindowText(touchPrice_cstr);
		double touchPrice = _ttof(touchPrice_cstr);

		string opendirection = CT2A(m_strDirection.GetBuffer());
		CButton* pProfitORLossButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_PROFIT));
		if (pProfitORLossButton->GetCheck() == BST_CHECKED)
		{//选中止盈的处理
			if (opendirection == "多")
			{//多仓止盈单，触发价格必须大于等于现价(用本次结算价代替)
				if ( !(touchPrice >= settlement_price) ) //异常触发价报错
				{
					::MessageBox(NULL, _T("多单止盈：触发价应大于等于现价"), _T("出错"), MB_OK);//调试用
					return;//出错后直接return
				}
				else
				{//若为正确的输入则开BuyStop类型的多单止盈
					order_param.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
					order_param.StopPrice = touchPrice;
				}
			}
			else if (opendirection == "空")
			{//空仓止盈单，触发价格必须小于等于现价(用本次结算价代替)
				if (!(touchPrice <= settlement_price)) //异常触发价报错
				{
					::MessageBox(NULL, _T("空单止盈：触发价应小于等于现价"), _T("出错"), MB_OK);//调试用
					return;//出错后直接return
				}
				else
				{//若为正确的输入则开SellStop类型的空单止盈
					order_param.ContingentCondition = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
					order_param.StopPrice = touchPrice;
				}
			}
			else
			{

			}
		}
		pProfitORLossButton = reinterpret_cast<CButton*>(this->GetDlgItem(RADIO_LOSS));
		if (pProfitORLossButton->GetCheck() == BST_CHECKED)
		{//选中止损的处理
			if (opendirection == "多")
			{//多仓止损单，触发价格必须小于等于现价(用本次结算价代替)
				if (!(touchPrice <= settlement_price)) //异常触发价报错
				{
					::MessageBox(NULL, _T("多单止损：触发价应小于等于现价"), _T("出错"), MB_OK);//调试用
					return;//出错后直接return
				}
				else
				{//多单止损开SellStop单
					order_param.ContingentCondition = THOST_FTDC_CC_LastPriceLesserEqualStopPrice;
					order_param.StopPrice = touchPrice;
				}
			}
			else if (opendirection == "空")
			{//空仓止损单，触发价格必须大于等于现价(用本次结算价代替)
				if (!(touchPrice >= settlement_price)) //异常触发价报错
				{
					::MessageBox(NULL, _T("空单止损：触发价应大于等于现价"), _T("出错"), MB_OK);//调试用
					return;//出错后直接return
				}
				else
				{//空单止损开BuyStop单
					order_param.ContingentCondition = THOST_FTDC_CC_LastPriceGreaterEqualStopPrice;
					order_param.StopPrice = touchPrice;
				}
			}
			else
			{

			}
		}
		order_param.TimeCondition = THOST_FTDC_TC_GFD;//当日有效,CTP的条件单目前仅支持当日有效，此字段若不填会回报字段错误，不要忽略该字段
	}

	if (::MessageBox(NULL, _T("是否确认执行"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::MakeOrder, &order_param);
		PostMessage(WM_CLOSE, NULL, NULL);
	}
}


void CClosePositionDlg::OnEnChangeVolume()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
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
	if (strTemp == _T("0"))
	{
		editHelp->SetWindowText(_T("1"));
	}
}


void CClosePositionDlg::OnEnChangePrice()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_PRICE)));
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


void CClosePositionDlg::OnEnChangeTouchprice()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString strTemp = _T("");
	CEdit* editHelp = ((CEdit*)(GetDlgItem(EDIT_TOUCHPRICE)));
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


void CClosePositionDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	__super::OnClose();
}
