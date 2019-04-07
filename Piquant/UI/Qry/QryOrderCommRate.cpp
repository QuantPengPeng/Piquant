// QryOrderCommRate.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "QryOrderCommRate.h"
#include "afxdialogex.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpMsg.h"

// CQryOrderCommRate dialog

IMPLEMENT_DYNAMIC(CQryOrderCommRate, CDialogEx)

CQryOrderCommRate::CQryOrderCommRate(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQryOrderCommRate::IDD, pParent)
{

}

CQryOrderCommRate::~CQryOrderCommRate()
{
}

void CQryOrderCommRate::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQryOrderCommRate, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(MFCBUTTON_QRYCOMMRATE, &CQryOrderCommRate::OnBnClickedQrycommrate)
END_MESSAGE_MAP()

BOOL CQryOrderCommRate::OnInitDialog()
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
	return TRUE;
}

void CQryOrderCommRate::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RspQryInstrumentOrderCommRate)
	{
		OrderCommRate* ordercomm_rate = reinterpret_cast<OrderCommRate*>(lParam);
		CString OrderCommByVolume;
		OrderCommByVolume.Format(_T("%0.3lf"), ordercomm_rate->OrderCommByVolume);
		GetDlgItem(EDIT_OrderCommByVolume)->SetWindowText(OrderCommByVolume);
		CString OrderActionCommByVolume;
		OrderActionCommByVolume.Format(_T("%0.3lf"), ordercomm_rate->OrderActionCommByVolume);
		GetDlgItem(EDIT_OrderActionCommByVolume)->SetWindowText(OrderActionCommByVolume);
	}
}


// CQryOrderCommRate message handlers


void CQryOrderCommRate::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	get_global_mgr().SetStartTimeQry();
	__super::OnClose();
}


void CQryOrderCommRate::OnBnClickedQrycommrate()
{
	QryInstrumentOrderCommRateParam ordercommrate_param;
	USES_CONVERSION;
	CString instrument_cstr;
	((CComboBox*)GetDlgItem(COMBO_INSTRUMENT))->GetWindowText(instrument_cstr);
	ordercommrate_param.InstrumentID = T2A(instrument_cstr); //T2A将mfc的cstring类型转换为char*类型
	ordercommrate_param.BrokerID = "";
	ordercommrate_param.InvestorID = "";
	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::QryInstrumentOrderCommRate, &ordercommrate_param);
		//PostMessage(WM_CLOSE, NULL, NULL);
	}
}
