
#include "stdafx.h"

#include "TradeSituationWnd.h"
#include "../Resource.h"
#include "../MainFrm.h"
#include "../../ServiceLayer/DataTypes.h"
#include <atlconv.h>//用于字符转换
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_UPDATE_GRIDCTRL, &COutputWnd::OnUpdateGridCtrl)
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	get_CtpMsgDistributor()->AddReceiver(this);//将COutputWnd类对象添加进接收者容器中

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	// 创建3个子输出窗格: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	if (!m_wndOrderList.Create(rectDummy, &m_wndTabs, ID_WNDORDERLIST, dwStyle) ||
		!m_wndPosition.Create(rectDummy, &m_wndTabs, ID_WNDPOSITION, dwStyle) ||
		!m_wndTrade.Create(rectDummy, &m_wndTabs, ID_WNDTRADE, dwStyle)||
		!m_wndUnfilledOrderList.Create(rectDummy, &m_wndTabs, ID_WNDUNFILLEDORDERLIST, dwStyle)||
		!m_wndPositionDetail.Create(rectDummy, &m_wndTabs, ID_WNDPOSITIONDETAIL, dwStyle)||
		!m_wndCloseTrade.Create(rectDummy, &m_wndTabs, ID_WNDCLOSETRADE, dwStyle) ||
		!m_wndParkedOrderList.Create(rectDummy, &m_wndTabs, ID_WNDPARKEDORDERLIST, dwStyle) ||
		!m_wndParkedOrderActionList.Create(rectDummy, &m_wndTabs, ID_WNDPARKEDORDERACTIONLIST, dwStyle) ||
		!m_wndConditionOrderList.Create(rectDummy, &m_wndTabs, ID_WNDCONDITIONORDER, dwStyle))
	{//1999对应Resource.h中的资源宏ID_WNDPOSITION， 1998对应ID_WNDTRADE，1997对应ID_WNDUNFILLEDORDERLIST   
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	ConfigReader& cfg_reader = get_config_reader();
	cfg_reader.load("brokers.xml", "config.xml");
	vector<ColumnInfo> header_colums = cfg_reader.GetHeaderColumn("ListCtrl所有委托单");
	m_wndOrderList.LoadColumns(header_colums);
	m_wndUnfilledOrderList.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl持仓");
	m_wndPosition.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl成交记录");
	m_wndTrade.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl持仓明细");
	m_wndPositionDetail.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl平仓列表");
	m_wndCloseTrade.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl预埋单列表");
	m_wndParkedOrderList.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl预埋撤单列表");
	m_wndParkedOrderActionList.LoadColumns(header_colums);
	header_colums = cfg_reader.GetHeaderColumn("ListCtrl条件单列表");
	m_wndConditionOrderList.LoadColumns(header_colums);
	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡: 
	bNameValid = strTabName.LoadString(IDS_POSITION_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndPosition, strTabName, (UINT)1);

	bNameValid = strTabName.LoadString(IDS_POSITIONDETAIL_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndPositionDetail, strTabName, (UINT)4);

	bNameValid = strTabName.LoadString(IDS_ORDER_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOrderList, strTabName, (UINT)0);

	bNameValid = strTabName.LoadString(IDS_UNFILLEDORDER_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndUnfilledOrderList, strTabName, (UINT)3);

	bNameValid = strTabName.LoadString(IDS_TRADED_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndTrade, strTabName, (UINT)2);

	bNameValid = strTabName.LoadString(IDS_CLOSETRADED_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndCloseTrade, strTabName, (UINT)5);

	bNameValid = strTabName.LoadString(IDS_CONDITIONORDER_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndConditionOrderList, strTabName, (UINT)8);

	bNameValid = strTabName.LoadString(IDS_PARKEDORDER_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndParkedOrderList, strTabName, (UINT)6);

	bNameValid = strTabName.LoadString(IDS_PARKEDORDERACTION_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndParkedOrderActionList, strTabName, (UINT)7);

	// 使用一些虚拟文本填写输出选项卡(无需复杂数据)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	//m_wndOutputBuild.AddString(_T("生成输出正显示在此处。"));
	//m_wndOutputBuild.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputBuild.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillDebugWindow()
{
	//m_wndOutputDebug.AddString(_T("调试输出正显示在此处。"));
	//m_wndOutputDebug.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputDebug.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillFindWindow()
{
	//m_wndOutputFind.AddString(_T("查找输出正显示在此处。"));
	//m_wndOutputFind.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputFind.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::UpdateFonts()
{
	m_wndOrderList.SetFont(&afxGlobalData.fontRegular);
	m_wndPosition.SetFont(&afxGlobalData.fontRegular);
	m_wndTrade.SetFont(&afxGlobalData.fontRegular);
	m_wndUnfilledOrderList.SetFont(&afxGlobalData.fontRegular);
	m_wndPositionDetail.SetFont(&afxGlobalData.fontRegular);
	m_wndCloseTrade.SetFont(&afxGlobalData.fontRegular);
	m_wndParkedOrderList.SetFont(&afxGlobalData.fontRegular);
	m_wndParkedOrderActionList.SetFont(&afxGlobalData.fontRegular);
	m_wndConditionOrderList.SetFont(&afxGlobalData.fontRegular);
}

void COutputWnd::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryInvestorPosition)
	{//接收到持仓回馈的消息后，去刷新相应的持仓界面
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam==ctp_msg::RspQryOrder)
	{//接收到委托回馈的消息后，去刷新相应的持仓界面
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RtnOrder)
	{//接收到委托回馈的消息后，去刷新相应的持仓界面
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspQryTrade)
	{//接收到委托回馈的消息后，去刷新相应的持仓界面
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RtnTrade)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam==ctp_msg::RspQryInvestorPositionDetail)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspQryParkedOrder)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspParkedOrderInsert)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspRemoveParkedOrder)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspQryParkedOrderAction)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspParkedOrderAction)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
	if (wParam == ctp_msg::RspRemoveParkedOrderAction)
	{
		SendMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);
	}
}

LRESULT COutputWnd::OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam)
{
	//分不同消息去更新窗口
	if (wParam == ctp_msg::RspQryInvestorPosition)
	{
		vector<InvestorPosition>* InvestorPositions_list = reinterpret_cast<vector<InvestorPosition>*>(lParam);
		vector<InvestorPosition> InvestorPositions_list_merged; //已做合并处理的持仓vector
		CString Instrument_id_mergedvector;
		CString direction_mergedvector;
		InvestorPosition position_merged;
		if (InvestorPositions_list!=NULL)
		{
			for (int i = 0; i < InvestorPositions_list->size(); ++i)
			{
				InvestorPosition Position = (*InvestorPositions_list)[i];
				CString Instrument_id(Position.InstrumentID.c_str());//获取行头
				CString direction(Position.PosiDirection);//由char到cstring可自动转换
				position_merged = Position;
				vector<InvestorPosition>::iterator it;
				for ( it = InvestorPositions_list_merged.begin(); it != InvestorPositions_list_merged.end(); )
				{
					Instrument_id_mergedvector = (*it).InstrumentID.c_str();//获取行头
					direction_mergedvector = (*it).PosiDirection;                //由char到cstring可自动转换
					if (Instrument_id == Instrument_id_mergedvector && direction == direction_mergedvector)
					{
						//先处理这么多数据, 有些数据需合并,有些数据不需合并,后期可完善
						position_merged.PositionProfit += (*it).PositionProfit;
						position_merged.Position += (*it).Position;
						position_merged.YdPosition += (*it).YdPosition;
						position_merged.TodayPosition += (*it).TodayPosition;
						position_merged.UseMargin += (*it).UseMargin;
						position_merged.OpenCost += (*it).OpenCost;
						//合并完后,将原有的删除掉
						it=InvestorPositions_list_merged.erase(it);
					}
					//防止出现iterator not incrementable的解决办法
					else
					{
						++it;
					}
				}
				InvestorPositions_list_merged.push_back(position_merged); //将经过合并的持仓压入
			}
		}

		if (InvestorPositions_list_merged.size()!=0)
		{
			for (int i = 0; i < InvestorPositions_list_merged.size(); ++i)
			{
				InvestorPosition Position = InvestorPositions_list_merged[i];
				CString Instrument_id(Position.InstrumentID.c_str());//获取行头
				CString direction(Position.PosiDirection);//由char到cstring可自动转换
				USES_CONVERSION;
				string direction_stringtype = CT2A(direction.GetBuffer()); //将cstring转换为string
				// fill rows/cols with text
				int row = 1;
				bool bFined = false;

				for (; row <= m_wndPosition.GetRowCount(); row++)//从第一行开始去找，因为第0行是列头行
				{
					CString row_data = m_wndPosition.GetItemText(row, 0);//比较各行行头与Instrumentid是否一致
					CString row_Instrument_id(row_data);

					row_data = m_wndPosition.GetItemText(row, 2);//此处要注意第2列位置必须是买卖方向
					CString row_direction(row_data);

					USES_CONVERSION;
					string row_direction_stringtype = CT2A(row_direction.GetBuffer()); //将cstring转换为string

					if (row_Instrument_id == Instrument_id)
					{
						if ((row_direction_stringtype == "空" && direction_stringtype == "3") || (row_direction_stringtype == "多" && direction_stringtype == "2") || (row_direction_stringtype == "净" && direction_stringtype == "1"))
						{
							bFined = true;
							break;//匹配成功就break出去，此时的row就是我们要找的用户的信息
						}
					}
				}
				//每一列对应一个字段，开始遍历该用户的所有信息
				for (int col = 0; col < m_wndPosition.GetColumnCount(); col++)
				{
					GV_ITEM Item;
					Item.mask = GVIF_TEXT | GVIF_FORMAT;
					Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
					Item.col = col;//不同列名的所在列号,指定item操作的列
					//Item.lParam = (LPARAM)CString(base_data.code).GetBuffer();
					int field_id = m_wndPosition.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
					CString disp_str(GetTextById(Position, field_id).c_str());//根据field_id获取要求的值
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
					Item.strText.Format(_T("%s"), disp_str);
					if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
					{
						m_wndPosition.SetItem(&Item);
					}
					else
					{
						row = m_wndPosition.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
						bFined = true;
					}
					if (FIELD_ID_InvestorPosition::FIELD_FloatProfit_Position == field_id)//若此时是刷新行头合约名数据
					{
						string floatprofit_str = CT2A(m_wndPosition.GetItemText(row, col).GetBuffer());
						double floatprofit = stod(floatprofit_str);
						if (floatprofit <= 0)
						{
							m_wndPosition.SetItemBkColour(row, col, RGB(0, 255, 0)); //设置背景色
							m_wndPosition.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
						}
						if (floatprofit > 0)
						{
							m_wndPosition.SetItemBkColour(row, col, RGB(255, 0, 0)); //设置背景色
							m_wndPosition.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
						}
					}
				}
				m_wndPosition.Refresh();
				//遍历持仓列表，将已经持仓为空的改行删除掉
				if (Position.Position == 0)
				{
					for (int rowcnt = 1; rowcnt < m_wndPosition.GetRowCount(); rowcnt++)
					{
						CString PositionValue = m_wndPosition.GetItemText(rowcnt, 6); //要确保第6列处一定是持仓字段
						USES_CONVERSION;
						string PositionValue_stringtype = CT2A(PositionValue.GetBuffer()); //将cstring转换为string
						if (PositionValue_stringtype == "0")
						{
							m_wndPosition.DeleteRow(rowcnt);
							break;
						}
					}
					continue;
				}
			}
		}
		return 1;
	}

	
	if (wParam == ctp_msg::RspQryOrder)
	{
		vector<Order>* Orders_list = reinterpret_cast<vector<Order>*>(lParam);
		if (Orders_list != NULL)
		{
			for (int i = 0; i < Orders_list->size(); ++i) //循环取其中的各个元素
			{
				Order order = (*Orders_list)[i];
				CString Instrument_id(order.InstrumentID);//获取行头

				{//处理所有委托部分
					if (  !(order.OrderStatus == THOST_FTDC_OST_NotTouched || order.OrderStatus == THOST_FTDC_OST_Touched)   )
					{//将触价条件单过滤出去，已经交由条件单面板部分处理
						if (order.ContingentCondition == THOST_FTDC_CC_Immediately )
						{//为了过滤掉条件单撤单，条件单撤单后也是撤单状态，但是触发类型不是立即
							bool bFined = false;
							int row = m_wndOrderList.GetRowCount(); //获取起始行
							for (int col = 0; col < m_wndOrderList.GetColumnCount(); col++)
							{//每一列对应一个字段，开始遍历该用户的所有信息
								GV_ITEM Item;
								Item.mask = GVIF_TEXT | GVIF_FORMAT;
								Item.row = row;
								Item.col = col;//不同列名的所在列号,指定item操作的列
								int field_id = m_wndOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
								CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
								Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
								Item.strText.Format(_T("%s"), disp_str);
								if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
								{
									m_wndOrderList.SetItem(&Item);
								}
								else
								{
									row = m_wndOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
									bFined = true;
								}
							}
							m_wndOrderList.Refresh();
						}
					}
				}
				
				{//处理未成委托部分
					if ( !(order.OrderStatus == THOST_FTDC_OST_AllTraded || 
						order.OrderStatus == THOST_FTDC_OST_Canceled || 
						order.OrderStatus == THOST_FTDC_OST_Unknown || 
						order.OrderStatus == THOST_FTDC_OST_Touched ||
						order.OrderStatus == THOST_FTDC_OST_NotTouched || 
						order.OrderStatus == THOST_FTDC_OST_Touched) )
					{//将不属于未成委托的continue出去
						bool bFined = false;
						int row = m_wndUnfilledOrderList.GetRowCount(); //获取起始行
						for (int col = 0; col < m_wndUnfilledOrderList.GetColumnCount(); col++)
						{//每一列对应一个字段，开始遍历该用户的所有信息
							GV_ITEM Item;
							Item.mask = GVIF_TEXT | GVIF_FORMAT;
							Item.row = row;
							Item.col = col;//不同列名的所在列号,指定item操作的列
							int field_id = m_wndUnfilledOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
							CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
							Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
							Item.strText.Format(_T("%s"), disp_str);
							if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
							{
								m_wndUnfilledOrderList.SetItem(&Item);
							}
							else
							{
								row = m_wndUnfilledOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
								bFined = true;
							}
						}
						m_wndUnfilledOrderList.Refresh();
					}
				}

				{//处理条件单部分
					if (  order.ContingentCondition == THOST_FTDC_CC_Immediately  )
					{//条件单的触发条件不是立即触发，所以除开立即触发类型的，就是条件单的信息，应接收处理
						continue;
					}
					bool bFined = false;
					int row = m_wndConditionOrderList.GetRowCount(); //获取起始行
					for (int col = 0; col < m_wndConditionOrderList.GetColumnCount(); col++)
					{//每一列对应一个字段，开始遍历该用户的所有信息
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;
						Item.col = col;//不同列名的所在列号,指定item操作的列
						int field_id = m_wndConditionOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
						CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndConditionOrderList.SetItem(&Item);
						}
						else
						{
							row = m_wndConditionOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
					}
					m_wndConditionOrderList.Refresh();
				}
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RtnOrder)
	{
		Order* porder = reinterpret_cast<Order*>(lParam);
		if (porder != NULL)
		{
			Order order = *porder;
			if (order.OrderStatus == THOST_FTDC_OST_Unknown) //如果报单已提交，则报单状态为未知，我们不需要无内容含量的信息，为此直接过滤掉
			{
				return 1;
			}
			CString Instrument_id(order.InstrumentID);//获取行头

			{//处理委托部分
				if (!(order.OrderStatus == THOST_FTDC_OST_NotTouched || order.OrderStatus == THOST_FTDC_OST_Touched))
				{//非触发条件单及已触发条件单都不归委托部分处理
					if (order.ContingentCondition == THOST_FTDC_CC_Immediately)
					{//为了过滤掉条件单撤单，条件单撤单后也是撤单状态，但是触发类型不是立即
						for (int j = 1; j <= m_wndOrderList.GetRowCount() - 1; ++j)
						{//row==1处开始是数据行, m_wndOrderList.GetRowCount()-1为数据行行数, 行标从0开始,第0行为列头行
							CString orderSysID = m_wndOrderList.GetItemText(j, m_wndOrderList.GetColumnCount() - 1);//要确保最后一列为报单编号
							if (orderSysID == order.OrderSysID) //要保证委托列表中报单编号的唯一性
							{
								if (orderSysID != _T(""))
								{//在非交易时段的报单是没有报单编号的，此时不要覆盖
									m_wndOrderList.DeleteRow(j);
								}
							}
						}
						bool bFined = false; //尚未插入改行
						int row = m_wndOrderList.GetRowCount(); //获取起始行ID
						for (int col = 0; col < m_wndOrderList.GetColumnCount(); col++)
						{//每一列对应一个字段，开始遍历所有信息
							GV_ITEM Item;
							Item.mask = GVIF_TEXT | GVIF_FORMAT;
							Item.row = row;
							Item.col = col;//不同列名的所在列号,指定item操作的列
							int field_id = m_wndOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
							CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
							Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
							Item.strText.Format(_T("%s"), disp_str);
							if (bFined)//若已插入该行，则只做刷新
							{
								m_wndOrderList.SetItem(&Item);
							}
							else
							{
								row = m_wndOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名, 要插入到第一行中
								bFined = true;
							}
						}
						m_wndOrderList.Refresh();
					}
				}
			}

			{//处理未成委托部分
				if ( !(order.OrderStatus == THOST_FTDC_OST_NotTouched || order.OrderStatus == THOST_FTDC_OST_Touched) )
				{//非触发条件单及已触发条件单都不归未成委托部分处理
					if (order.ContingentCondition == THOST_FTDC_CC_Immediately)
					{//为了过滤掉条件单撤单，条件单撤单后也是撤单状态，但是触发类型不是立即
						if (order.OrderStatus == THOST_FTDC_OST_AllTraded || order.OrderStatus == THOST_FTDC_OST_Canceled || order.OrderStatus == THOST_FTDC_OST_Touched)
						{//要检查是否是已有委托单的全部成交及撤单,  若是,  则要从未成委托列表中删除相应委托行
							for (int j = 1; j <= m_wndUnfilledOrderList.GetRowCount() - 1; ++j)
							{
								CString orderSysID = m_wndUnfilledOrderList.GetItemText(j, m_wndUnfilledOrderList.GetColumnCount() - 1);//要确保最后一列为报单编号
								if (orderSysID == order.OrderSysID) //要保证委托列表中报单编号的唯一性
								{
									m_wndUnfilledOrderList.DeleteRow(j);
									m_wndUnfilledOrderList.Refresh();
									return 1;//删完该行后,直接return出去
								}
							}
							return 1; //即使在未成委托中找不到相应未成委托,也应该return出去,防止添加
						}
						for (int j = 1; j <= m_wndUnfilledOrderList.GetRowCount() - 1; ++j)
						{//row==1处开始是数据行, m_wndUnfilledOrderList.GetRowCount()-1为数据行行数, 行标从0开始,第0行为列头行
							CString orderSysID = m_wndUnfilledOrderList.GetItemText(j, m_wndUnfilledOrderList.GetColumnCount() - 1);//要确保最后一列为报单编号
							if (orderSysID == order.OrderSysID) //要保证委托列表中报单编号的唯一性
							{
								if (orderSysID != _T(""))
								{//在非交易时段的报单是没有报单编号的，此时不要覆盖
									m_wndUnfilledOrderList.DeleteRow(j);
								}
							}
						}
						bool bFined = false; //尚未插入该行
						int row = m_wndUnfilledOrderList.GetRowCount(); //获取起始行ID
						for (int col = 0; col < m_wndUnfilledOrderList.GetColumnCount(); col++)
						{//每一列对应一个字段，开始遍历所有信息
							GV_ITEM Item;
							Item.mask = GVIF_TEXT | GVIF_FORMAT;
							Item.row = row;
							Item.col = col;//不同列名的所在列号,指定item操作的列
							int field_id = m_wndUnfilledOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
							CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
							Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
							Item.strText.Format(_T("%s"), disp_str);
							if (bFined)//若已插入该行，则只做刷新
							{
								m_wndUnfilledOrderList.SetItem(&Item);
							}
							else
							{
								row = m_wndUnfilledOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
								bFined = true;
							}
						}
						m_wndUnfilledOrderList.Refresh();
					}
				}
			}

			{//处理条件单部分
				if (  !(order.ContingentCondition == THOST_FTDC_CC_Immediately)  )
				{//尚未触发及已触发条件单归此处处理
					for (int j = 1; j <= m_wndConditionOrderList.GetRowCount() - 1; ++j)
					{//row==1处开始是数据行, m_wndOrderList.GetRowCount()-1为数据行行数, 行标从0开始,第0行为列头行
						CString orderSysID = m_wndConditionOrderList.GetItemText(j, m_wndConditionOrderList.GetColumnCount() - 1);//要确保最后一列为报单编号
						if (orderSysID == order.OrderSysID) //要保证委托列表中报单编号的唯一性
						{
							if (orderSysID != _T(""))
							{//在非交易时段的报单是没有报单编号的，此时不要覆盖
								m_wndConditionOrderList.DeleteRow(j);
							}
						}
					}
					bool bFined = false; //尚未插入改行
					int row = m_wndConditionOrderList.GetRowCount(); //获取起始行ID
					for (int col = 0; col < m_wndConditionOrderList.GetColumnCount(); col++)
					{//每一列对应一个字段，开始遍历所有信息
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;
						Item.col = col;//不同列名的所在列号,指定item操作的列
						int field_id = m_wndConditionOrderList.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
						CString disp_str(GetTextById(order, field_id).c_str());//根据field_id获取要求的值
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若已插入该行，则只做刷新
						{
							m_wndConditionOrderList.SetItem(&Item);
						}
						else
						{
							row = m_wndConditionOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名, 要插入到第一行中
							bFined = true;
						}
					}
					m_wndConditionOrderList.Refresh();
				}
			}

		}
		return 1;
	}

	if (wParam==ctp_msg::RspQryTrade)
	{
		vector<Trade>* Trades_list = reinterpret_cast<vector<Trade>*>(lParam);
		if (Trades_list != NULL)
		{
			for (int i = 0; i < Trades_list->size(); ++i) //循环取其中的各个元素
			{
				Trade trade = (*Trades_list)[i];
				CString Instrument_id(trade.InstrumentID);//获取行头
				{//处理所有委托部分
					bool bFined = false;
					int row = m_wndTrade.GetRowCount(); //获取起始行
					for (int col = 0; col < m_wndTrade.GetColumnCount(); col++)
					{//每一列对应一个字段，开始遍历该用户的所有信息
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;
						Item.col = col;//不同列名的所在列号,指定item操作的列
						int field_id = m_wndTrade.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
						CString disp_str(GetTextById(trade, field_id).c_str());//根据field_id获取要求的值
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndTrade.SetItem(&Item);
						}
						else
						{
							row = m_wndTrade.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
					}
					m_wndTrade.Refresh();
				}
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RtnTrade)
	{
		Trade* ptrade = reinterpret_cast<Trade*>(lParam);
		if (ptrade != NULL)
		{
			Trade trade = *ptrade;
			CString Instrument_id(trade.InstrumentID);
			{
				bool bFined = false; 
				int row = m_wndTrade.GetRowCount(); //获取起始行index
				for (int col = 0; col < m_wndTrade.GetColumnCount(); col++)
				{
					GV_ITEM Item;
					Item.mask = GVIF_TEXT | GVIF_FORMAT;
					Item.row = row;
					Item.col = col;
					int field_id = m_wndTrade.GetItemData(0, col);
					CString disp_str(GetTextById(trade, field_id).c_str());
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
					Item.strText.Format(_T("%s"), disp_str);
					if (bFined)
					{
						m_wndTrade.SetItem(&Item);
					}
					else
					{
						row = m_wndTrade.InsertRow(Instrument_id);
						bFined = true;
					}
				}
				m_wndTrade.Refresh();
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspQryInvestorPositionDetail)
	{
		{//更新持仓明细列表
			vector<InvestorPositionDetail>* InvestorPositionDetails_list = reinterpret_cast<vector<InvestorPositionDetail>*>(lParam);
			if (InvestorPositionDetails_list != NULL)
			{
				int row = 0;
				for (int i = 0; i < InvestorPositionDetails_list->size(); ++i)
				{
					InvestorPositionDetail PositionDetail = (*InvestorPositionDetails_list)[i];
					CString Instrument_id(PositionDetail.InstrumentID);//获取行头
					CString trade_id(PositionDetail.TradeID);//获取传入数据的成交编号

					bool bFined = false;//一开始假设不存在
					row = m_wndPositionDetail.GetRowCount(); //一开始假设不存在,设定插入行Index
					for (int row_index = 1; row_index < m_wndPositionDetail.GetRowCount(); row_index++)
					{
						if (trade_id == m_wndPositionDetail.GetItemText(row_index, m_wndPositionDetail.GetColumnCount() - 1)) //要保证最后一列为成交编号
						{
							bFined = true;
							row = row_index; //获取row index,从0开始
							break;
						}
					}
					for (int col = 0; col < m_wndPositionDetail.GetColumnCount(); col++)
					{//每一列对应一个字段，开始遍历该用户的所有信息
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
						Item.col = col;//不同列名的所在列号,指定item操作的列
						int field_id = m_wndPositionDetail.GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
						CString disp_str(GetTextById(PositionDetail, field_id).c_str());//根据field_id获取要求的值
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndPositionDetail.SetItem(&Item);
						}
						else
						{
							row = m_wndPositionDetail.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
						if (FIELD_ID_InvestorPositionDetail::FIELD_PositionProfitByTrade_PositionDetail == field_id)//若此时是刷新行头合约名数据
						{
							string floatprofit_str = CT2A(m_wndPositionDetail.GetItemText(row, col).GetBuffer());
							double floatprofit = stod(floatprofit_str);
							if (floatprofit <= 0)
							{
								m_wndPositionDetail.SetItemBkColour(row, col, RGB(0, 255, 0)); //设置背景色
								m_wndPositionDetail.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
							}
							if (floatprofit > 0)
							{
								m_wndPositionDetail.SetItemBkColour(row, col, RGB(255, 0, 0)); //设置背景色
								m_wndPositionDetail.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
							}
						}
					}
					m_wndPositionDetail.Refresh();
					//m_wndPositionDetail.ExpandColumnsToFit(TRUE);
					//遍历持仓列表，将已经持仓为空的改行删除掉
					if (PositionDetail.Volume == 0)
					{
						for (int rowcnt = 1; rowcnt < m_wndPositionDetail.GetRowCount(); rowcnt++)
						{
							CString PositionValue = m_wndPositionDetail.GetItemText(rowcnt, 3); //要确保第3列处一定是持仓量字段
							USES_CONVERSION;
							string PositionValue_stringtype = CT2A(PositionValue.GetBuffer()); //将cstring转换为string
							if (PositionValue_stringtype == "0")
							{
								m_wndPositionDetail.DeleteRow(rowcnt);
								break;
							}
						}
						continue;
					}
				}
			}
		}

		{//借助持仓明细来更新平仓列表
			vector<InvestorPositionDetail> InvestorPositionDetail_vector;
			get_global_mgr().GetService()->get_all_InvestorPositionDetails(InvestorPositionDetail_vector);
			if (InvestorPositionDetail_vector.size() != 0)
			{
				int row = 0;
				for (int i = 0; i < InvestorPositionDetail_vector.size(); ++i)
				{
					InvestorPositionDetail PositionDetail = InvestorPositionDetail_vector[i];
					if (PositionDetail.CloseVolume == 0)
					{//非平仓数据过滤掉
						continue;
					}
					CString Instrument_id(PositionDetail.InstrumentID);//获取行头
					CString trade_id(PositionDetail.TradeID);//获取传入数据的成交编号
					bool bFined = false;//一开始假设不存在
					row = m_wndCloseTrade.GetRowCount(); //一开始假设不存在,设定插入行Index
					for (int row_index = 1; row_index < m_wndCloseTrade.GetRowCount(); row_index++)
					{
						if (trade_id == m_wndCloseTrade.GetItemText(row_index, m_wndCloseTrade.GetColumnCount() - 1)) //要保证最后一列为成交编号
						{
							bFined = true;
							row = row_index; //获取row index,
							break;
						}
					}
					int col = 0;
					for (col = 0; col < m_wndCloseTrade.GetColumnCount(); col++)
					{
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
						Item.col = col;//不同列名的所在列号,指定item操作的列
						CString disp_str;
						if (m_wndCloseTrade.GetItemText(0, col) == _T("合约"))
						{
							disp_str = PositionDetail.InstrumentID;
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("买卖"))
						{
							if (PositionDetail.Direction == THOST_FTDC_D_Buy)
							{
								disp_str = _T("多");
							}
							if (PositionDetail.Direction == THOST_FTDC_D_Sell)
							{
								disp_str = _T("空");
							}
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("平仓量"))
						{
							disp_str = to_string(PositionDetail.CloseVolume).c_str();
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("开仓价"))
						{
							char OpenPrice_cstr[100];
							sprintf(OpenPrice_cstr, "%0.2f", PositionDetail.OpenPrice); //截取为两位精度后转化为字符串类型
							disp_str = OpenPrice_cstr;
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("平仓价"))
						{
							int Instrument_Multiple = (PositionDetail.CloseAmount - (PositionDetail.CloseProfitByTrade / PositionDetail.CloseVolume)) / PositionDetail.OpenPrice;
							double ClosePrice = PositionDetail.CloseAmount / Instrument_Multiple;
							char ClosePrice_cstr[100];
							sprintf(ClosePrice_cstr, "%0.2f", ClosePrice); //截取为两位精度后转化为字符串类型
							disp_str = ClosePrice_cstr;
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("平仓盈亏"))
						{
							char CloseProfitByTrade_cstr[100];
							sprintf(CloseProfitByTrade_cstr, "%0.2f", PositionDetail.CloseProfitByTrade); //截取为两位精度后转化为字符串类型
							disp_str = CloseProfitByTrade_cstr;
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("开仓日期"))
						{
							disp_str = PositionDetail.OpenDate;
						}
						else if (m_wndCloseTrade.GetItemText(0, col) == _T("成交编号"))
						{
							disp_str = PositionDetail.TradeID;
						}
						else
						{
							disp_str = _T("---");
						}
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndCloseTrade.SetItem(&Item);
						}
						else
						{
							row = m_wndCloseTrade.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
						if (m_wndCloseTrade.GetItemText(0, col) == _T("平仓盈亏"))
						{
							string closeprofit_str = CT2A(m_wndCloseTrade.GetItemText(row, col).GetBuffer());
							double closeprofit = stod(closeprofit_str);
							if (closeprofit <= 0)
							{
								m_wndCloseTrade.SetItemBkColour(row, col, RGB(0, 255, 0)); //设置背景色
								m_wndCloseTrade.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
							}
							if (closeprofit > 0)
							{
								m_wndCloseTrade.SetItemBkColour(row, col, RGB(255, 0, 0)); //设置背景色
								m_wndCloseTrade.SetItemFgColour(row, col, RGB(0, 0, 0)); //设置前景色
							}
						}
					}
					m_wndCloseTrade.Refresh();
				}
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspQryParkedOrder)
	{
		{//更新预埋单列表
			vector<ParkedOrder>* parkedOrder_list = reinterpret_cast<vector<ParkedOrder>*>(lParam);
			if (parkedOrder_list != NULL)
			{
				int row = 0;
				for (int i = 0; i < parkedOrder_list->size(); ++i)
				{
					ParkedOrder parkedOrder = (*parkedOrder_list)[i];
					CString Instrument_id(parkedOrder.InstrumentID);//获取行头
					CString parkedOrder_id(parkedOrder.ParkedOrderID);//获取传入数据的报单编号
					int  Error_id(parkedOrder.ErrorID);//获取传入数据的错误ID
					/*
					if (Error_id != 0)
					{//有错误的预埋报单不是待成交的预埋报单,过滤掉, 暂时不过滤
					continue;
					}
					*/
					bool bFined = false;//一开始假设不存在
					row = m_wndParkedOrderList.GetRowCount(); //一开始假设不存在,设定插入行Index
					for (int row_index = 1; row_index < m_wndParkedOrderList.GetRowCount(); row_index++)
					{
						if (parkedOrder_id == m_wndParkedOrderList.GetItemText(row_index, m_wndParkedOrderList.GetColumnCount() - 1)) //要保证最后一列为预埋报单编号
						{
							bFined = true;
							row = row_index; //获取row index
							break;
						}
					}
					int col = 0;
					for (col = 0; col < m_wndParkedOrderList.GetColumnCount(); col++)
					{//每一列对应一个字段
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
						Item.col = col;//不同列名的所在列号,指定item操作的列
						CString disp_str;
						if (m_wndParkedOrderList.GetItemText(0, col) == _T("合约"))
						{
							disp_str = parkedOrder.InstrumentID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("买卖"))
						{
							if (parkedOrder.Direction == THOST_FTDC_D_Buy)
							{
								disp_str = _T("多");
							}
							if (parkedOrder.Direction == THOST_FTDC_D_Sell)
							{
								disp_str = _T("空");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报单手数"))
						{
							disp_str = to_string(parkedOrder.VolumeTotalOriginal).c_str();
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报单价格"))
						{
							char OpenPrice_cstr[100];
							sprintf(OpenPrice_cstr, "%0.2f", parkedOrder.LimitPrice); //截取为两位精度后转化为字符串类型
							disp_str = OpenPrice_cstr;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("触发价"))
						{
							char	TriggerPrice_cstr[100];
							sprintf(TriggerPrice_cstr, "%0.2f", parkedOrder.StopPrice); //截取为两位精度后转化为字符串类型
							disp_str = TriggerPrice_cstr;
							if (disp_str == _T("0.00") || parkedOrder.StopPrice<0)
							{//parkedOrder.StopPrice<0说明是乱码,处理某些少见的异常情况
								disp_str = _T("无");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("触发条件"))
						{
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_Immediately)
							{
								disp_str = _T("立即");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_Touch)
							{
								disp_str = _T("止损");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_TouchProfit)
							{
								disp_str = _T("止盈");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_ParkedOrder)
							{
								disp_str = _T("预埋单");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice)
							{
								disp_str = _T("最新价大于等于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterThanStopPrice)
							{
								disp_str = _T("最新价大于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice)
							{
								disp_str = _T("最新价小于等于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceLesserThanStopPrice)
							{
								disp_str = _T("最新价小于条件价");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("状态"))
						{
							if (parkedOrder.Status == THOST_FTDC_PAOS_NotSend)
							{
								disp_str = _T("未发送");
							}
							if (parkedOrder.Status == THOST_FTDC_PAOS_Send)
							{
								disp_str = _T("已发送");
							}
							if (parkedOrder.Status == THOST_FTDC_PAOS_Deleted)
							{
								disp_str = _T("已删除");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("预埋单编号"))
						{
							disp_str = parkedOrder.ParkedOrderID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("交易所"))
						{
							disp_str = parkedOrder.ExchangeID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("开平"))
						{
							if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
							{
								disp_str = _T("开仓");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_Close)
							{
								disp_str = _T("平仓");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
							{
								disp_str = _T("平今");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_ForceClose)
							{
								disp_str = _T("强平");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
							{
								disp_str = _T("平昨");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_ForceOff)
							{
								disp_str = _T("强减");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_LocalForceClose)
							{
								disp_str = _T("本地强平");
							}
							else
							{
								disp_str = _T("---");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报价类型"))
						{
							if (parkedOrder.OrderPriceType == THOST_FTDC_OPT_AnyPrice)
							{
								disp_str = _T("市价");
							}
							else if (parkedOrder.OrderPriceType == THOST_FTDC_OPT_LimitPrice)
							{
								disp_str = _T("限价");
							}
							else
							{
								disp_str = _T("其它");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("错误信息"))
						{
							disp_str = parkedOrder.ErrorMsg;
							if (disp_str==_T(""))
							{
								disp_str = _T("无");
							}
						}
						else
						{
							disp_str = _T("---");
						}
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndParkedOrderList.SetItem(&Item);
						}
						else
						{
							row = m_wndParkedOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
					}
					m_wndParkedOrderList.Refresh();
				}
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspParkedOrderInsert)
	{
		{//必须在非交易时段,下预埋单才能成功,当下预埋单成功后,刷新预埋记录列表
			ParkedOrder* pParkedOrder = reinterpret_cast<ParkedOrder*>(lParam);
			ParkedOrder parkedOrder = *pParkedOrder;
			if (pParkedOrder != NULL)
			{
				int row = 0;
				CString Instrument_id(parkedOrder.InstrumentID);//获取行头
				CString parkedOrder_id(parkedOrder.ParkedOrderID);//获取传入数据的报单编号
					bool bFined = false;//一开始假设不存在
					row = m_wndParkedOrderList.GetRowCount(); //一开始假设不存在,设定插入行Index
					for (int row_index = 1; row_index < m_wndParkedOrderList.GetRowCount(); row_index++)
					{
						if (parkedOrder_id == m_wndParkedOrderList.GetItemText(row_index, m_wndParkedOrderList.GetColumnCount() - 1)) //要保证最后一列为预埋报单编号
						{
							bFined = true;
							row = row_index; //获取row index
							break;
						}
					}
					int col = 0;
					for (col = 0; col < m_wndParkedOrderList.GetColumnCount(); col++)
					{//每一列对应一个字段
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
						Item.col = col;//不同列名的所在列号,指定item操作的列
						CString disp_str;
						if (m_wndParkedOrderList.GetItemText(0, col) == _T("合约"))
						{
							disp_str = parkedOrder.InstrumentID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("买卖"))
						{
							if (parkedOrder.Direction == THOST_FTDC_D_Buy)
							{
								disp_str = _T("多");
							}
							if (parkedOrder.Direction == THOST_FTDC_D_Sell)
							{
								disp_str = _T("空");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报单手数"))
						{
							disp_str = to_string(parkedOrder.VolumeTotalOriginal).c_str();
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报单价格"))
						{
							char OpenPrice_cstr[100];
							sprintf(OpenPrice_cstr, "%0.2f", parkedOrder.LimitPrice); //截取为两位精度后转化为字符串类型
							disp_str = OpenPrice_cstr;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("触发价"))
						{
							char	TriggerPrice_cstr[100];
							sprintf(TriggerPrice_cstr, "%0.2f", parkedOrder.StopPrice); //截取为两位精度后转化为字符串类型
							disp_str = TriggerPrice_cstr;
							if (disp_str == _T("0.00") || parkedOrder.StopPrice<0)
							{//parkedOrder.StopPrice<0说明是乱码,处理某些少见的异常情况
								disp_str = _T("无");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("触发条件"))
						{
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_Immediately)
							{
								disp_str = _T("立即");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_Touch)
							{
								disp_str = _T("止损");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_TouchProfit)
							{
								disp_str = _T("止盈");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_ParkedOrder)
							{
								disp_str = _T("预埋单");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice)
							{
								disp_str = _T("最新价大于等于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterThanStopPrice)
							{
								disp_str = _T("最新价大于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice)
							{
								disp_str = _T("最新价小于等于条件价");
							}
							if (parkedOrder.ContingentCondition == THOST_FTDC_CC_LastPriceLesserThanStopPrice)
							{
								disp_str = _T("最新价小于条件价");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("状态"))
						{
							if (parkedOrder.Status == THOST_FTDC_PAOS_NotSend)
							{
								disp_str = _T("未发送");
							}
							if (parkedOrder.Status == THOST_FTDC_PAOS_Send)
							{
								disp_str = _T("已发送");
							}
							if (parkedOrder.Status == THOST_FTDC_PAOS_Deleted)
							{
								disp_str = _T("已删除");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("预埋单编号"))
						{
							disp_str = parkedOrder.ParkedOrderID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("交易所"))
						{
							disp_str = parkedOrder.ExchangeID;
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("开平"))
						{
							if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
							{
								disp_str = _T("开仓");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_Close)
							{
								disp_str = _T("平仓");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
							{
								disp_str = _T("平今");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_ForceClose)
							{
								disp_str = _T("强平");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_CloseYesterday)
							{
								disp_str = _T("平昨");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_ForceOff)
							{
								disp_str = _T("强减");
							}
							else if (parkedOrder.CombOffsetFlag[0] == THOST_FTDC_OF_LocalForceClose)
							{
								disp_str = _T("本地强平");
							}
							else
							{
								disp_str = _T("---");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("报价类型"))
						{
							if (parkedOrder.OrderPriceType == THOST_FTDC_OPT_AnyPrice)
							{
								disp_str = _T("市价");
							}
							else if (parkedOrder.OrderPriceType == THOST_FTDC_OPT_LimitPrice)
							{
								disp_str = _T("限价");
							}
							else
							{
								disp_str = _T("其它");
							}
						}
						else if (m_wndParkedOrderList.GetItemText(0, col) == _T("错误信息"))
						{
							disp_str = parkedOrder.ErrorMsg;
							if (disp_str == _T(""))
							{
								disp_str = _T("无");
							}
						}
						else
						{
							disp_str = _T("---");
						}
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndParkedOrderList.SetItem(&Item);
						}
						else
						{
							row = m_wndParkedOrderList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
					}
					m_wndParkedOrderList.Refresh();
				}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspRemoveParkedOrder)
	{//删除后只更新状态
		Remove_ParkedOrder* pParkedOrder = reinterpret_cast<Remove_ParkedOrder*>(lParam);
		CString parkedOrderID(pParkedOrder->ParkedOrderID);
		int row = 0;
		for (int row_index = 1; row_index < m_wndParkedOrderList.GetRowCount(); row_index++)
		{
			if (parkedOrderID == m_wndParkedOrderList.GetItemText(row_index, m_wndParkedOrderList.GetColumnCount() - 1)) //要保证最后一列为预埋报单编号
			{
				row = row_index; //获取row index
				break;
			}
		}
		CString disp_str;
		int col = 0;
		for (col = 0; col < m_wndParkedOrderList.GetColumnCount(); col++)
		{//每一列对应一个字段
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
			Item.col = col;//不同列名的所在列号,指定item操作的列
			if (m_wndParkedOrderList.GetItemText(0, col) == _T("状态"))
			{
				disp_str = _T("已删除");
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format(_T("%s"), disp_str);
				m_wndParkedOrderList.SetItem(&Item);
				break;
			}
		}
		m_wndParkedOrderList.Refresh();
	}


	if (wParam == ctp_msg::RspQryParkedOrderAction)
	{
		{//更新预埋撤单列表
			vector<ParkedOrderAction>* parkedOrderAction_list = reinterpret_cast<vector<ParkedOrderAction>*>(lParam);
			if (parkedOrderAction_list != NULL)
			{
				int row = 0;
				for (int i = 0; i < parkedOrderAction_list->size(); ++i)
				{
					ParkedOrderAction parkedOrderAction = (*parkedOrderAction_list)[i];
					CString Instrument_id(parkedOrderAction.InstrumentID);//获取行头
					CString parkedOrderAction_id(parkedOrderAction.ParkedOrderActionID);//获取传入数据的报单编号
					int  Error_id(parkedOrderAction.ErrorID);//获取传入数据的错误ID
					/*
					if (Error_id != 0)
					{//有错误的预埋撤单报单不是待成交的预埋撤单报单,过滤掉, 暂时不过滤
					continue;
					}
					*/
					bool bFined = false;//一开始假设不存在
					row = m_wndParkedOrderActionList.GetRowCount(); //一开始假设不存在,设定插入行Index
					for (int row_index = 1; row_index < m_wndParkedOrderActionList.GetRowCount(); row_index++)
					{
						if (parkedOrderAction_id == m_wndParkedOrderActionList.GetItemText(row_index, m_wndParkedOrderActionList.GetColumnCount() - 1)) //要保证最后一列为预埋撤单报单编号
						{
							bFined = true;
							row = row_index; //获取row index
							break;
						}
					}
					int col = 0;
					for (col = 0; col < m_wndParkedOrderActionList.GetColumnCount(); col++)
					{//每一列对应一个字段
						GV_ITEM Item;
						Item.mask = GVIF_TEXT | GVIF_FORMAT;
						Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
						Item.col = col;//不同列名的所在列号,指定item操作的列
						CString disp_str;
						if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("合约"))
						{
							disp_str = parkedOrderAction.InstrumentID;
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("交易量"))
						{
							disp_str = to_string(parkedOrderAction.VolumeChange).c_str();
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("价格"))
						{
							char OpenPrice_cstr[100];
							sprintf(OpenPrice_cstr, "%0.2f", parkedOrderAction.LimitPrice); //截取为两位精度后转化为字符串类型
							disp_str = OpenPrice_cstr;
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("状态"))
						{
							if (parkedOrderAction.Status == THOST_FTDC_PAOS_NotSend)
							{
								disp_str = _T("未发送");
							}
							if (parkedOrderAction.Status == THOST_FTDC_PAOS_Send)
							{
								disp_str = _T("已发送");
							}
							if (parkedOrderAction.Status == THOST_FTDC_PAOS_Deleted)
							{
								disp_str = _T("已删除");
							}
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("预埋撤单编号"))
						{
							disp_str = parkedOrderAction.ParkedOrderActionID;
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("交易所"))
						{
							disp_str = parkedOrderAction.ExchangeID;
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("报单编号"))
						{
							disp_str = parkedOrderAction.OrderSysID;
						}
						else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("错误信息"))
						{
							disp_str = parkedOrderAction.ErrorMsg;
							if (disp_str == _T(""))
							{
								disp_str = _T("无");
							}
						}
						else
						{
							disp_str = _T("---");
						}
						Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
						Item.strText.Format(_T("%s"), disp_str);
						if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
						{
							m_wndParkedOrderActionList.SetItem(&Item);
						}
						else
						{
							row = m_wndParkedOrderActionList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
							bFined = true;
						}
					}
					m_wndParkedOrderActionList.Refresh();
				}
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspParkedOrderAction)
	{
		{//必须在非交易时段,下预埋撤单才能成功,当下预埋撤单成功后,刷新预埋撤单记录列表
			ParkedOrderAction* pParkedOrderAction = reinterpret_cast<ParkedOrderAction*>(lParam);
			ParkedOrderAction parkedOrderAction = *pParkedOrderAction;
			if (pParkedOrderAction != NULL)
			{
				int row = 0;
				CString Instrument_id(parkedOrderAction.InstrumentID);//获取行头
				CString parkedOrderAction_id(parkedOrderAction.ParkedOrderActionID);//获取传入数据的报单编号
				bool bFined = false;//一开始假设不存在
				row = m_wndParkedOrderActionList.GetRowCount(); //一开始假设不存在,设定插入行Index
				for (int row_index = 1; row_index < m_wndParkedOrderActionList.GetRowCount(); row_index++)
				{
					if (parkedOrderAction_id == m_wndParkedOrderActionList.GetItemText(row_index, m_wndParkedOrderActionList.GetColumnCount() - 1)) //要保证最后一列为预埋报单编号
					{
						bFined = true;
						row = row_index; //获取row index
						break;
					}
				}
				int col = 0;
				for (col = 0; col < m_wndParkedOrderActionList.GetColumnCount(); col++)
				{//每一列对应一个字段
					GV_ITEM Item;
					Item.mask = GVIF_TEXT | GVIF_FORMAT;
					Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
					Item.col = col;//不同列名的所在列号,指定item操作的列
					CString disp_str;
					if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("合约"))
					{
						disp_str = parkedOrderAction.InstrumentID;
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("交易量"))
					{
						disp_str = to_string(parkedOrderAction.VolumeChange).c_str();
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("价格"))
					{
						char OpenPrice_cstr[100];
						sprintf(OpenPrice_cstr, "%0.2f", parkedOrderAction.LimitPrice); //截取为两位精度后转化为字符串类型
						disp_str = OpenPrice_cstr;
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("状态"))
					{
						if (parkedOrderAction.Status == THOST_FTDC_PAOS_NotSend)
						{
							disp_str = _T("未发送");
						}
						if (parkedOrderAction.Status == THOST_FTDC_PAOS_Send)
						{
							disp_str = _T("已发送");
						}
						if (parkedOrderAction.Status == THOST_FTDC_PAOS_Deleted)
						{
							disp_str = _T("已删除");
						}
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("预埋撤单编号"))
					{
						disp_str = parkedOrderAction.ParkedOrderActionID;
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("交易所"))
					{
						disp_str = parkedOrderAction.ExchangeID;
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("报单编号"))
					{
						disp_str = parkedOrderAction.OrderSysID;
					}
					else if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("错误信息"))
					{
						disp_str = parkedOrderAction.ErrorMsg;
						if (disp_str == _T(""))
						{
							disp_str = _T("无");
						}
					}
					else
					{
						disp_str = _T("---");
					}
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
					Item.strText.Format(_T("%s"), disp_str);
					if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
					{
						m_wndParkedOrderActionList.SetItem(&Item);
					}
					else
					{
						row = m_wndParkedOrderActionList.InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
						bFined = true;
					}
				}
				m_wndParkedOrderActionList.Refresh();
			}
		}
		return 1;
	}

	if (wParam == ctp_msg::RspRemoveParkedOrderAction)
	{//删除后只更新状态
		Remove_ParkedOrderAction* pParkedOrderAction = reinterpret_cast<Remove_ParkedOrderAction*>(lParam);
		CString parkedOrderActionID(pParkedOrderAction->ParkedOrderActionID);
		int row = 0;
		for (int row_index = 1; row_index < m_wndParkedOrderActionList.GetRowCount(); row_index++)
		{
			if (parkedOrderActionID == m_wndParkedOrderActionList.GetItemText(row_index, m_wndParkedOrderActionList.GetColumnCount() - 1)) //要保证最后一列为预埋报单编号
			{
				row = row_index; //获取row index
				break;
			}
		}
		CString disp_str;
		int col = 0;
		for (col = 0; col < m_wndParkedOrderActionList.GetColumnCount(); col++)
		{//每一列对应一个字段
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
			Item.col = col;//不同列名的所在列号,指定item操作的列
			if (m_wndParkedOrderActionList.GetItemText(0, col) == _T("状态"))
			{
				disp_str = _T("已删除");
				Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				Item.strText.Format(_T("%s"), disp_str);
				m_wndParkedOrderActionList.SetItem(&Item);
				break;
			}
		}
		m_wndParkedOrderActionList.Refresh();
	}

}


/////////////////////////////////////////////////////////////////////////////
// COutputList1

CTradeSituationWnd::CTradeSituationWnd()
{
}

CTradeSituationWnd::~CTradeSituationWnd()
{
}

BEGIN_MESSAGE_MAP(CTradeSituationWnd, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void CTradeSituationWnd::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void CTradeSituationWnd::OnEditCopy()
{
	MessageBox(_T("复制输出"));
}

void CTradeSituationWnd::OnEditClear()
{
	MessageBox(_T("清除输出"));
}

void CTradeSituationWnd::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}