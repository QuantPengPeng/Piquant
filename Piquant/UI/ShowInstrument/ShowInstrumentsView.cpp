
// ShowInstrumentsView.cpp : implementation of the CShowInstrumentsView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "../App.h"//改入口
#endif

#include "ShowInstrumentsView.h"
#include "../Doc.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/DataTypes.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShowInstrumentsView

IMPLEMENT_DYNCREATE(CShowInstrumentsView, CView)

BEGIN_MESSAGE_MAP(CShowInstrumentsView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CShowInstrumentsView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_UPDATE_GRIDCTRL, &CShowInstrumentsView::OnUpdateGridCtrl)//将消息与消息映射函数相绑定
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CShowInstrumentsView construction/destruction

CShowInstrumentsView::CShowInstrumentsView()
{
	// TODO: add construction code here
	m_pGridCtrl = NULL;

}

CShowInstrumentsView::~CShowInstrumentsView()
{
	if (NULL != m_pGridCtrl)
	{
		delete m_pGridCtrl;//释放指针指向的对象
		m_pGridCtrl = NULL;//重置为空指针，很好的设计习惯
	}
}

BOOL CShowInstrumentsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CShowInstrumentsView drawing

void CShowInstrumentsView::OnDraw(CDC* /*pDC*/)
{
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CShowInstrumentsView printing


void CShowInstrumentsView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CShowInstrumentsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CShowInstrumentsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CShowInstrumentsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CShowInstrumentsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CShowInstrumentsView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CShowInstrumentsView diagnostics

#ifdef _DEBUG
void CShowInstrumentsView::AssertValid() const
{
	CView::AssertValid();
}

void CShowInstrumentsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDoc* CShowInstrumentsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}

void CShowInstrumentsView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CString title = _T("合约信息");
	GetParent()->SetWindowTextW(title); 
	get_CtpMsgDistributor()->AddReceiver(this);//将CShowInstrumentsView类实例添加进接收者容器中
	//从以下开始写个性化代码:
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return;
		// Create the Gridctrl window
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->Create(rect, this, 100);
		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);//设置可编辑
		m_pGridCtrl->EnableDragAndDrop(FALSE);//设置可拖拽

		ConfigReader& cfg_reader = get_config_reader();
		cfg_reader.load("brokers.xml", "config.xml");//从xml文件中加载经纪商信息及列头信息、订阅合约信息等，加载到容器类的成员变量中，方便后续调用
		vector<ColumnInfo> header_colums = cfg_reader.GetHeaderColumn("ListCtrl合约信息");//根据表名,获取表内各列列头结构体ColumnInfo,合计为vector
		if (header_colums.size() <= 0)
		{//做个异常判断处理，可能xml配置文件有误等原因，读取不到信息，此时直接return掉
			return;
		}
		try {
			m_pGridCtrl->SetRowCount(1);//行数
			m_pGridCtrl->SetColumnCount(header_colums.size());//列数
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}
		USES_CONVERSION;
		// 初始化的时候形成列头行
		int row = 0;//选定死第一行，也就是列头行
		for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)//遍历列头行中的每个列字段
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_PARAM;
			Item.row = row;
			Item.col = col;
			Item.lParam = header_colums[col].column_id;
			Item.nFormat = DT_LEFT | DT_WORDBREAK;
			Item.strText.Format(_T("%s"), A2T(header_colums[col].column_name.c_str()));//给各列的列头填充名字
			m_pGridCtrl->SetItem(&Item);
			m_pGridCtrl->SetColumnWidth(col, header_colums[col].column_width);//设置每列的宽度
		}
		//m_pGridCtrl->AutoSize();//不要自动宽度
	}
}

LRESULT CShowInstrumentsView::OnUpdateGridCtrl(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryInstrument)
	{
		vector<BaseData>* instruments_list = reinterpret_cast<vector<BaseData>*>(lParam);
		if (instruments_list != NULL)
		{
			for (int i = 0; i < instruments_list->size(); ++i)
			{
				BaseData base_data = (*instruments_list)[i];
				CString Instrument_id(base_data.code);//获取行头
				// fill rows/cols with text
				int row = 1;
				bool bFined = false;
				for (; row < m_pGridCtrl->GetRowCount(); row++)//从第一行开始去找，因为第0行是列头行
				{
					LPARAM row_data = m_pGridCtrl->GetItemData(row, 0);//比较各行行头与Instrument_id是否一致
					CString row_Instrument_id((TCHAR*)row_data);
					if (row_Instrument_id == Instrument_id)
					{
						bFined = true;
						break;//匹配成功就break出去，说明此时已经存在了该合约信息行
					}
				}
				//每一列对应一个字段，开始遍历该用户的所有信息
				for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
				{
					GV_ITEM Item;
					Item.mask = GVIF_TEXT | GVIF_FORMAT;
					Item.row = row;//通过上一个循环，若找到则row已经锁死,若尚未找到则滚到末尾行的下一行,指定item操作的行
					Item.col = col;//不同列名的所在列号,指定item操作的列
					//Item.lParam = (LPARAM)CString(base_data.code).GetBuffer();
					int field_id = m_pGridCtrl->GetItemData(0, col);//从表头的各列获取field_id,所以为第0行
					CString disp_str(GetTextById(base_data, field_id).c_str());//根据field_id获取要求的值
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
					if (FIELD_Code==field_id)//若此时是刷新行头合约名数据
					{
						m_pGridCtrl->SetItemBkColour(row, col, RGB(255, 154, 53)); //设置背景色
						m_pGridCtrl->SetItemFgColour(row, col, RGB(0, 0, 255)); //设置前景色
					}
				}
				m_pGridCtrl->Refresh();
			}
		}
	}
	return 0;
}

void CShowInstrumentsView::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryInstrument)
	{//此处接收消息，只做视图更新
		PostMessage(WM_UPDATE_GRIDCTRL, wParam, lParam);//WM_UPDATE_GRIDCTRL消息发送后会引发消息处理函数OnUpdateGridCtrl以更新视图界面
	}
}

#endif //_DEBUG


// CShowInstrumentsView message handlers


void CShowInstrumentsView::OnSize(UINT nType, int cx, int cy)
{

	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (m_pGridCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->MoveWindow(rect);
	}
}
