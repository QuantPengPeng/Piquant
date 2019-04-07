#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlParkedOrderActionList

class CGridCtrlParkedOrderActionList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlParkedOrderActionList)

public:
	CGridCtrlParkedOrderActionList();
	virtual ~CGridCtrlParkedOrderActionList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


