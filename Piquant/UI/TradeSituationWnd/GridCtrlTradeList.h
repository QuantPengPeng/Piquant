#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlTradeList

class CGridCtrlTradeList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlTradeList)

public:
	CGridCtrlTradeList();
	virtual ~CGridCtrlTradeList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


