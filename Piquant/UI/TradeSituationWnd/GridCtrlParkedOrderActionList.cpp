// GridCtrlParkedOrderActionList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlParkedOrderActionList.h"
#include "../MakeOrder/RemoveParkedOrderActionDlg.h"


// CGridCtrlParkedOrderActionList

IMPLEMENT_DYNAMIC(CGridCtrlParkedOrderActionList, CCustomGridCtrl)

CGridCtrlParkedOrderActionList::CGridCtrlParkedOrderActionList()
{

}

CGridCtrlParkedOrderActionList::~CGridCtrlParkedOrderActionList()
{
}


void CGridCtrlParkedOrderActionList::OnFixedColumnClick(CCellID& cell)
{
	int row = cell.row;
	CString InstrumentID = this->GetItemText(row, 0);//获取合约ID，确保第0列一定是合约ID
	CString ParkOrderActionID = this->GetItemText(row, 7);//获取预埋单ID，确保第7列一定是预埋撤单ID
	if (this->GetItemText(row, 4) == _T("未发送"))
	{//只有未发送状态下才可以预埋撤单
		CRemoveParkedOrderActionDlg removeparkedorderaction_dlg(InstrumentID, ParkOrderActionID);
		removeparkedorderaction_dlg.DoModal();
	}
	else
	{
		//不处理
	}
}

BEGIN_MESSAGE_MAP(CGridCtrlParkedOrderActionList, CCustomGridCtrl)
END_MESSAGE_MAP()



// CGridCtrlParkedOrderActionList message handlers


