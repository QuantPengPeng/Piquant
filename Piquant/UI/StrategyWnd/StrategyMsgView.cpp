// StrategyMsgView.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "StrategyMsgView.h"


// CStrategyMsgView

IMPLEMENT_DYNCREATE(CStrategyMsgView, CFormView)

CStrategyMsgView::CStrategyMsgView()
	: CFormView(CStrategyMsgView::IDD)
{

}

CStrategyMsgView::~CStrategyMsgView()
{
}

void CStrategyMsgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStrategyMsgView, CFormView)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CStrategyMsgView diagnostics

#ifdef _DEBUG
void CStrategyMsgView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CStrategyMsgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

void CStrategyMsgView::OnInitialUpdate()
{
	get_strategymsg_distributor()->AddReceiver(this); //添加为策略消息分发器的观察者

	((CListCtrl*)GetDlgItem(LIST_STRATEGYMSG))->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	((CListCtrl*)GetDlgItem(LIST_STRATEGYMSG))->InsertColumn(0, _T("策略名"), NULL, 60, -1);
	((CListCtrl*)GetDlgItem(LIST_STRATEGYMSG))->InsertColumn(1, _T("日志时间"), LVCFMT_CENTER, 60, -1);
	((CListCtrl*)GetDlgItem(LIST_STRATEGYMSG))->InsertColumn(2, _T("日志级别"), NULL, 60, -1);
	((CListCtrl*)GetDlgItem(LIST_STRATEGYMSG))->InsertColumn(3, _T("日志内容"), NULL, 200, -1);
}

void CStrategyMsgView::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_UPDATESTRATEGYMSG)
	{
		StrategyMsg* pStrategyMsg = reinterpret_cast<StrategyMsg*>(lParam);
		StrategyMsg msg = *pStrategyMsg;
		CString strategyName((msg.strategyName).c_str());
		if (strategyName == _T(""))
		{//处理异常
			return;
		}
		//插入一行，行头为策略名
		int rowcount = ((CListCtrl*)(this->GetDlgItem(LIST_STRATEGYMSG)))->GetItemCount(); //获取行数
		((CListCtrl*)(this->GetDlgItem(LIST_STRATEGYMSG)))->InsertItem(rowcount, strategyName); //插入一行
		//填充消息的本地时间
		time_t nowtime; 
		nowtime = time(NULL); //获取本地时间
		struct tm* local;
		local = localtime(&nowtime);
		char loggingTime[100];
		sprintf_s(loggingTime, "%s", asctime(local)); //输出带月日时分秒毫秒
		msg.loggingTime = loggingTime;
		//以下为各列刷新操作
		if (msg.loggingTime != "")
		{
			CString loggingTime(msg.loggingTime.c_str());
			((CListCtrl*)(this->GetDlgItem(LIST_STRATEGYMSG)))->SetItemText(rowcount, 1, loggingTime);
		}
		if (msg.loggingLevel != 0)
		{
			CString loggingLevel;
			if (msg.loggingLevel == UEGENT)
			{
				loggingLevel = _T("紧急");
			}
			else if (msg.loggingLevel==IMPORTANT)
			{
				loggingLevel = _T("重要");
			}
			else
			{
				loggingLevel = _T("一般");
			}
			((CListCtrl*)(this->GetDlgItem(LIST_STRATEGYMSG)))->SetItemText(rowcount, 2, loggingLevel);
		}
		if (msg.loggingContent != "")
		{
			CString loggingContent(msg.loggingContent.c_str());
			((CListCtrl*)(this->GetDlgItem(LIST_STRATEGYMSG)))->SetItemText(rowcount, 3, loggingContent);
		}
	}
}

#endif
#endif //_DEBUG


// CStrategyMsgView message handlers


void CStrategyMsgView::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_strategymsg_distributor()->DelReceiver(this); //从观察者列表中剔除
	__super::OnClose();
}
