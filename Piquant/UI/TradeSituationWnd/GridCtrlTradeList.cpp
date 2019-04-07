// GridCtrlTradeList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlTradeList.h"


// CGridCtrlTradeList

IMPLEMENT_DYNAMIC(CGridCtrlTradeList, CCustomGridCtrl)

CGridCtrlTradeList::CGridCtrlTradeList()
{

}

CGridCtrlTradeList::~CGridCtrlTradeList()
{
}


BEGIN_MESSAGE_MAP(CGridCtrlTradeList, CCustomGridCtrl)
END_MESSAGE_MAP()

void CGridCtrlTradeList::OnFixedColumnClick(CCellID& cell)
{
	//::MessageBox(NULL, _T("返回"), _T("返回"), MB_OK);//调试用
}



// CGridCtrlTradeList message handlers


