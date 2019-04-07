#pragma once
#include "CustomGridCtrl.h"

// CGridCtrlPositionList

class CGridCtrlPositionList : public CCustomGridCtrl
{
	DECLARE_DYNAMIC(CGridCtrlPositionList)

public:
	CGridCtrlPositionList();
	virtual ~CGridCtrlPositionList();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


