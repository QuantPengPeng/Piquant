
// Doc.cpp : CDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "App.h"
#endif

#include "Doc.h"
#include "MarketDataView/CFFEXView.h"
#include "MarketDataView/SHFE_INEView.h"
#include "MarketDataView/DCEView.h"
#include "MarketDataView/CZCEView.h"
#include "../ServiceLayer/CtpMsg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMultiTraderDoc

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
END_MESSAGE_MAP()


// CMultiTraderDoc 构造/析构

CDoc::CDoc()
{
	// TODO:  在此添加一次性构造代码
	get_CtpMsgDistributor()->AddReceiver(this);
	hMonitorThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorThreadFunc,this, 0, &MonitorThreadID); //在构造函数中创建监听线程, 用以不断监听数据队列
}

CDoc::~CDoc()
{
	get_CtpMsgDistributor()->DelReceiver(this);
}

BOOL CDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	SetTitle(L"Piquant"); //修改mfc软件的exe名字
	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	CApp*p_app = (CApp*)AfxGetApp();
	CreateNewWindow(p_app->pDocTemplate_ShowInstrument, this);
	CreateNewWindow(p_app->pDocTemplate_CZCEView, this);
	CreateNewWindow(p_app->pDocTemplate_DCEView, this);
	CreateNewWindow(p_app->pDocTemplate_SHFEINEView, this);
	return TRUE;
}




// CMultiTraderDoc 序列化

void CDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMultiTraderDoc 诊断

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

CFrameWnd* CDoc::CreateNewWindow(CDocTemplate* pTemplate, CDocument* pDocument)
{
	ASSERT_VALID(pTemplate);
	ASSERT_VALID(pDocument);
	CFrameWnd* pFrame =pTemplate->CreateNewFrame(pDocument, NULL);
	CRect rect;
	GetClientRect(pFrame->m_hWnd, &rect);
	pFrame->MoveWindow(100, 150, rect.Width(), rect.Height());
	if (pFrame == NULL)
	{
		TRACE0("Warning: failed to create new frame.\n");
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);
	pTemplate->InitialUpdateFrame(pFrame, pDocument);
	return pFrame;
}

void CDoc::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam==ctp_msg::RtnMarketData)
	{
		MarketData* pMarketData = reinterpret_cast<MarketData*>(lParam);
		MarketData marketData = *pMarketData;
		DataQueue.Push(marketData);//推送数据进安全的数据队列
	}
}



//全局的线程监听函数
void MonitorThreadFunc(CDoc* pDoc)
{
	vector<BaseData> instruments;
	if (NULL != get_global_mgr().GetService())
	{
		while (instruments.size()==0)
		{//若提取到的合约信息集为空,则等待后持续获取到可得到为止
			Sleep(2000);
			get_global_mgr().GetService()->get_basic_data(instruments);
		}
	}
	else
	{//连全局管理器都获取不到,直接return
		return;
	}
	static MarketData marketData;
	string marketData_instrumentID;

	while (TRUE)
	{
		memset(&marketData, 0, sizeof(marketData));//重置
		if (pDoc->DataQueue.Pop(marketData) == TRUE)
		{//提出数据成功
			marketData_instrumentID = marketData.code;
			for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
			{
				if (strcmp(marketData_instrumentID.c_str(), it->code) == 0)
				{//匹配合约ID
					//因为CTP传回的深度行情数据ExchangeID为空, 此处我们主动填充
					strcpy_s(marketData.ExchangeID, it->ExchangeID);
				}
			}
			if (strcmp(marketData.ExchangeID, "CFFEX") == 0)
			{//中金所数据传给中金所的行情view
				POSITION pos = pDoc->GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = pDoc->GetNextView(pos);
					if (pView->IsKindOf(RUNTIME_CLASS(CCFFEXView)))
					{
						CCFFEX_ChildView *CffexView = (CCFFEX_ChildView*)pView;
						if (CffexView->m_pGridCtrl->GetRowCount() != 0)
						{//必须已经初始化行情表框后才传递深度行情数据刷新
							CffexView->UpdateMarketDataView((LPARAM)&marketData);
						}
					}
				}
			}
			else if (strcmp(marketData.ExchangeID, "SHFE") == 0 || strcmp(marketData.ExchangeID, "INE") == 0)
			{
				POSITION pos = pDoc->GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = pDoc->GetNextView(pos);
					if (pView->IsKindOf(RUNTIME_CLASS(CSHFE_INEView)))
					{
						CSHFE_INE_ChildView *CShfe_Ine_View = (CSHFE_INE_ChildView*)pView;
						if (CShfe_Ine_View->m_pGridCtrl->GetRowCount() != 0)
						{//必须已经初始化行情表框后才传递深度行情数据刷新
							CShfe_Ine_View->UpdateMarketDataView((LPARAM)&marketData);
						}
					}
				}
			}
			else if (strcmp(marketData.ExchangeID, "DCE") == 0)
			{
				POSITION pos = pDoc->GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = pDoc->GetNextView(pos);
					if (pView->IsKindOf(RUNTIME_CLASS(CDCEView)))
					{
						CDCE_ChildView *CDce_View = (CDCE_ChildView*)pView;
						if (CDce_View->m_pGridCtrl->GetRowCount() != 0)
						{//必须已经初始化行情表框后才传递深度行情数据刷新
							CDce_View->UpdateMarketDataView((LPARAM)&marketData);
						}
					}
				}
			}
			else if (strcmp(marketData.ExchangeID, "CZCE") == 0)
			{
				POSITION pos = pDoc->GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = pDoc->GetNextView(pos);
					if (pView->IsKindOf(RUNTIME_CLASS(CCZCEView)))
					{
						CCZCE_ChildView *CCZCE_View = (CCZCE_ChildView*)pView;
						if (CCZCE_View->m_pGridCtrl->GetRowCount() != 0)
						{//必须已经初始化行情表框后才传递深度行情数据刷新
							CCZCE_View->UpdateMarketDataView((LPARAM)&marketData);
						}
					}
				}
			}
			else
			{
				//未知异常,暂不做处理
			}
		}
	}
}

#endif //_DEBUG


// CDoc 命令
