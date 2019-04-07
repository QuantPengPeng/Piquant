#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlUnfilledOrderList

class CGridCtrlUnfilledOrderList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlUnfilledOrderList)

public:
	CGridCtrlUnfilledOrderList();
	virtual ~CGridCtrlUnfilledOrderList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


