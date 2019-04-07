#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlCloseTradeList

class CGridCtrlCloseTradeList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlCloseTradeList)

public:
	CGridCtrlCloseTradeList();
	virtual ~CGridCtrlCloseTradeList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


