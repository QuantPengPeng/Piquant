
// ShowInstrumentsView.h : interface of the CShowInstrumentsView class
//

#pragma once
#include "../resource.h"
#include "../../Tool/ConfigReader.h"
#include "../GridCtrl/GridCtrl.h"
#include "../Doc.h"
class CGridCtrl;//前向声明

#define WM_UPDATE_GRIDCTRL WM_USER+3000 //自定义的消息类型

class CShowInstrumentsView : public CView, public CtpMsgReceiver
{
protected: // create from serialization only
	CShowInstrumentsView();
	DECLARE_DYNCREATE(CShowInstrumentsView)

// Attributes
public:
	CDoc* GetDocument() const;
	CGridCtrl* m_pGridCtrl;//指向表格的

// Operations
public:
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam);//自定义的消息映射函数
	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CShowInstrumentsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();// 构造后第一次调用

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in ShowInstrumentsView.cpp
inline CShowInstrumentsDoc* CShowInstrumentsView::GetDocument() const
   { return reinterpret_cast<CShowInstrumentsDoc*>(m_pDocument); }
#endif

