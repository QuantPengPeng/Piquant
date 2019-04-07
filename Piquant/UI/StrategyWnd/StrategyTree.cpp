
#include "stdafx.h"
#include "../mainfrm.h"
#include "StrategyTree.h"
#include "../Resource.h"
#include "../App.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_DUMMY_COMPILE, StartStrategy)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_REFRESH, &CFileView::OnRefresh)
	ON_COMMAND(ID_OPENFILE, &CFileView::OnOpenfile)
	ON_COMMAND(ID_STOPSTRATEGY, &CFileView::OnStopstrategy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建策略树\n");
		return -1;      // 未能创建
	}

	// 加载视图图像: 
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::FillFileView()
{//扫描目录加载dll;
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("策略接口"), 0, 0);	//策略根结点
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	CFileFind file_find;
	BOOL bFind = file_find.FindFile(_T("*.dll"));  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		CString DataFile = file_find.GetFilePath(); //GetFilePath：得到全路径名;
		HTREEITEM hSrc = hRoot;
		if (file_find.IsDirectory() && !file_find.IsDots())    //如果是目录 
		{
			hSrc = m_wndFileView.InsertItem(file_find.GetFileName(), 0, 0, hRoot);
		}
		else if (!file_find.IsDirectory() && !file_find.IsDots())//不为目录
		{
			CString FileTitle = file_find.GetFileTitle();//GetFileTitle:得到不带后缀文件名
			CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
			HMODULE hModule = LoadLibrary(file_find.GetFilePath());
			if (hModule == NULL)
			{
				//认为该文件不是一个dll;
			}
			else
			{
				FARPROC proc = GetProcAddress(hModule, "CreateStrategy");	//检查是否有CreateStrategy函数可导出，唯有可导出的dll才是策略文件
				pfnCreateStrategy pfnCreator = (pfnCreateStrategy)GetProcAddress(hModule, "CreateStrategy");
				if (NULL != pfnCreator)
				{
					HTREEITEM hInc = m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
					m_wndFileView.SetItemData(hInc, (DWORD_PTR)hModule);
				}
			}
			//........
		}
	}
	file_find.Close();

	HTREEITEM hRoot_Log = m_wndFileView.InsertItem(_T("日志"), 0, 0);	//日志结点
	m_wndFileView.SetItemState(hRoot_Log, TVIS_BOLD, TVIS_BOLD);
	bFind = file_find.FindFile(_T("log/*.txt"));
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
		HTREEITEM hSrc = hRoot_Log;
		m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
	}
	file_find.Close();

	HTREEITEM hRoot_Python = m_wndFileView.InsertItem(_T("脚本"), 0, 0);	//脚本结点
	m_wndFileView.SetItemState(hRoot_Python, TVIS_BOLD, TVIS_BOLD);
	bFind = file_find.FindFile(_T("script/*.py"));
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
		HTREEITEM hSrc = hRoot_Python;
		m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
	}
	file_find.Close();

	HTREEITEM hRoot_Exe = m_wndFileView.InsertItem(_T("exe快捷方式"), 0, 0);	//其它结点
	m_wndFileView.SetItemState(hRoot_Exe, TVIS_BOLD, TVIS_BOLD);
	bFind = file_find.FindFile(_T("lnk/*.lnk"));
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
		HTREEITEM hSrc = hRoot_Exe;
		m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
	}
	file_find.Close();

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hRoot_Log, TVE_EXPAND);
	m_wndFileView.Expand(hRoot_Python, TVE_EXPAND);
	m_wndFileView.Expand(hRoot_Exe, TVE_EXPAND);
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项: 
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CFileView::StartStrategy()
{//添加事件响应
	// TODO:  在此处添加命令处理程序代码
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString nodetext = m_wndFileView.GetItemText(hItem);
	int length = nodetext.Find(_T(".")); //分割符
	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("txt") || nodetext.Right(nodetext.GetLength() - length - 1) == _T("py") || nodetext.Right(nodetext.GetLength() - length - 1) == _T("lnk"))
	{
		MessageBox(_T("非策略"), _T("错误"), MB_OK);
		return;
	}
	HMODULE hStrategyModule = (HMODULE)m_wndFileView.GetItemData(hItem);
	//GetProcAddress
	//pfn()
	pfnCreateStrategy pfnCreator = (pfnCreateStrategy)GetProcAddress(hStrategyModule, "CreateStrategy"); //pfnCreateStratey为函数指针,获取导出函数
	if (pfnCreator)
	{
		IStrategy* pIStrategy = reinterpret_cast<IStrategy*>(pfnCreator()); //pfnCreator()此函数执行后得到策略框架类
		if (NULL == pIStrategy)
		{
			//AfxMessageBox(_T("策略创建失败...."));
			MessageBox(_T("策略创建失败"), _T("错误"), MB_OK);
		}
		else
		{
			if (pIStrategy->isRunning()==true)
			{//已经正在运行了,拒绝再次申请运行
				MessageBox(_T("策略运行中"), _T("错误"), MB_OK);
			}
			else
			{
				//AfxMessageBox(_T("策略创建成功...."));
				pIStrategy->set_ApiInterface(get_global_mgr().GetService());	//默认对接ctp,若以后对接IB等其它接口，请改
				pIStrategy->start();	//启动策略运行
			}
		}
	}
}

void CFileView::OnStopstrategy()
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString nodetext = m_wndFileView.GetItemText(hItem);
	int length = nodetext.Find(_T(".")); //分割符
	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("txt") || nodetext.Right(nodetext.GetLength() - length - 1) == _T("py") || nodetext.Right(nodetext.GetLength() - length - 1) == _T("lnk"))
	{
		MessageBox(_T("非策略"), _T("错误"), MB_OK);
		return;
	}
	HMODULE hStrategyModule = (HMODULE)m_wndFileView.GetItemData(hItem);
	pfnCreateStrategy pfnCreator = (pfnCreateStrategy)GetProcAddress(hStrategyModule, "CreateStrategy"); //pfnCreateStratey为函数指针,获取导出函数
	if (pfnCreator)
	{
		IStrategy* pIStrategy = reinterpret_cast<IStrategy*>(pfnCreator()); //pfnCreator()此函数执行后得到策略框架类
		if (NULL == pIStrategy)
		{
			//AfxMessageBox(_T("策略停止失败...."));
			MessageBox(_T("策略停止失败"), _T("错误"), MB_OK);
		}
		else
		{
			//AfxMessageBox(_T("策略停止成功...."));
			if (pIStrategy->isRunning()==false)
			{//已经停止运行了,拒绝再次申请停止运行
				MessageBox(_T("策略尚未创建/策略已经停止"), _T("错误"), MB_OK);
			}
			else
			{
				pIStrategy->stop();	//停止策略运行,注意只是暂停运行
			}
		}
	}
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图:  %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

BOOL CFileView::CanBeClosed() const
{
	return FALSE;
}

BOOL CFileView::FloatPane(CRect rectFloat, AFX_DOCK_METHOD dockMethod /*= DM_UNKNOWN*/, bool bShow /*= true*/)
{
	return true;
}



void CFileView::OnRefresh()
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString nodetext = m_wndFileView.GetItemText(hItem);
	int length = nodetext.Find(_T(".")); //分割符
	//根据不同根节点类型做刷新
	bool bFind;
	CFileFind file_find;

	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("txt"))
	{
		HTREEITEM hSrc = m_wndFileView.GetParentItem(hItem); //假设设计上只存在二级结点结构，此处用于获取一级结点句柄
		HTREEITEM hCurItem = m_wndFileView.GetChildItem(hSrc); //从一级结点开始，获取二级子结点
		HTREEITEM hPreItem;
		while (hCurItem)
		{
			hPreItem = hCurItem; //记录为上一结点
			hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
			m_wndFileView.DeleteItem(hPreItem);
		}
		bFind = file_find.FindFile(_T("log/*.txt"));
		while (bFind)
		{
			bFind = file_find.FindNextFile();
			CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
			//刷新前先清空所有子结点
			m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
		}
		file_find.Close();
	}
	else if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("py"))
	{
		//刷新前先行清空所有二级结点
		HTREEITEM hSrc = m_wndFileView.GetParentItem(hItem); //假设设计上只存在二级结点结构，此处用于获取一级结点句柄
		HTREEITEM hCurItem = m_wndFileView.GetChildItem(hSrc); //从一级结点开始，获取二级子结点
		HTREEITEM hPreItem;
		while (hCurItem)
		{
			hPreItem = hCurItem; //记录为上一结点
			hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
			m_wndFileView.DeleteItem(hPreItem);
		}
		bFind = file_find.FindFile(_T("script/*.py"));
		while (bFind)
		{
			bFind = file_find.FindNextFile();
			CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
			m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
		}
		file_find.Close();
	}
	else if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("lnk"))
	{
		//刷新前先行清空所有二级结点
		HTREEITEM hSrc = m_wndFileView.GetParentItem(hItem); //假设设计上只存在二级结点结构，此处用于获取一级结点句柄
		HTREEITEM hCurItem = m_wndFileView.GetChildItem(hSrc); //从一级结点开始，获取二级子结点
		HTREEITEM hPreItem;
		while (hCurItem)
		{
			hPreItem = hCurItem; //记录为上一结点
			hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
			m_wndFileView.DeleteItem(hPreItem);
		}
		bFind = file_find.FindFile(_T("lnk/*.lnk"));
		while (bFind)
		{
			bFind = file_find.FindNextFile();
			CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
			m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
		}
		file_find.Close();
	}
	else if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("dll"))
	{
		//刷新前先行清空所有二级结点
		HTREEITEM hSrc = m_wndFileView.GetParentItem(hItem); //假设设计上只存在二级结点结构，此处用于获取一级结点句柄
		HTREEITEM hCurItem = m_wndFileView.GetChildItem(hSrc); //从一级结点开始，获取二级子结点
		HTREEITEM hPreItem;
		while (hCurItem)
		{
			hPreItem = hCurItem; //记录为上一结点
			hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
			m_wndFileView.DeleteItem(hPreItem);
		}
		BOOL bFind = file_find.FindFile(_T("*.dll"));  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
		while (bFind)
		{
			bFind = file_find.FindNextFile();
			if (file_find.IsDirectory() && !file_find.IsDots())    //如果是目录 
			{
				continue;
			}
			else if (!file_find.IsDirectory() && !file_find.IsDots())//不为目录
			{
				CString FileTitle = file_find.GetFileTitle();//GetFileTitle:得到不带后缀文件名
				CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
				HMODULE hModule = LoadLibrary(file_find.GetFilePath());
				if (hModule == NULL)
				{
					//认为该文件不是一个dll;
					continue;
				}
				else
				{
					FARPROC proc = GetProcAddress(hModule, "CreateStrategy");	//检查是否有CreateStrategy函数可导出，唯有可导出的dll才是策略文件
					pfnCreateStrategy pfnCreator = (pfnCreateStrategy)GetProcAddress(hModule, "CreateStrategy");
					if (NULL != pfnCreator)
					{
						HTREEITEM hInc = m_wndFileView.InsertItem(FileName, 1, 1, hSrc);
						m_wndFileView.SetItemData(hInc, (DWORD_PTR)hModule);
					}
				}
			}
		}
		file_find.Close();
	}
	else//若不是选定二级结点下的刷新，则是选定一级结点下的刷新
	{
		if (nodetext == _T("策略接口"))
		{
			//刷新前先行清空所有二级结点
			HTREEITEM hCurItem = m_wndFileView.GetChildItem(hItem); //从一级结点开始，获取二级子结点
			HTREEITEM hPreItem;
			while (hCurItem)
			{
				hPreItem = hCurItem; //记录为上一结点
				hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
				m_wndFileView.DeleteItem(hPreItem);
			}
			BOOL bFind = file_find.FindFile(_T("*.dll"));  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
			while (bFind)
			{
				bFind = file_find.FindNextFile();
				if (file_find.IsDirectory() && !file_find.IsDots())    //如果是目录 
				{
					continue;
				}
				else if (!file_find.IsDirectory() && !file_find.IsDots())//不为目录
				{
					CString FileTitle = file_find.GetFileTitle();//GetFileTitle:得到不带后缀文件名
					CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
					HMODULE hModule = LoadLibrary(file_find.GetFilePath());
					if (hModule == NULL)
					{
						//认为该文件不是一个dll;
						continue;
					}
					else
					{
						FARPROC proc = GetProcAddress(hModule, "CreateStrategy");	//检查是否有CreateStrategy函数可导出，唯有可导出的dll才是策略文件
						pfnCreateStrategy pfnCreator = (pfnCreateStrategy)GetProcAddress(hModule, "CreateStrategy");
						if (NULL != pfnCreator)
						{
							HTREEITEM hInc = m_wndFileView.InsertItem(FileName, 1, 1, hItem);
							m_wndFileView.SetItemData(hInc, (DWORD_PTR)hModule);
						}
					}
				}
			}
			file_find.Close();
		}
		else if (nodetext == _T("日志"))
		{
			HTREEITEM hCurItem = m_wndFileView.GetChildItem(hItem); //从一级结点开始，获取二级子结点
			HTREEITEM hPreItem;
			while (hCurItem)
			{
				hPreItem = hCurItem; //记录为上一结点
				hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
				m_wndFileView.DeleteItem(hPreItem);
			}
			bFind = file_find.FindFile(_T("log/*.txt"));
			while (bFind)
			{
				bFind = file_find.FindNextFile();
				CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
				//刷新前先清空所有子结点
				m_wndFileView.InsertItem(FileName, 1, 1, hItem);
			}
			file_find.Close();
		}
		else if (nodetext == _T("脚本"))
		{
			//刷新前先行清空所有二级结点
			HTREEITEM hCurItem = m_wndFileView.GetChildItem(hItem); //从一级结点开始，获取二级子结点
			HTREEITEM hPreItem;
			while (hCurItem)
			{
				hPreItem = hCurItem; //记录为上一结点
				hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
				m_wndFileView.DeleteItem(hPreItem);
			}
			bFind = file_find.FindFile(_T("script/*.py"));
			while (bFind)
			{
				bFind = file_find.FindNextFile();
				CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
				m_wndFileView.InsertItem(FileName, 1, 1, hItem);
			}
			file_find.Close();
		}
		else if (nodetext == _T("exe快捷方式"))
		{
			//刷新前先行清空所有二级结点
			HTREEITEM hCurItem = m_wndFileView.GetChildItem(hItem); //从一级结点开始，获取二级子结点
			HTREEITEM hPreItem;
			while (hCurItem)
			{
				hPreItem = hCurItem; //记录为上一结点
				hCurItem = m_wndFileView.GetNextSiblingItem(hCurItem);	//找寻下一兄弟结点为当前结点
				m_wndFileView.DeleteItem(hPreItem);
			}
			bFind = file_find.FindFile(_T("lnk/*.lnk"));
			while (bFind)
			{
				bFind = file_find.FindNextFile();
				CString FileName = file_find.GetFileName(); //GetFileName:得到带后缀的文件名
				m_wndFileView.InsertItem(FileName, 1, 1, hItem);
			}
			file_find.Close();
		}
		else
		{
			//其它类型处理
		}
	}
}


void CFileView::OnOpenfile()
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString nodetext = m_wndFileView.GetItemText(hItem);
	int length = nodetext.Find(_T(".")); //分割符
	bool bFind;
	CFileFind file_find;
	CString FilePath;

	//处理不同文件类型的打开
	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("txt"))
	{
		CString fileName = _T("log//") + nodetext;
		BOOL bFind = file_find.FindFile(fileName);  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
		if (bFind)
		{
			bFind = file_find.FindNextFileW();	//这句话很重要,不能少
			FilePath = file_find.GetRoot(); //获取根路径
		}
		int ret = (int)ShellExecute(NULL, _T("edit"), m_wndFileView.GetItemText(hItem), NULL, FilePath, SW_SHOWNORMAL);
		if (ret < 32)//检测是否打开成功
			MessageBox(_T("打开出错"), _T("错误"), MB_OK);
	}

	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("dll"))
	{
		MessageBox(_T("dll策略文件无法打开"), _T("错误"), MB_OK);
	}

	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("py"))
	{
		CString fileName = _T("script//") + nodetext;
		BOOL bFind = file_find.FindFile(fileName);  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
		if (bFind)
		{
			bFind = file_find.FindNextFileW();	//这句话很重要,不能少
			FilePath = file_find.GetRoot(); //获取根路径
		}
		int ret = (int)ShellExecute(NULL, _T("open"), m_wndFileView.GetItemText(hItem), NULL, FilePath, SW_SHOWNORMAL);
		if (ret < 32)//检测是否打开成功
			MessageBox(_T("打开出错"), _T("错误"), MB_OK);
	}

	if (nodetext.Right(nodetext.GetLength() - length - 1) == _T("lnk"))
	{
		CString fileName = _T("lnk//") + nodetext;
		BOOL bFind = file_find.FindFile(fileName);  //file_find默认是在工程输出目录去查找dll的，也即是C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug
		if (bFind)
		{
			bFind = file_find.FindNextFileW();	//这句话很重要,不能少
			FilePath = file_find.GetRoot(); //获取根路径
		}
		int ret = (int)ShellExecute(NULL, _T("open"), m_wndFileView.GetItemText(hItem), NULL, FilePath, SW_SHOWNORMAL);
		if (ret < 32)//检测是否打开成功
			MessageBox(_T("打开出错"), _T("错误"), MB_OK);
	}
}


