// MultiTraderView.cpp : CMultiTraderView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "../App.h"
#endif

#include "../Doc.h"
#include "CFFEXView.h"
#include "..\GridCtrl\GridCtrl.h"
#include <atlconv.h>//用于字符转换
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../GlobalMgr.h" 
#include <Windows.h> //定时器需要该头文件
#include <time.h>
#include <set>
#include <float.h>
#include "../../ServiceLayer/CtpMsg.h"
#include <map>
#include <utility>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMultiTraderView

IMPLEMENT_DYNCREATE(CCFFEXView, CFormView)

BEGIN_MESSAGE_MAP(CCFFEXView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_MESSAGE(WM_UPDATE_GRIDCTRL,&CCFFEXView::OnUpdateGridCtrl)//将消息与消息映射函数相绑定
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMultiTraderView 构造/析构


CCFFEXView::CCFFEXView()
	: CFormView(CCFFEXView::IDD)
{
	// TODO:  在此处添加构造代码
	m_pGridCtrl = NULL;
}

CCFFEXView::~CCFFEXView()
{
	if (NULL != m_pGridCtrl)
	{
		delete m_pGridCtrl;		//释放指针指向的对象
		m_pGridCtrl = NULL;	//重置为空指针，很好的设计习惯
	}
}

void CCFFEXView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CCFFEXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CFormView::PreCreateWindow(cs);
}

void CCFFEXView::SetGridCtrlColumns(const vector<ColumnInfo>& header_columns)
{
	if (header_columns.size() <= 0)
	{//做个异常判断处理，此时直接return掉
		return;
	}
	//建立表头列
	m_pGridCtrl->DeleteAllItems();
	m_pGridCtrl->SetRowCount(1);//行数
	m_pGridCtrl->SetColumnCount(header_columns.size());//列数
	m_pGridCtrl->SetFixedRowCount(1);
	m_pGridCtrl->SetFixedColumnCount(1);
	USES_CONVERSION;
	int row = 0;//选定死第一行，也就是列头行
	for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)//遍历列头行中的每个列字段
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_PARAM;
		Item.row = row;
		Item.col = col;
		Item.lParam = header_columns[col].column_id;
		Item.nFormat = DT_LEFT | DT_WORDBREAK;
		Item.strText.Format(_T("%s"), A2T(header_columns[col].column_name.c_str()));//给各列的列头填充名字
		m_pGridCtrl->SetItem(&Item);
		m_pGridCtrl->SetColumnWidth(col, header_columns[col].column_width);//设置每列的宽度
	}
	//m_pGridCtrl->AutoSize();//不要自动宽度
}

void CCFFEXView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CString title = _T("中金");
	GetParent()->SetWindowTextW(title);
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	get_CtpMsgDistributor()->AddReceiver(this);
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CMarketDataGridCtrl();
		if (!m_pGridCtrl) return;
		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->Create(rect, this, 100);
		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);//设置不可编辑
		m_pGridCtrl->EnableDragAndDrop(FALSE);//设置不可拖拽
	}
	ConfigReader& cfg_reader = get_config_reader();
	cfg_reader.load("brokers.xml", "config.xml");//从xml文件中加载经纪商信息及列头信息、订阅合约信息等，加载到容器类的成员变量中，方便后续调用
	vector<ColumnInfo> header_colums = cfg_reader.GetHeaderColumn("ListCtrl报价表");
	SetGridCtrlColumns(header_colums);   
}

void CCFFEXView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCFFEXView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


//诊断

#ifdef _DEBUG
void CCFFEXView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCFFEXView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDoc* CCFFEXView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG


//消息处理程序
void CCFFEXView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if (m_pGridCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->MoveWindow(rect);
	}
}

BOOL CCFFEXView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pGridCtrl && IsWindow(m_pGridCtrl->m_hWnd))
		if (m_pGridCtrl->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CCFFEXView::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryInstrument)
	{
		PostMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);//初始化行情界面
	}
}

LRESULT CCFFEXView::OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam)//更新列表用
{
	//CFFEX
	if (wParam == RspQryInstrument)
	{//接受上海及能源交易所的合约数据，并做重排序
		vector<BaseData>* instruments_list = reinterpret_cast<vector<BaseData>*>(lParam);
		vector<BaseData> shfe_ine_instrumentlist;
		vector<BaseData> shfe_ine_resort_instrumentlist; //重排聚合好的
		set<string> productID_set;
		if (instruments_list != NULL)
		{
			for (int i = 0; i < instruments_list->size(); ++i)
			{//只接受上海及能源交易所的合约数据
				BaseData base_data = (*instruments_list)[i];
				CString Exchange_id(base_data.ExchangeID);//获取交易所
				if (Exchange_id == _T("CFFEX"))
				{
					shfe_ine_instrumentlist.push_back(base_data);
				}
			}
			for (int j = 0;  j < shfe_ine_instrumentlist.size(); ++j)
			{//获取产品类型集合
				productID_set.insert(shfe_ine_instrumentlist[j].ProductID);
			}
			string productID;
			vector<BaseData> instrument_buffer;//合约读写缓冲区，其缓存的是一个品种类型的合约集合
			for (set<string>::iterator it = productID_set.begin(); it != productID_set.end(); ++it)
			{
				productID = *it;
				if (instrument_buffer.size() != 0)
				{//先清空
					instrument_buffer.clear();
				}
				for (vector<BaseData>::iterator it = shfe_ine_instrumentlist.begin(); it != shfe_ine_instrumentlist.end(); ++it)
				{//重排聚合
					string productID_Instrument = "";
					for (int strindex = 0; strindex < strlen((*it).code); ++strindex)
					{
						char a = ((*it).code)[strindex];
						if (isalpha(((*it).code)[strindex]))//是否是字母
						{
							productID_Instrument += ((*it).code)[strindex];
						}
					}
					if (strcmp(productID_Instrument.c_str(), productID.c_str()) == 0) //产品类型相同
					{
						instrument_buffer.push_back(*it); //暂时先压入缓冲区
					}
				}
				int Min_DeliveryMonth=13;//12月后加1，这里象征月份中无法达到的最大值
				int Min_DeliveryYear = 9999;
				BaseData instrument_timesort;
				int resort_num = instrument_buffer.size();
				for (int k = 0; k < resort_num; ++k)
				{//有多少个元素就必须比较排序多少次
					Min_DeliveryMonth = 13;//重置
					Min_DeliveryYear = 9999;
					for (vector<BaseData>::iterator it = instrument_buffer.begin(); it != instrument_buffer.end(); ++it)
					{//按年份及月份重排
						if (it->DeliveryYear < Min_DeliveryYear)
						{
							Min_DeliveryYear = it->DeliveryYear;
							Min_DeliveryMonth = it->DeliveryMonth;
							instrument_timesort = *it;
						}
						else if (it->DeliveryYear == Min_DeliveryYear)
						{
							if (it->DeliveryMonth < Min_DeliveryMonth)
							{
								Min_DeliveryYear = it->DeliveryYear;
								Min_DeliveryMonth = it->DeliveryMonth;
								instrument_timesort = *it;
							}
						}
						else
						{//大于的，直接continue
							continue;
						}
					}
					shfe_ine_resort_instrumentlist.push_back(instrument_timesort);
					for (vector<BaseData>::iterator it = instrument_buffer.begin(); it != instrument_buffer.end(); )
					{//将已经压入shfe_ine_resort_instrumentlist中，已无需比较的合约数据从缓存区中删除
						if (strcmp(it->code, instrument_timesort.code) == 0)
						{
							it = instrument_buffer.erase(it);
						}
						else
						{
							it++;
						}
					}
				}
			}
		}

		if (shfe_ine_resort_instrumentlist.size()!=0)
		{
			for (int i = 0; i < shfe_ine_resort_instrumentlist.size(); ++i)
			{
				BaseData base_data = shfe_ine_resort_instrumentlist[i];
				CString Instrument_id(base_data.code);//获取行头即合约ID
				int row = m_pGridCtrl->GetRowCount();//默认表尾后插入数据
				bool bFined = false;
				for (; row <= m_pGridCtrl->GetRowCount(); row++)//从第一行开始去找，因为第0行是列头行
				{
					CString row_data = m_pGridCtrl->GetItemText(row, 0);//比较各行行头与Instrument_id是否一致
					CString row_Instrument_id(row_data);
					if (row_Instrument_id == Instrument_id)
					{
						bFined = true;
						break;//匹配成功就break出去，说明此时已经存在了该合约信息行
					}
				}
				//遍历字段
				CString disp_str;
				for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
				{
					GV_ITEM Item;
					Item.mask = GVIF_TEXT | GVIF_FORMAT;
					Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行
					Item.col = col;//不同列名的所在列号,指定item操作的列
					//Item.lParam = (LPARAM)CString(base_data.code).GetBuffer();

					if (m_pGridCtrl->GetItemText(0, col)==_T("合约"))
					{
						disp_str = base_data.code;
					}
					else if (m_pGridCtrl->GetItemText(0, col) == _T("合约名"))
					{
						disp_str = base_data.InstrumentName;
					}
					else if (m_pGridCtrl->GetItemText(0, col) == _T("交易所"))
					{
						disp_str = base_data.ExchangeID;
					}
					else
					{//其它字段不在这里赋值处理，直接continue
						continue;
					}
					Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
					Item.strText.Format(_T("%s"), disp_str);
					if (bFined)//若从表中可以找到已存有该合约的信息，则只做刷新
					{
						m_pGridCtrl->SetItem(&Item);
					}
					else
					{
						row = m_pGridCtrl->InsertRow(Instrument_id);//插入一行,第一列的数据就是合约名
						bFined = true;
					}
				}
				m_pGridCtrl->Refresh();
			}
		}
		
	}
	return 0;
}

void CCFFEXView::OnDraw(CDC* pDC)
{
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
	{
		return;
	}
}


void CCFFEX_ChildView::UpdateMarketDataView(LPARAM lParam)
{
	MarketData* pMarketData = reinterpret_cast<MarketData*>(lParam);
	MarketData marketData = *pMarketData;
	CString marketData_instrumentID(marketData.code);
	int matchrow = -1;//记录匹配的行号
	for (int row = 1; row <= m_pGridCtrl->GetRowCount(); row++)//从第一行开始去找，因为第0行是列头行
	{
		CString row_data = m_pGridCtrl->GetItemText(row, 0);//比较各行行头与Instrument_id是否一致
		CString row_Instrument_id(row_data);
		if (row_Instrument_id == marketData_instrumentID)
		{
			matchrow = row;
			break;//匹配成功就break出去，说明此时已经存在了该合约信息行
		}
	}
	if (matchrow == -1) //出现未知的某种异常,居然无法匹配成功
	{
		return;
	}
	CString disp_str;
	for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = matchrow;
		Item.col = col;
		if (m_pGridCtrl->GetItemText(0, col) == _T("最新价"))
		{
			if (marketData.LastPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char lastprice_cstr[100];
				sprintf(lastprice_cstr, "%0.2f", marketData.LastPrice);
				disp_str = lastprice_cstr;
				m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(255, 152, 49));//橙底黑字
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("卖一价"))
		{
			if (marketData.AskPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char askprice_cstr[100];
				sprintf(askprice_cstr, "%0.2f", marketData.AskPrice);
				disp_str = askprice_cstr;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("卖一量"))
		{
			char askvolume_cstr[100];
			sprintf(askvolume_cstr, "%d", marketData.AskVolume);
			disp_str = askvolume_cstr;
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("买一价"))
		{
			if (marketData.BidPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char bidprice_cstr[100];
				sprintf(bidprice_cstr, "%0.2f", marketData.BidPrice);
				disp_str = bidprice_cstr;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("买一量"))
		{
			char bidvolume_cstr[100];
			sprintf(bidvolume_cstr, "%d", marketData.BidVolume);
			disp_str = bidvolume_cstr;
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("涨停价"))
		{
			if (marketData.UpperLimitPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char UpperLimitPrice_cstr[100];
				sprintf(UpperLimitPrice_cstr, "%0.2f", marketData.UpperLimitPrice);
				disp_str = UpperLimitPrice_cstr;
				m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(255, 255, 0));
				m_pGridCtrl->SetItemFgColour(matchrow, col, RGB(255, 0, 0));
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("跌停价"))
		{
			if (marketData.LowerLimitPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char LowerLimitPrice_cstr[100];
				sprintf(LowerLimitPrice_cstr, "%0.2f", marketData.LowerLimitPrice);
				disp_str = LowerLimitPrice_cstr;
				m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(255, 255, 0));
				m_pGridCtrl->SetItemFgColour(matchrow, col, RGB(0, 128, 128));
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("成交量"))
		{
			char Volume[100];
			sprintf(Volume, "%d", marketData.Volume);
			disp_str = Volume;
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("昨收盘"))
		{
			if (marketData.PreClosePrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char PreClosePrice[100];
				sprintf(PreClosePrice, "%0.2f", marketData.PreClosePrice);
				disp_str = PreClosePrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("今开盘"))
		{
			if (marketData.OpenPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char OpenPrice[100];
				sprintf(OpenPrice, "%0.2f", marketData.OpenPrice);
				disp_str = OpenPrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("最高价"))
		{
			if (marketData.HighestPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char HighestPrice[100];
				sprintf(HighestPrice, "%0.2f", marketData.HighestPrice);
				disp_str = HighestPrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("最低价"))
		{
			if (marketData.LowestPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char LowestPrice[100];
				sprintf(LowestPrice, "%0.2f", marketData.LowestPrice);
				disp_str = LowestPrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("今收盘"))
		{//行情通知中，例如结算价、收盘价、买卖价出现double极大值，则表示该字段没有值。例如涨停板的时候，因为没有卖价，会给出一个double极大值，同时卖量为0。
			if (marketData.ClosePrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char ClosePrice[100];
				sprintf(ClosePrice, "%0.2f", marketData.ClosePrice);
				disp_str = ClosePrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("昨结算"))
		{
			if (marketData.PreSettlementPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char PreSettlementPrice[100];
				sprintf(PreSettlementPrice, "%0.2f", marketData.PreSettlementPrice);
				disp_str = PreSettlementPrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("结算价"))
		{//行情通知中，例如结算价、收盘价、买卖价出现double极大值，则表示该字段没有值。例如涨停板的时候，因为没有卖价，会给出一个double极大值，同时卖量为0。
			if (marketData.SettlementPrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				char SettlementPrice[100];
				sprintf(SettlementPrice, "%0.2f", marketData.SettlementPrice);
				disp_str = SettlementPrice;
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("行情更新时间"))
		{
			disp_str = marketData.UpdateTime;
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("涨跌"))
		{
			if (marketData.LastPrice == DBL_MAX || marketData.PreClosePrice==DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				double range = marketData.LastPrice - marketData.PreClosePrice;
				char range_cstr[100];
				sprintf(range_cstr, "%0.2f", range);
				disp_str = range_cstr;
				if (range >= 0)
				{
					m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(255, 0, 0));
				}
				else
				{
					m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(0, 255, 0));
				}
			}
		}
		else if (m_pGridCtrl->GetItemText(0, col) == _T("涨跌幅"))
		{
			if (marketData.LastPrice == DBL_MAX || marketData.PreClosePrice == DBL_MAX)
			{
				disp_str = _T("无");
			}
			else
			{
				double range = 0;
				if (marketData.PreClosePrice != 0)
				{
					range = (marketData.LastPrice - marketData.PreClosePrice) / marketData.PreClosePrice * 100;
					char range_cstr[100];
					sprintf(range_cstr, "%0.2f%%", range);
					disp_str = range_cstr;
				}
				else
				{
					disp_str = _T("---");
				}
				if (range >= 0)
				{
					m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(255, 0, 0));
				}
				else
				{
					m_pGridCtrl->SetItemBkColour(matchrow, col, RGB(0, 255, 0));
				}
			}
		}
		else
		{//其它字段不在这里赋值处理，直接continue
			continue;
		}
		Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
		Item.strText.Format(_T("%s"), disp_str);
		m_pGridCtrl->SetItem(&Item);
	}
	m_pGridCtrl->Refresh();
}


void CCFFEXView::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	__super::OnClose();
}