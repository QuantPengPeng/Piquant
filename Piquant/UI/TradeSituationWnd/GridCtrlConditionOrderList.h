#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlConditionOrderList

class CGridCtrlConditionOrderList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlConditionOrderList)

public:
	CGridCtrlConditionOrderList();
	virtual ~CGridCtrlConditionOrderList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


