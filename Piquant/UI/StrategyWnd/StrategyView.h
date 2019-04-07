#pragma once
#include "StrategyStatusView.h"
#include "StrategyMsgView.h"

// CStrategyView

class CStrategyView : public CDockablePane
{
	DECLARE_DYNAMIC(CStrategyView)

public:
	CStrategyView();
	virtual ~CStrategyView();

protected:
	CFrameWnd*      m_pMyFrame;    //主窗口
	CSplitterWnd      m_cSplitter;       // 左右分隔的子窗口

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


