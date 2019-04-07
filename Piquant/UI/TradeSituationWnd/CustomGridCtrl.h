#pragma once
#include "../GridCtrl/GridCtrl.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../Tool/ConfigReader.h"
#include <vector>

// CCustomGridCtrl

class CCustomGridCtrl : public CGridCtrl
{
	DECLARE_DYNAMIC(CCustomGridCtrl)

public:
	CCustomGridCtrl();
	virtual ~CCustomGridCtrl();
	//初始化列头;
	void LoadColumns(const vector<ColumnInfo>& header_colums);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

