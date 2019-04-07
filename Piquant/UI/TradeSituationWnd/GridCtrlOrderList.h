#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlOrderList

class CGridCtrlOrderList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlOrderList)

public:
	CGridCtrlOrderList();
	virtual ~CGridCtrlOrderList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


