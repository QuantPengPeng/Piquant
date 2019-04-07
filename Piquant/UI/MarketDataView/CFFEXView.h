#pragma once
#include "../resource.h"
#include "../../Tool/ConfigReader.h"
#include "../LocalMessageEvent/LocalMessage.h"
#include "../LocalMessageEvent/LocalMessageEventHandle.h"
#include "../MarketDataView/MarketDataGridCtrl.h"

class CGridCtrl;//前向声明

class CCFFEXView : public CFormView, public CtpMsgReceiver
{
protected: // 仅从序列化创建
	CCFFEXView();
	DECLARE_DYNCREATE(CCFFEXView)

public:
	enum{ IDD = IDD_MULTITRADER_FORM };

// 特性
public:
	CDoc* GetDocument() const;
	CMarketDataGridCtrl* m_pGridCtrl;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CCFFEXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT  OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam);//自定义的消息映射函数
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void SetGridCtrlColumns(const vector<ColumnInfo>& header_columns);
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);//CMultiTraderView重载了继承自IServiceReceiver类的方法
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnClose();
};

#ifndef _DEBUG  // MultiTraderView.cpp 中的调试版本
inline CDoc* CCFFEXView::GetDocument() const
   { return reinterpret_cast<CDoc*>(m_pDocument); }
#endif

//派生类
class CCFFEX_ChildView : public CCFFEXView
{
public:
	CCFFEX_ChildView() :CCFFEXView(){};
	~CCFFEX_ChildView();
	void UpdateMarketDataView(LPARAM lParam);
};