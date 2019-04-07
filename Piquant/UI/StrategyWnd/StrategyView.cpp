// StrategyView.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "StrategyView.h"


// CStrategyView

IMPLEMENT_DYNAMIC(CStrategyView, CDockablePane)

CStrategyView::CStrategyView()
{

}

CStrategyView::~CStrategyView()
{
}


BEGIN_MESSAGE_MAP(CStrategyView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CStrategyView message handlers




int CStrategyView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CString strMyClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH) ::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	// Create the frame window with "this" as the parent
	m_pMyFrame = new CFrameWnd;
	m_pMyFrame->Create(strMyClass, _T(""), WS_CHILD, CRect(0, 0, 200, 200), this);
	m_pMyFrame->ShowWindow(SW_SHOW);

	// and finally, create the splitter with the frame as the parent
	m_cSplitter.CreateStatic(m_pMyFrame, 2, 1); //在Frame里切分视图窗口为1×2，就是两行一列
	m_cSplitter.CreateView(0, 0, RUNTIME_CLASS(CStrategyStatusView), CSize(100, 320), NULL);//第一行一列
	m_cSplitter.CreateView(1, 0, RUNTIME_CLASS(CStrategyMsgView), CSize(100, 500), NULL);//第二行一列
	
	return 0;
}


void CStrategyView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect cRect;
	GetWindowRect(&cRect);
	ScreenToClient(&cRect);
	m_pMyFrame->MoveWindow(&cRect);
	m_pMyFrame->ShowWindow(SW_SHOW);
}
