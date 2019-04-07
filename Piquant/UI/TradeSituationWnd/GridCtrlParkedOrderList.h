#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlParkedOrderList

class CGridCtrlParkedOrderList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlParkedOrderList)

public:
	CGridCtrlParkedOrderList();
	virtual ~CGridCtrlParkedOrderList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


