// MakeOrderBoardPane.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "MakeOrderBoardPane.h"


// CMakeOrderBoardPane

IMPLEMENT_DYNAMIC(CMakeOrderBoardPane, CDockablePane)

CMakeOrderBoardPane::CMakeOrderBoardPane()
:m_wndSMOBoard(NULL)
{//记得在构造函数中给指针初始化为null

}

CMakeOrderBoardPane::~CMakeOrderBoardPane()
{
}

//在消息map中添加消息绑定
BEGIN_MESSAGE_MAP(CMakeOrderBoardPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMakeOrderBoardPane message handlers

int CMakeOrderBoardPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	m_wndSMOBoard = new CStandardMakeOrderBoard(this);  //借助CStandardMakeOrderBoard类的构造函数来创建下单板
	return 0;
}


void CMakeOrderBoardPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}
	CRect rectClient;
	GetClientRect(rectClient); //此处用于获取窗口客户区的大小，并将相应的大小参数存入rectClient这个结构体中去
	m_wndSMOBoard->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

