#pragma once
#include "StandardMakeOrderBoard.h"　//下单板所需用的类，包含该头文件

// CMakeOrderBoardPane

class CMakeOrderBoardPane : public CDockablePane
{
	DECLARE_DYNAMIC(CMakeOrderBoardPane)

public:
	CMakeOrderBoardPane();
	virtual ~CMakeOrderBoardPane();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CStandardMakeOrderBoard* m_wndSMOBoard; //面板本身是没有资源的，要在其上添加下单板，请添加个CStandardMakeOrderBoard类的指针

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); //消息映射函数
	afx_msg void OnSize(UINT nType, int cx, int cy); //消息映射函数

};


