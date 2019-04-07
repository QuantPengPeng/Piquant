
#pragma once
#include "CustomGridCtrl.h"
#include "../../ServiceLayer/CtpService.h"
#include "GridCtrlOrderList.h"
#include "GridCtrlPositionList.h"
#include "GridCtrlTradeList.h"
#include "GridCtrlUnfilledOrderList.h"
#include "GridCtrlPositionDetailList.h"
#include "GridCtrlCloseTradeList.h"
#include "GridCtrlParkedOrderList.h"
#include "GridCtrlParkedOrderActionList.h"
#include "GridCtrlConditionOrderList.h"
#include "../LocalMessageEvent/LocalMessage.h"
//#define WM_UPDATE_GRIDCTRL WM_USER+10 //已弃用
/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口

class CTradeSituationWnd : public CListBox
{
// 构造
public:
	CTradeSituationWnd();

// 实现
public:
	virtual ~CTradeSituationWnd();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()

};

class COutputWnd : public CDockablePane, public CtpMsgReceiver
{
// 构造
public:
	COutputWnd();

	void UpdateFonts();

// 特性
protected:
	CMFCTabCtrl	m_wndTabs;

	//委托列表;
	CGridCtrlOrderList m_wndOrderList;
	//未成委托列表
	CGridCtrlUnfilledOrderList m_wndUnfilledOrderList;
	//持仓;
	CGridCtrlPositionList m_wndPosition; 
	//持仓明细列表
	CGridCtrlPositionDetailList m_wndPositionDetail;
	//成交;
	CGridCtrlTradeList m_wndTrade;
	//平仓列表
	CGridCtrlCloseTradeList m_wndCloseTrade;
	//预埋单列表
	CGridCtrlParkedOrderList m_wndParkedOrderList;
	//预埋撤单列表
	CGridCtrlParkedOrderActionList m_wndParkedOrderActionList;
	//条件单
	CGridCtrlConditionOrderList m_wndConditionOrderList;

protected:
	void FillBuildWindow();
	void FillDebugWindow();
	void FillFindWindow();

	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();
	afx_msg LRESULT OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam);
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	

	DECLARE_MESSAGE_MAP()

	

};

