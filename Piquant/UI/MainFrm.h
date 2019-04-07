
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "StrategyWnd/StrategyTree.h"
#include "TradeSituationWnd/TradeSituationWnd.h"
#include "MakeOrder/MakeOrderBoardPane.h"
#include "AccountStatusBar/AccountStatusBar.h"
#include"../ServiceLayer/CtpService.h"
#include "../ServiceLayer/DataTypes.h"
#include "../ServiceLayer/CtpMsgDistributor.h"
#include "GlobalMgr.h"
#include "LocalMessageEvent/LocalMessage.h"
#include "LocalMessageEvent/LocalMessageEventHandle.h"
#include "StrategyWnd/StrategyView.h"
#define WM_UPDATE_STATUSBAR WM_USER+1003 //自定义的消息类型

class CMainFrame : public CMDIFrameWndEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBarImages m_UserImages;
	CAccountStatusBar     m_wndAccountStatusBar;
	CFileView         m_wndFileView;
	CStrategyView  m_wndStrategyView;
	COutputWnd        m_wndOutput;
	//添加个下单板
	CMakeOrderBoardPane m_wndSMOBoardPane;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnSystemLogin();
	afx_msg void OnQrycommissionrate();
	afx_msg void OnQryinstrumentmarginrate();
	afx_msg void OnQryexchangerate();
	afx_msg void OnQryordercommrate();
	afx_msg void OnLinkexchange();
	afx_msg void OnQrymaincontract();
	afx_msg LRESULT  OnUpdateStatusBar(WPARAM wParam, LPARAM lParam);//自定义的消息映射函数

public:
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

public:
	HANDLE  hThread_LocalMessageThread;   //本地消息线程句柄
	DWORD  ThreadID_LocalMessageThread;  //本地消息线程ID
	HANDLE  hThread;   //定时查询线程句柄
	DWORD  ThreadID;  //定时查询线程ID
	afx_msg void OnSystemLoginout();
};

//全局函数
void CALLBACK  Timerproc_LocalMessageThread(); //本地消息线程函数原型声明
void CALLBACK  Timerproc_ReqQry();					   //定时查询线程函数原型声明