
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "App.h"
#include "MainFrm.h"
#include "Login/LoginDlg.h"
#include "Qry/QryCommissionRate.h"
#include "Qry/QryInstrumentMarginRateDlg.h"
#include "Qry/QryExchangeRateDlg.h"
#include "Qry/QryOrderCommRate.h"
#include "Hyperlink/ExchangeHyperlinkDlg.h"
#include "Qry/QryMainContractDlg.h"
#include "../Tool/rwJson.h"
#include "SinaDataQuote/SinaDataQuote.h"
#include "../ServiceLayer/CtpMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/********************************************全局变量**************************************************/
bool RspQryPosition_Flag = false; //持仓返回的应答, 唯有当查询持仓返回后才允许开启下一轮的持仓查询，防止ctp处理不过来阻塞卡死了
bool RspQryPositionDetail_Flag = false;
bool RspQryTradingAccount_Flag = false;
bool AlreadySetTimer = false;
map<string, bool> TimeProc_Switch = { { "QryPosition", true }, { "QryPositionDetail", false }, { "QryTradingAccount", false } }; //使用成员初始化列表进行初始化

// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(MENU_SYSTEM_LOGIN, &CMainFrame::OnSystemLogin)
	ON_COMMAND(ID_QRYCOMMISSIONRATE, &CMainFrame::OnQrycommissionrate)
	ON_COMMAND(ID_QryInstrumentMarginRate, &CMainFrame::OnQryinstrumentmarginrate)
	ON_COMMAND(ID_QRYEXCHANGERATE, &CMainFrame::OnQryexchangerate)
	ON_COMMAND(ID_QRYORDERCOMMRATE, &CMainFrame::OnQryordercommrate)
	ON_COMMAND(ID_LINKEXCHANGE, &CMainFrame::OnLinkexchange)
	ON_COMMAND(ID_QRYMAINCONTRACT, &CMainFrame::OnQrymaincontract)
	ON_MESSAGE(WM_UPDATE_STATUSBAR, &CMainFrame::OnUpdateStatusBar)//将消息与消息映射函数相绑定
	//下面这句话有很大的bug，若添加该消息映射，会导致添加菜单项不成功，具体原因未知
	//ON_COMMAND_RANGE(ID_SEPARATOR, ID_INDICATOR_COMMISSION, NULL)//为了能改变状态栏的文字的颜色和背景颜色需要加入oncommand
	ON_COMMAND(MENU_SYSTEM_LOGINOUT, &CMainFrame::OnSystemLoginout)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_AVAILABLE,
	ID_INDICATOR_STATICPROFITLOSS,
	ID_INDICATOR_CLOSEPROFIT,
	ID_INDICATOR_POSITIONPROFIT,
	ID_INDICATOR_COMMISSION,
	//可通过注释来删减状态指示器
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	get_CtpMsgDistributor()->AddReceiver(this);
}

CMainFrame::~CMainFrame()
{
	get_CtpMsgDistributor()->DelReceiver(this);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建本地消息线程,并且设置本地消息线程的全局ID号
	hThread_LocalMessageThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Timerproc_LocalMessageThread, NULL, 0, &ThreadID_LocalMessageThread); 
	get_global_mgr().SetLocalMessageThreadID(this->ThreadID_LocalMessageThread);

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = FALSE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	
	if (!m_wndAccountStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndAccountStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	RECT rcClient;
	int width[6];									//状态栏分割数组
	GetClientRect(&rcClient);				//获取父窗口矩形
	int length = rcClient.right / 6;       //根据父窗口宽度来分割指定大小
	width[0] = length/3;                     //状态栏第一栏宽度，为分割长度length
	width[1] = length ;                  
	width[2] = length*1.5;
	width[3] = length ;
	width[4] = length ;
	width[5] = length ;
	m_wndAccountStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, width[0]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_SEPARATOR), RGB(255, 128, 128));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_SEPARATOR), RGB(0, 0, 0));

	m_wndAccountStatusBar.SetPaneInfo(1, ID_INDICATOR_STATICPROFITLOSS, SBPS_NORMAL, width[1]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_STATICPROFITLOSS), RGB(255, 204, 153));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_STATICPROFITLOSS), RGB(0, 0, 0));

	m_wndAccountStatusBar.SetPaneInfo(2, ID_INDICATOR_AVAILABLE, SBPS_NORMAL, width[2]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_AVAILABLE), RGB(147, 220, 255));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_AVAILABLE), RGB(255, 0, 0));

	m_wndAccountStatusBar.SetPaneInfo(3, ID_INDICATOR_CLOSEPROFIT, SBPS_NORMAL, width[3]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_CLOSEPROFIT), RGB(255, 204, 153));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_CLOSEPROFIT), RGB(0, 0, 255));

	m_wndAccountStatusBar.SetPaneInfo(4, ID_INDICATOR_POSITIONPROFIT, SBPS_NORMAL, width[4]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_POSITIONPROFIT), RGB(147, 220, 255));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_POSITIONPROFIT), RGB(255, 0, 0));

	m_wndAccountStatusBar.SetPaneInfo(5, ID_INDICATOR_COMMISSION, SBPS_NORMAL, width[5]);
	m_wndAccountStatusBar.SetPaneBackgroundColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_COMMISSION), RGB(255, 204, 153));
	m_wndAccountStatusBar.SetPaneTextColor(m_wndAccountStatusBar.CommandToIndex(ID_INDICATOR_COMMISSION), RGB(0, 0, 255));
	

	// TODO:  如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上): 
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndStrategyView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndStrategyView);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndFileView.AttachToTabWnd(&m_wndStrategyView, DM_SHOW, TRUE, &pTabbedBar);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	m_wndSMOBoardPane.EnableDocking(CBRS_ALIGN_ANY); //使下单板可停靠
	DockPane(&m_wndSMOBoardPane); //停靠下单板窗口

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO:  定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}


//此处用来创建停靠窗口，停靠窗口顾名思义就是需要停靠到CDockingPane上的窗口
BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	CString strStrategyView;
	bNameValid = strStrategyView.LoadString(IDS_STRATEGY_VIEW);
	ASSERT(bNameValid);
	if (!m_wndStrategyView.Create(strStrategyView, this, CRect(0, 0, 280, 280), TRUE, IDS_STRATEGY_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“策略状态”窗口\n");
		return FALSE; // 未能创建
	}
	m_wndStrategyView.SetControlBarStyle(AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE);// 禁止显示关闭按钮

	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“策略树”窗口\n");
		return FALSE; // 未能创建
	}
	m_wndFileView.SetControlBarStyle(AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE);// 禁止显示关闭按钮

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建交易情况输出窗口\n");
		return FALSE; // 未能创建
	}
	m_wndOutput.SetControlBarStyle(AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE);// 禁止显示关闭按钮

	CString strMakeOrderBoard = _T("交易");
	if (!m_wndSMOBoardPane.Create(strMakeOrderBoard, this, CRect(0, 0, 320, 320), TRUE, ID_VIEW_STANDARDMAKEORDERBOARDWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“下单板”窗口\n");
		return FALSE; // 未能创建
	}
	m_wndSMOBoardPane.SetControlBarStyle(AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE);// 禁止显示关闭按钮

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

//菜单栏选项事件处理函数
void CMainFrame::OnSystemLogin()
{
	CLoginDlg login_dlg;
	login_dlg.DoModal();//此处用于显示登录菜单选项的界面
}

void CMainFrame::OnSystemLoginout()
{
	if (::MessageBox(NULL, _T("是否确认登出"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		get_global_mgr().GetService()->logout();
	}
}

void CMainFrame::OnQrycommissionrate()
{
	CQryCommissionRate qry_commissionrate_dlg;
	qry_commissionrate_dlg.DoModal();
}


void CMainFrame::OnQryinstrumentmarginrate()
{
	CQryInstrumentMarginRateDlg qry_marginrate_dlg;
	qry_marginrate_dlg.DoModal();
}


void CMainFrame::OnQryexchangerate()
{
	CQryExchangeRateDlg qry_exchangerate_dlg;
	qry_exchangerate_dlg.DoModal();
}


void CMainFrame::OnQryordercommrate()
{
	CQryOrderCommRate qry_ordercommrate_dlg;
	qry_ordercommrate_dlg.DoModal();
}


void CMainFrame::OnLinkexchange()
{
	CExchangeHyperlinkDlg exchangelink_dlg;
	exchangelink_dlg.DoModal();
}


void CMainFrame::OnQrymaincontract()
{
	/*
	//创建非模态对话框
	CQryMainContractDlg* maincontract_dlg = new CQryMainContractDlg(); //注意非模态对话框必须非临时,否则会出现立马创建立马销毁的闪现现象, 这里有内存泄漏,暂未解决
	maincontract_dlg->Create(DIALOG_QRYMAINCONTRACT, this);
	maincontract_dlg->ShowWindow(SW_SHOW);
	*/
	//创建模态对话框
	CQryMainContractDlg maincontract_dlg;
	maincontract_dlg.DoModal();
}

LRESULT  CMainFrame::OnUpdateStatusBar(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryTradingAccount)
	{
		TradingAccount* pTradingAccount = reinterpret_cast<TradingAccount*>(lParam);
		char balance[100];
		sprintf(balance, "%0.2f", pTradingAccount->Balance);
		CString balance_cstr=_T("动态权益：");
		balance_cstr += balance;
		m_wndAccountStatusBar.SetPaneText(2, balance_cstr);

		char available[100];
		sprintf(available, "%0.2f", pTradingAccount->Available);
		CString available_cstr = _T("可用资金：");
		available_cstr += available;
		m_wndAccountStatusBar.SetPaneText(1, available_cstr);

		char closeprofit[100];
		sprintf(closeprofit, "%0.2f", pTradingAccount->CloseProfit);
		CString closeprofit_cstr = _T("平仓盈亏：");
		closeprofit_cstr += closeprofit;
		m_wndAccountStatusBar.SetPaneText(3, closeprofit_cstr);

		char positionprofit[100];
		sprintf(positionprofit, "%0.2f", pTradingAccount->PositionProfit);
		CString positionprofit_cstr = _T("持仓盈亏：");
		positionprofit_cstr += positionprofit;
		m_wndAccountStatusBar.SetPaneText(4, positionprofit_cstr);

		char commission[100];
		sprintf(commission, "%0.2f", pTradingAccount->Commission);
		CString commission_cstr = _T("手续费：");
		commission_cstr += commission;
		m_wndAccountStatusBar.SetPaneText(5, commission_cstr);
	}
	return 1;
}

void CMainFrame::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	//下面1和2标记变量用于订阅深度行情时，控制执行顺序，
	static bool bTraderLogined = false;	//用于标记是否交易api已登录并查询所有上市合约完毕
	static bool bMarketLogined = false;	//用于标记行情api是否已登录上
	static vector<string> instruments; 

	if (wParam == ctp_msg::RspFrontConnected)
	{//交易api前置连接成功
	}
	if (wParam == ctp_msg::RspFrontDisconnected)
	{
		bTraderLogined = false;
		if (hThread != NULL)
		{
			CloseHandle(hThread);			  //关闭线程句柄
			TerminateThread(hThread, 0); //强行终止线程,不太稳定
		}
		AlreadySetTimer = false;			//定时查询线程销毁后重置状态
	}
	if (wParam == ctp_msg::RspFrontDisconnected_Md)
	{//行情api断开连接
		bMarketLogined = false;
	}
	if (wParam == ctp_msg::RspUserLogined)
	{//交易api登录成功
	}
	if (wParam == ctp_msg::RspUserLogined_Md)
	{
		bMarketLogined = true;//表明行情api已经登录上
		if (bTraderLogined)//若交易api先登录并完成了所有上市合约信息的查询，之后行情api才登录上，此时可以去订阅深度行情了
		{
			get_global_mgr().GetService()->request(ctp_msg::QryMarketData, &instruments);//此处订阅所有上市品种的行情
			bMarketLogined = false;//订阅成功后，将bMarketLogined改为false,防止交易api处重复订阅深度行情
			bTraderLogined = false;//重置回初始状态
		}
	}
	if (wParam == RspQrySettlementInfoConfirm)
	{
	}
	if (wParam == RspSettlementInfoConfirm)
	{

	}
	if (wParam == RspQryexchanges)
	{//当查询交易所信息成功返回后，我们可以开始查询所需合约
		vector<string> my_instruments = get_config_reader().GetMyInstruments();
		get_global_mgr().GetService()->request(QryInstruments, &my_instruments);
	}
	if (wParam == RspQryInstrument)
	{
		bTraderLogined = true;//当全部上市合约信息查询成功后，才标记交易api成功登录上，因为默认唯有当所有上市合约信息都查询完毕后，订阅深度行情才可有传参
		vector<BaseData>* instrument_list = reinterpret_cast<vector<BaseData>*>(lParam);//因为默认是查询所有合约，此处将返回的所有合约信息以vector的形式保存下来
		for (int i = 0; i < instrument_list->size(); ++i)
		{
			instruments.push_back((*instrument_list)[i].code);//提取basedata结构体中的合约名，逐个存取
		}
		if (bMarketLogined)//订阅深度行情，需要两个前提，一个是行情api必须连接上，二是必须传递要订阅的合约列表进去，为此当交易api将合约信息查询完毕后在开始订阅，是非常合理的
		{
			get_global_mgr().GetService()->request(ctp_msg::QryMarketData, &instruments);//此处订阅所有上市品种的行情
			bTraderLogined = false;//订阅成功后，将bTraderLogined改为false,防止其它地方再次订阅行情
			bMarketLogined = false;//重置回初始状态
		}
		//以下通过多线程间异步通讯，实现将合约数据存入sql server数据库的操作，可多处PostThreadMessage来实现本地消息映射机制
		//::PostThreadMessage(get_global_mgr().GetLocalMessageThreadID(), WM_DATASTORE_BASEDATA, RspQryInstrument, (LPARAM)lParam); //lParam已经是地址了
	}
	if (wParam == ctp_msg::RspQryTradingAccount)
	{
		RspQryTradingAccount_Flag = true;
		PostMessage(WM_UPDATE_STATUSBAR, wParam, (LPARAM)lParam); //刷新状态栏(账户情况)
	}
	if (wParam == RspQryInvestorPosition)
	{
		RspQryPosition_Flag = true;
	}
	if (wParam == ctp_msg::RspQryCommissionRate)
	{
	}
	if (wParam == ctp_msg::RspQryOrder)
	{
	}
	if (wParam == ctp_msg::RspQryTrade)
	{
	}
	if (wParam == ctp_msg::RspQryInvestorPositionDetail)
	{
		RspQryPositionDetail_Flag = true;
	}
	if (wParam == ctp_msg::RspQryParkedOrder)
	{
	}
	if (wParam == ctp_msg::RspQryParkedOrderAction)
	{//唯有当一轮初始化查询都做完的时候，才开始创建定时查询线程，防止对CTP同时申请查询造成的阻塞
		if (AlreadySetTimer == false)
		{//检查是否已经设置定时查询线程
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Timerproc_ReqQry, NULL, 0, &ThreadID); //创建子线程
			AlreadySetTimer = true;
		}
	}
}

//线程函数
void CALLBACK  Timerproc_ReqQry()
{
	while (true)
	{
		if (get_global_mgr().GetTimeQry_Switch() == true)
		{
			Sleep(1000);
			if (TimeProc_Switch["QryPosition"] == true)
			{
				if (RspQryPosition_Flag == true)
				{
					PositionParam param;
					memset(&param, 0, sizeof(param));
					//此处传入的申请结构体为空，由底层做些默认处理即可
					get_global_mgr().GetService()->request(ctp_msg::QryPosition, &param);
					RspQryPosition_Flag = false;
				}
				TimeProc_Switch["QryPosition"] = false;
				TimeProc_Switch["QryPositionDetail"] = true;
				TimeProc_Switch["QryTradingAccount"] = false;
				continue;
			}

			if (TimeProc_Switch["QryPositionDetail"] == true)
			{
				if (RspQryPositionDetail_Flag == true)
				{
					QryInvestorPositionDetailParam param;
					memset(&param, 0, sizeof(param));
					//此处传入的申请结构体为空，由底层做些默认处理即可
					get_global_mgr().GetService()->request(ctp_msg::QryInvestorPositionDetail, &param);
					RspQryPositionDetail_Flag = false;
				}
				TimeProc_Switch["QryPosition"] = false;
				TimeProc_Switch["QryPositionDetail"] = false;
				TimeProc_Switch["QryTradingAccount"] = true;
				continue;
			}

			if (TimeProc_Switch["QryTradingAccount"] == true)
			{
				if (RspQryTradingAccount_Flag == true)
				{
					QryTradingNoticeParam param;
					memset(&param, 0, sizeof(param));
					//此处传入的申请结构体为空，由底层做些默认处理即可
					get_global_mgr().GetService()->request(ctp_msg::QryTradingAccount, &param);
					RspQryTradingAccount_Flag = false;
				}
				TimeProc_Switch["QryPosition"] = true;
				TimeProc_Switch["QryPositionDetail"] = false;
				TimeProc_Switch["QryTradingAccount"] = false;
				continue;
			}
		}
	}
}

void CALLBACK Timerproc_LocalMessageThread()
{
	while (true)
	{
		Sleep(1000);
		{//处理预埋单及预埋撤单的主动界面更新
			time_t nowtime; //time_t是一个长整型,表示一个日历时间，也就是从1970年1月1日0时0分0秒到此时的秒数
			nowtime = time(NULL); //获取日历时间
			struct tm* local;//转换成本地时间，要用到tm结构,其是C风格下的结构体
			local = localtime(&nowtime);  //获取本地时间
			if ((local->tm_hour == 9 && local->tm_min == 0 && local->tm_sec == 5) ||
				(local->tm_hour == 13 && local->tm_min == 30 && local->tm_sec == 5) ||
				(local->tm_hour == 10 && local->tm_min == 30 && local->tm_sec == 5) ||
				(local->tm_hour == 21 && local->tm_min == 0 && local->tm_sec == 5))
			{//若本地时间结点分别坐落于早盘开盘第一分钟尾, 下午盘开盘第一分钟尾, 夜盘开盘第一分钟尾, 则此时要主动查询预埋单情况,因为此时都变为已发送状态
				get_global_mgr().SetStopTimeQry(); //停止定时查询, 特殊查询优先级较高
				Sleep(1000);//睡1秒是为了与定时查询尽可能错开, 防止阻塞
				QryParkedOrderParam param;
				memset(&param, 0, sizeof(param));
				get_global_mgr().GetService()->request(ctp_msg::QryParkedOrder, &param);
				get_global_mgr().SetStartTimeQry(); //恢复定时查询, 特殊查询优先级较高
			}
			if ((local->tm_hour == 9 && local->tm_min == 0 && local->tm_sec == 10) ||
				(local->tm_hour == 13 && local->tm_min == 30 && local->tm_sec == 10) ||
				(local->tm_hour == 10 && local->tm_min == 30 && local->tm_sec == 10) ||
				(local->tm_hour == 21 && local->tm_min == 0 && local->tm_sec == 10))
			{//若本地时间结点分别坐落于早盘开盘第一分钟尾, 下午盘开盘第一分钟尾, 夜盘开盘第一分钟尾, 则此时要主动查询预埋撤单情况,因为此时都变为已发送状态
				get_global_mgr().SetStopTimeQry(); //停止定时查询, 特殊查询优先级较高
				Sleep(1000);//睡1秒是为了与定时查询尽可能错开, 防止阻塞
				QryParkedOrderActionParam param;
				memset(&param, 0, sizeof(param));
				get_global_mgr().GetService()->request(ctp_msg::QryParkedOrderAction, &param);
				get_global_mgr().SetStartTimeQry(); //恢复定时查询, 特殊查询优先级较高
			}
		}

		MSG msg;//消息结构体,用于接收
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{//自建消息循环::GetMessage(&msg, NULL, 0, 0)
			if (msg.message == WM_DATASTORE_BASEDATA)
			{
				SqlDataStore_BaseData(msg.wParam, msg.lParam);
			}
		}
	}
}



