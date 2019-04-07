// QryInstrumentMarginRateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "QryInstrumentMarginRateDlg.h"
#include "afxdialogex.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../../sdk/ThostFtdcUserApiDataType.h"
#include "../../ServiceLayer/CtpMsg.h"



// CQryInstrumentMarginRateDlg dialog

IMPLEMENT_DYNAMIC(CQryInstrumentMarginRateDlg, CDialogEx)

CQryInstrumentMarginRateDlg::CQryInstrumentMarginRateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQryInstrumentMarginRateDlg::IDD, pParent)
{

}

CQryInstrumentMarginRateDlg::~CQryInstrumentMarginRateDlg()
{
}

void CQryInstrumentMarginRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQryInstrumentMarginRateDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(BUTTON_QRYMARGINRATE, &CQryInstrumentMarginRateDlg::OnBnClickedQrymarginrate)
END_MESSAGE_MAP()

BOOL CQryInstrumentMarginRateDlg::OnInitDialog()
{
	get_global_mgr().SetStopTimeQry();
	get_CtpMsgDistributor()->AddReceiver(this);//将CQryInstrumentMarginRateDlg类实例添加进接收者容器中
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

	if (((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->GetCount() != 0)
	{//防止二次加载,为此再加载前若不为空,先清除所有先
		((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->ResetContent();
	}
	((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->AddString(_T("投机"));
	((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->AddString(_T("套利"));
	((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->AddString(_T("套保"));
	((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->AddString(_T("做市商"));
	return true;
}


// CQryInstrumentMarginRateDlg message handlers


void CQryInstrumentMarginRateDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);//在销毁对话框之前,应先将其从消息接收容器receivers中剔除，防止后续再将消息传递过来，但是该对话框已被销毁
	get_global_mgr().SetStartTimeQry();
	CDialogEx::OnClose();
}

void CQryInstrumentMarginRateDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RspQryMarginRate)
	{
		MarginRate* margin_rate = reinterpret_cast<MarginRate*>(lParam);
		CString LongMarginRatioByMoney;
		LongMarginRatioByMoney.Format(_T("%0.6lf"), margin_rate->LongMarginRatioByMoney);
		GetDlgItem(EDIT_LongMarginRatioByMoney)->SetWindowText(LongMarginRatioByMoney);
		CString LongMarginRatioByVolume;
		LongMarginRatioByVolume.Format(_T("%0.2lf"), margin_rate->LongMarginRatioByVolume);
		GetDlgItem(EDIT_LongMarginRatioByVolume)->SetWindowText(LongMarginRatioByVolume);
		CString ShortMarginRatioByMoney;
		ShortMarginRatioByMoney.Format(_T("%0.6lf"), margin_rate->ShortMarginRatioByMoney);
		GetDlgItem(EDIT_ShortMarginRatioByMoney)->SetWindowText(ShortMarginRatioByMoney);
		CString ShortMarginRatioByVolume;
		ShortMarginRatioByVolume.Format(_T("%0.2lf"), margin_rate->ShortMarginRatioByVolume);
		GetDlgItem(EDIT_ShortMarginRatioByVolume)->SetWindowText(ShortMarginRatioByVolume);
	}
}


void CQryInstrumentMarginRateDlg::OnBnClickedQrymarginrate()
{
	QryInstrumentMarginRateParam marginrate_param;
	USES_CONVERSION;
	CString instrument_cstr;
	((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->GetWindowText(instrument_cstr);
	CString hedgeflag_cstr;
	((CComboBox*)GetDlgItem(COMBO_HEDGEFLAG))->GetWindowText(hedgeflag_cstr);
	marginrate_param.InstrumentID = T2A(instrument_cstr); //T2A将mfc的cstring类型转换为char*类型
	if (hedgeflag_cstr == _T("投机"))
	{
		marginrate_param.HedgeFlag = THOST_FTDC_HF_Speculation;
	}
	else if (hedgeflag_cstr == _T("套利"))
	{
		marginrate_param.HedgeFlag = THOST_FTDC_HF_Arbitrage;
	}
	else if (hedgeflag_cstr == _T("套保"))
	{
		marginrate_param.HedgeFlag = THOST_FTDC_HF_Hedge;
	}
	else
	{
		marginrate_param.HedgeFlag = THOST_FTDC_HF_MarketMaker;
	}
	marginrate_param.BrokerID="";
	marginrate_param.InvestorID="";
	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::QryMarginRate, &marginrate_param);
		//PostMessage(WM_CLOSE, NULL, NULL);
	}
}
