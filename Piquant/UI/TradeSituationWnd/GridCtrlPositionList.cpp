// GridCtrlPositionList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlPositionList.h"
#include "../MakeOrder/ClosePositionDlg.h"


// CGridCtrlPositionList

IMPLEMENT_DYNAMIC(CGridCtrlPositionList, CCustomGridCtrl)

CGridCtrlPositionList::CGridCtrlPositionList()
{

}

CGridCtrlPositionList::~CGridCtrlPositionList()
{
}


BEGIN_MESSAGE_MAP(CGridCtrlPositionList, CCustomGridCtrl)
END_MESSAGE_MAP()

void CGridCtrlPositionList::OnFixedColumnClick(CCellID& cell)
{
	int row = cell.row;
	CString InstrumentID = this->GetItemText(row,  0);//获取合约ID，确保第一列一定是合约ID
	CString Direction = this->GetItemText(row, 2);//获取方向，确保第2列一定是方向
	CString Position = this->GetItemText(row, 6);//获取总持仓，确保第6列一定是总持仓
	CString PrePosition = this->GetItemText(row, 7);//获取昨持仓，确保第7列一定是昨持仓
	CString TodayPosition = this->GetItemText(row, 8);//获取今持仓，确保第8列一定是今持仓
	CString SettlementPrice = this->GetItemText(row, 10);//获取结算价，确保第10列一定是结算价
	CClosePositionDlg PositionDlg(InstrumentID, Direction, Position, PrePosition, TodayPosition, SettlementPrice);
	PositionDlg.DoModal();
}



// CGridCtrlPositionList message handlers


