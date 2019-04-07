// RemoveParkedOrderActionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "RemoveParkedOrderActionDlg.h"
#include "afxdialogex.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpMsg.h"

// CRemoveParkedOrderActionDlg dialog

IMPLEMENT_DYNAMIC(CRemoveParkedOrderActionDlg, CDialogEx)

CRemoveParkedOrderActionDlg::CRemoveParkedOrderActionDlg(CString InstrumentID, CString ParkOrderActionID, CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoveParkedOrderActionDlg::IDD, pParent)
{
	this->InstrumentID = InstrumentID;
	this->ParkOrderActionID = ParkOrderActionID;
}

CRemoveParkedOrderActionDlg::~CRemoveParkedOrderActionDlg()
{
}

BOOL CRemoveParkedOrderActionDlg::OnInitDialog()
{
	CString dispText = this->InstrumentID + _T("(") + this->ParkOrderActionID + _T(")") + _T(":  ") + _T("预埋撤单撤单");
	GetDlgItem(STATIC_INSTRUMENT)->SetWindowText(dispText);
	return true;
}

void CRemoveParkedOrderActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoveParkedOrderActionDlg, CDialogEx)
	ON_BN_CLICKED(MFCBUTTON_REMOVEPARKEDORDER, &CRemoveParkedOrderActionDlg::OnBnClickedRemoveparkedorder)
END_MESSAGE_MAP()


// CRemoveParkedOrderActionDlg message handlers


void CRemoveParkedOrderActionDlg::OnBnClickedRemoveparkedorder()
{
	// TODO: Add your control notification handler code here
	USES_CONVERSION;
	RemoveParkedOrderActionParam order_param;
	//填充报单编号
	order_param.ParkedOrderActionID = T2A(this->ParkOrderActionID);//T2A将mfc的cstring类型转换为char*类型
	if (::MessageBox(NULL, _T("是否确认预埋撤单撤单"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->request(ctp_msg::RemoveParkedOrderAction, &order_param);
		PostMessage(WM_CLOSE, NULL, NULL);
	}
}
