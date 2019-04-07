#pragma once
#include "../resource.h"
#include "../../Tool/ConfigReader.h"
#include "../GridCtrl/GridCtrl.h"
#include "../Doc.h"
#include "../LocalMessageEvent/LocalMessage.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include <vector>
#include <set>
#include "MarketDataGridCtrl.h"
class CGridCtrl;//前向声明

// CCZCEView view

class CCZCEView : public CView, public CtpMsgReceiver
{
	DECLARE_DYNCREATE(CCZCEView)

protected:
	CCZCEView();           // protected constructor used by dynamic creation
	virtual ~CCZCEView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	CMarketDataGridCtrl* m_pGridCtrl;//指向表格的
public:
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);
	virtual void OnInitialUpdate();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT  OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam);//自定义的消息映射函数
};

//派生类
class CCZCE_ChildView : public CCZCEView
{
public:
	CCZCE_ChildView() :CCZCEView(){};
	~CCZCE_ChildView();
	void UpdateMarketDataView(LPARAM lParam);
};