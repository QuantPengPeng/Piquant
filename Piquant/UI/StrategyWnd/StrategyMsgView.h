#pragma once
#include "../GlobalMgr.h"
#include "../LocalMessageEvent/LocalMessage.h"
#include "../../Interface/IStrategy.h"
#include "../../Interface/StrategyMessage.h"


// CStrategyMsgView form view

class CStrategyMsgView : public CFormView, public StrategyMsgDistributor
{
	DECLARE_DYNCREATE(CStrategyMsgView)

protected:
	CStrategyMsgView();           // protected constructor used by dynamic creation
	virtual ~CStrategyMsgView();

public:
	enum { IDD = FORMVIEW_STRATEGYMSG };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	virtual void OnInitialUpdate();
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};


