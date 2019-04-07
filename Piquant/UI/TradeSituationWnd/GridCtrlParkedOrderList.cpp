// GridCtrlParkedOrderList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlParkedOrderList.h"
#include "../MakeOrder/RemoveParkedOrderDlg.h"


// CGridCtrlParkedOrderList

IMPLEMENT_DYNAMIC(CGridCtrlParkedOrderList, CCustomGridCtrl)

CGridCtrlParkedOrderList::CGridCtrlParkedOrderList()
{

}

CGridCtrlParkedOrderList::~CGridCtrlParkedOrderList()
{
}


void CGridCtrlParkedOrderList::OnFixedColumnClick(CCellID& cell)
{
	int row = cell.row;
	CString InstrumentID = this->GetItemText(row, 0);//获取合约ID，确保第0列一定是合约ID
	CString ParkOrderID = this->GetItemText(row, 11);//获取预埋单ID，确保第11列一定是预埋单ID
	if (this->GetItemText(row, 9) == _T("未发送"))
	{//只有未发送状态下才可以预埋撤单
		CRemoveParkedOrderDlg removeparkedorder_dlg(InstrumentID, ParkOrderID);
		removeparkedorder_dlg.DoModal();
	}
	else
	{
		//不处理
	}
	
}

BEGIN_MESSAGE_MAP(CGridCtrlParkedOrderList, CCustomGridCtrl)
END_MESSAGE_MAP()



// CGridCtrlParkedOrderList message handlers


