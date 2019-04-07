// QryCommissionRate.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "QryCommissionRate.h"
#include "afxdialogex.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpMsg.h"


// CQryCommissionRate dialog

IMPLEMENT_DYNAMIC(CQryCommissionRate, CDialogEx)

CQryCommissionRate::CQryCommissionRate(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQryCommissionRate::IDD, pParent)
	, m_strInstrument(_T(""))
	, m_strExchange(_T(""))
{

}

CQryCommissionRate::~CQryCommissionRate()
{
}

void CQryCommissionRate::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, COMBO_INSTRUMENT, m_strInstrument);
	DDX_CBString(pDX, COMBO_EXCHANGE, m_strExchange);
}


BEGIN_MESSAGE_MAP(CQryCommissionRate, CDialogEx)
	ON_BN_CLICKED(BUTTON_QRY, &CQryCommissionRate::OnBnClickedQryCommissionRate)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CQryCommissionRate::OnInitDialog()
{
	get_global_mgr().SetStopTimeQry();
	get_CtpMsgDistributor()->AddReceiver(this);//将CQryCommissionRate类实例添加进接收者容器中
	//借助唯一的get_global_mgr外部接口去获取ctpservice对象, 进而获取交易api对象及行情api对象, 由其来获取相应的信息
	vector<BaseData> instruments;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_basic_data(instruments);
		if (!instruments.empty())
		{//在这里, 把数据填充到组合框中去
			if (((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->GetCount() != 0)
			{//防止二次加载,为此再加载前若不为空,先清除所有先
				((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->ResetContent();
			}
			for (int i = 0; i < instruments.size(); ++i)
			{
				((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->AddString(CString(instruments[i].code));
			}
		}
	}

	vector<Exchange> exchanges;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_exchanges(exchanges);
		if (!exchanges.empty())
		{//在这里, 把数据填充到组合框中去
			if (((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->GetCount() != 0)
			{//防止二次加载,为此再加载前若不为空,先清除所有先
				((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->ResetContent();
			}
			for (int j = 0; j < exchanges.size(); ++j)
			{
				((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->AddString(CString(exchanges[j].ExchangeID.c_str()));
			}
			((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->SetCurSel(0);
		}
	}
	return TRUE;
}


// CQryCommissionRate message handlers


void CQryCommissionRate::OnBnClickedQryCommissionRate()
{
	InstrumentCommissionRateParam commissionrate_param;
	USES_CONVERSION;
	CString instrument_cstr;
	((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->GetWindowText(instrument_cstr);
	CString exchange_cstr;
	((CComboBox*)GetDlgItem(COMBO_EXCHANGE))->GetWindowText(exchange_cstr);
	strcpy_s(commissionrate_param.InstrumentID, T2A(instrument_cstr)); //T2A将mfc的cstring类型转换为char*类型
	strcpy_s(commissionrate_param.ExchangeID, T2A(exchange_cstr));//T2A将mfc的cstring类型转换为char*类型
	strcpy_s(commissionrate_param.BrokerID, ""); 
	strcpy_s(commissionrate_param.InvestorID, "");
	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::QryCommissionRate, &commissionrate_param);
		//PostMessage(WM_CLOSE, NULL, NULL);
	}
}

void CQryCommissionRate::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RspQryCommissionRate)
	{
		InstrumentCommissionRate* commission_rate = reinterpret_cast<InstrumentCommissionRate*>(lParam);
		CString OpenRatioByMoney;
		OpenRatioByMoney.Format(_T("%0.6lf"), commission_rate->OpenRatioByMoney);
		GetDlgItem(EDIT_OPENCOMMISSIONRATE)->SetWindowText(OpenRatioByMoney); 
		CString OpenRatioByVolume;
		OpenRatioByVolume.Format(_T("%0.2lf"), commission_rate->OpenRatioByVolume);
		GetDlgItem(EDIT_OPENCOMMISSION)->SetWindowText(OpenRatioByVolume); 
		CString CloseRatioByMoney;
		CloseRatioByMoney.Format(_T("%0.6lf"), commission_rate->CloseRatioByMoney);
		GetDlgItem(EDIT_CLOSECOMMISSIONRATE)->SetWindowText(CloseRatioByMoney);
		CString CloseRatioByVolume;
		CloseRatioByVolume.Format(_T("%0.2lf"), commission_rate->CloseRatioByVolume);
		GetDlgItem(EDIT_CLOSECOMMISSION)->SetWindowText(CloseRatioByVolume);
		CString CloseTodayRatioByMoney;
		CloseTodayRatioByMoney.Format(_T("%0.6lf"), commission_rate->CloseTodayRatioByMoney);
		GetDlgItem(EDIT_CLOSETODAYCOMMISSIONRATE)->SetWindowText(CloseTodayRatioByMoney);
		CString CloseTodayRatioByVolume;
		CloseTodayRatioByVolume.Format(_T("%0.2lf"), commission_rate->CloseTodayRatioByVolume);
		GetDlgItem(EDIT_CLOSETODAYCOMMISSION)->SetWindowText(CloseTodayRatioByVolume);
	}
}


void CQryCommissionRate::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);//在销毁对话框之前,应先将其从消息接收容器receivers中剔除，防止后续再将消息传递过来，但是该对话框已被销毁
	get_global_mgr().SetStartTimeQry();
	__super::OnClose();
}
