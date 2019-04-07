// CustomGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "CustomGridCtrl.h"
#include "../resource.h"
#include "../Login/LoginDlg.h"

// CCustomGridCtrl

IMPLEMENT_DYNAMIC(CCustomGridCtrl, CGridCtrl)

CCustomGridCtrl::CCustomGridCtrl()
{

}

CCustomGridCtrl::~CCustomGridCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomGridCtrl, CGridCtrl)
//	ON_MESSAGE(WM_UPDATE_GRIDCTRL, &CCustomGridCtrl::OnUpdateGridctrl)
	//此处是通过类向导添加的自定义消息及处理函数
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCustomGridCtrl message handlers




int CCustomGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}

void CCustomGridCtrl::LoadColumns(const vector<ColumnInfo>& header_colums)
{
	if (::IsWindow(GetSafeHwnd()))
	{
		if (header_colums.size() <= 0)
		{
			return;
		}
		
		
		
		DeleteAllItems();
		SetRowCount(1);
		SetColumnCount(header_colums.size());
		SetFixedRowCount(1);
		SetFixedColumnCount(1);

		SetGridLines(GVL_BOTH); 
		SetEditable(FALSE); 
		SetListMode(FALSE);
		EnableSelection(FALSE);
		EnableDragAndDrop(FALSE);
		SetRowResize(TRUE);
		SetColumnResize(TRUE);
		SetHandleTabKey(TRUE);
		SetDoubleBuffering(TRUE);
		SetFrameFocusCell(TRUE);
		SetAutoSizeStyle(GVS_BOTH);
		SetGridLineColor(RGB(0,118,236));


		// fill rows/cols with text
		USES_CONVERSION;
		int row = 0;
		for (int col = 0; col < GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_PARAM;
			Item.row = row;
			Item.col = col;
			Item.lParam = header_colums[col].column_id;
			Item.nFormat = DT_LEFT | DT_WORDBREAK;
			Item.strText.Format(_T("%s"), A2T(header_colums[col].column_name.c_str()));
			SetItem(&Item);
			SetColumnWidth(col, header_colums[col].column_width);
		}
	}
}


