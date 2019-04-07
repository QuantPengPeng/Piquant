// GridCtrlOrderList.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "GridCtrlOrderList.h"


// CGridCtrlOrderList

IMPLEMENT_DYNAMIC(CGridCtrlOrderList, CCustomGridCtrl)

CGridCtrlOrderList::CGridCtrlOrderList()
{

}

CGridCtrlOrderList::~CGridCtrlOrderList()
{
}


BEGIN_MESSAGE_MAP(CGridCtrlOrderList, CCustomGridCtrl)
END_MESSAGE_MAP()

void CGridCtrlOrderList::OnFixedColumnClick(CCellID& cell)
{
	//::MessageBox(NULL, _T("返回"), _T("返回"), MB_OK);//调试用
	//具体情况具体分析，以调用合适的弹出面板
	//CLoginDlg login_dlg;
	//login_dlg.DoModal();//此处用于显示登录菜单选项的界面
}



// CGridCtrlOrderList message handlers


