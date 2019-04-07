// QryExchangeRateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "QryExchangeRateDlg.h"
#include "afxdialogex.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpMsg.h"


// CQryExchangeRateDlg dialog

IMPLEMENT_DYNAMIC(CQryExchangeRateDlg, CDialogEx)

CQryExchangeRateDlg::CQryExchangeRateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQryExchangeRateDlg::IDD, pParent)
{
	get_global_mgr().SetStopTimeQry();
	get_CtpMsgDistributor()->AddReceiver(this);
}

CQryExchangeRateDlg::~CQryExchangeRateDlg()
{
}

void CQryExchangeRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQryExchangeRateDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(BUTTON_QRYEXCHANGERATE, &CQryExchangeRateDlg::OnBnClickedQryexchangerate)
END_MESSAGE_MAP()

void CQryExchangeRateDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RspQryExchangeRate)
	{
		_ExchangeRate* commission_rate = reinterpret_cast<_ExchangeRate*>(lParam);
		CString FromCurrencyUnit;
		FromCurrencyUnit.Format(_T("%0.2lf"), commission_rate->FromCurrencyUnit);
		GetDlgItem(EDIT_FromCurrencyUnit)->SetWindowText(FromCurrencyUnit);
		CString ExchangeRate;
		ExchangeRate.Format(_T("%0.6lf"), commission_rate->ExchangeRate);
		GetDlgItem(EDIT_ExchangeRate)->SetWindowText(ExchangeRate);
	}
}


// CQryExchangeRateDlg message handlers


void CQryExchangeRateDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	get_global_mgr().SetStartTimeQry();
	__super::OnClose();
}


void CQryExchangeRateDlg::OnBnClickedQryexchangerate()
{
	QryExchangeRateParam exchangerate_param;
	USES_CONVERSION;
	CString FromCurrencyID;
	((CComboBox*)GetDlgItem(COMBO_FROMCURRENCYID))->GetWindowText(FromCurrencyID);
	CString ToCurrencyID;
	((CComboBox*)GetDlgItem(COMBO_TOCURRENCYID))->GetWindowText(ToCurrencyID);
	exchangerate_param.FromCurrencyID=T2A(FromCurrencyID); //T2A将mfc的cstring类型转换为char*类型
	exchangerate_param.ToCurrencyID=T2A(ToCurrencyID);//T2A将mfc的cstring类型转换为char*类型
	exchangerate_param.BrokerID="";
	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::QryExchangeRate, &exchangerate_param);
		//PostMessage(WM_CLOSE, NULL, NULL);
	}
}
