// GridCtrlConditionOrderList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlConditionOrderList.h"
#include "../MakeOrder/CancelOrderDlg.h"

// CGridCtrlConditionOrderList

IMPLEMENT_DYNAMIC(CGridCtrlConditionOrderList, CCustomGridCtrl)

CGridCtrlConditionOrderList::CGridCtrlConditionOrderList()
{

}

CGridCtrlConditionOrderList::~CGridCtrlConditionOrderList()
{
}


void CGridCtrlConditionOrderList::OnFixedColumnClick(CCellID& cell)
{
	int row = cell.row;
	CString OrderSysID = this->GetItemText(row, 16);//获取报单编号，确保第16列一定是报单编号
	CString InstrumentID = this->GetItemText(row, 0);//获取合约ID，确保第0列一定是合约ID
	CString Direction = this->GetItemText(row, 2);//获取方向，确保第2列一定是方向
	CString OpenClose = this->GetItemText(row, 3);//获取开平，确保第3列一定是开平类型
	CString Price = this->GetItemText(row, 5);//获取价格，确保第5列一定是报单价格
	CString insertVolume = this->GetItemText(row, 6);//获取报单量，确保第6列一定是报单量
	CString ExchangeID = this->GetItemText(row, 15);//获取交易所ID，确保第15列一定是交易所ID
	if (this->GetItemText(row, 9) == _T("尚未触发"))
	{
		CCancelOrderDlg cancelorder_dlg(OrderSysID, InstrumentID, Direction, OpenClose, Price, insertVolume, ExchangeID);
		cancelorder_dlg.DoModal();
	}
	else
	{
		//不处理
	}
}

BEGIN_MESSAGE_MAP(CGridCtrlConditionOrderList, CCustomGridCtrl)
END_MESSAGE_MAP()



// CGridCtrlConditionOrderList message handlers


