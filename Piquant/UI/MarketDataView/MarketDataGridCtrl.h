#pragma once
#include "../GridCtrl/GridCtrl.h"

// CMarketDataGridCtrl

class CMarketDataGridCtrl : public CGridCtrl
{
	DECLARE_DYNAMIC(CMarketDataGridCtrl)

public:
	CMarketDataGridCtrl();
	virtual ~CMarketDataGridCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnFixedColumnClick(CCellID& cell);

};


