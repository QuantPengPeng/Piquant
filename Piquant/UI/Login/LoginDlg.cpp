// TestCtp01Dlg.cpp : 实现文件
//
#include "stdafx.h"
#include "LoginDlg.h"
#include "../../ServiceLayer/IService.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/ServerInfo.h"//存储经纪商信息
#include <atlconv.h> //将C++风格的字符串string转换为C风格的字符串
#define LAST_LOGIN_USER  "last_login_user"
#include "../../Tool/ConfigReader.h"//需使用ConfigReader.h中的get_config_reader()友元函数以读取xml配置内容
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLoginDlg 对话框
CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
	, m_strusername(_T(""))
	, m_strpassword(_T(""))
	, m_bSaveUserName(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strConfigfile = "./users.ini";//在构造函数中初始化ini配置文件名
	m_storepassword = true;
	//IService* ctp_service = new CTPService();//封装现有的ctp接口,所有对ctp的调用全都使用CTPService这个类来完成
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edtUsername);
	DDX_Control(pDX, IDC_EDIT2, m_edtPassword);
	DDX_Control(pDX, IDC_COMBO1, m_cbServers);
	DDX_Text(pDX, IDC_EDIT1, m_strusername);
	DDX_Text(pDX, IDC_EDIT2, m_strpassword);
	DDX_Check(pDX, IDC_CHECK1, m_bSaveUserName);
	DDX_Control(pDX, IDC_PROGRESS1, m_pbLoginProgress);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnBnClickedExitApp)
	ON_BN_CLICKED(IDC_BUTTON2, &CLoginDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_CHECK1, &CLoginDlg::OnBnClickedStorePassword)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &CLoginDlg::OnLoginProgress)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);			// 设置小图标

	// TODO:  在此添加额外的初始化代码
	get_CtpMsgDistributor()->AddReceiver(this);//this指向CLoginDlg对象，其多重继承自IServiceReceiver，因此派生类指针可以赋给基类指针

	
	//****************************************读取ini配置文件,用于获取账户和密码************************************************
	//在当前目录读取ini配置文件，Create是静态函数，归类使用，可直接类名::静态函数名；
	std::string user_name = CIniFile::GetValue("username", LAST_LOGIN_USER, m_strConfigfile);//从ini文件中读取最后登录的用户名
	std::string password = CIniFile::GetValue("password", LAST_LOGIN_USER, m_strConfigfile);//从ini文件中读取最后登录的用户密码
	m_strusername = CString(user_name.c_str());	//将从ini文件中读取到的用户名赋给相应的控件变量
	m_strpassword = CString(password.c_str());		//将从ini文件中读取到的用户密码赋给相应的控件变量
	UpdateData(FALSE);											//在OnInitDialog()时，将控件变量的值刷新到控件上以显式化
	
	//****************************************读取xml配置文件,用于获取经纪商************************************************
	if (!get_config_reader().load("brokers.xml", "config.xml")) //若读取xml配置文件失败
	{//"C:\\Users\\Quant\\Desktop\\CTP_MFC\\Project\\TestCtp01\\Debug\\brokers.xml"为xml配置文件的绝对路径
		MessageBox(_T("读取配置文件失败"),_T("错误"),MB_OK);
	}
	map<string, ServerInfo>& broker_server = get_config_reader().GetBrokers();//通过GetBrokers()公有接口获取经纪商信息
	map<string, ServerInfo>::iterator svr_iter = broker_server.begin();//创建迭代器
	USES_CONVERSION;
	for (; svr_iter != broker_server.end(); ++svr_iter)
	{
		m_cbServers.AddString(A2T(svr_iter->first.c_str()));//first获取的是map的键
		m_cbServers.SetCurSel(1);//默认选项
	}

	//默认复选框勾选
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(BST_CHECKED);

	return TRUE;  // 除非将焦点设置到控件，否则返回TRUE
}


void CLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLoginDlg::OnBnClickedExitApp()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
	exit(0);			//使程序强制退出
}


void CLoginDlg::OnBnClickedLogin()//点击登录按钮后的处理
{
	//**********第一种获取控件数据的方式比较原始，需要获取控件的句柄并依此为基础才能获取，相当麻烦**********//
	/*
	//获取用户名
	TCHAR user_name[128];//存储用户名
	HWND user_name_hwnd = ::GetDlgItem(this->GetSafeHwnd(), IDC_EDIT1);
	::GetWindowText(user_name_hwnd, user_name, 128);
	//获取用户密码
	TCHAR password[128];//存储用户密码
	HWND password_hwnd = ::GetDlgItem(this->GetSafeHwnd(), IDC_EDIT2);
	::GetWindowText(password_hwnd, password, 128);
	MessageBox(user_name,_T("用户名"), MB_OK);//测试用
	MessageBox(password, _T("用户密码"), MB_OK);//测试用
	*/


	//**********第二种获取控件数据的方式，通过选定控件并添加变量可以实现变量与控件间的绑定及数据交换，这样就可以避开先获得句柄这一环，更加方便**********//
	/*
	//此处是通过控件类变量提供的函数获取数据，控件类变量指CEdit及CComboBox这类控件类变量
	CString user_name, password, server_addr;
	m_edtUsername.GetWindowText(user_name);
	m_edtPassword.GetWindowText(password);
	m_cbServers.GetWindowText(server_addr);
	CString login_param = _T("用户：") + user_name + _T("，密码：") + password + _T("，登录：") + server_addr;
	MessageBox(login_param,_T("登录信息"),MB_OK);
	*/

	//**********第三种是通过普通值变量及UpdateData(TRUE)来把控件中的内容放到值变量中，由此获取，值变量指CString、int这类绑定控件的变量**********//
	/*
	//UpdateData(TRUE):把控件中的内容放到值变量中，由此获取;
	//UpdateData(FALSE):把值变量里的内容放到控件里并且更新控件的显示；
	UpdateData(TRUE);
	CString login_param = _T("用户：") + m_strusername + _T("，密码：") + m_strpassword + _T("，登录") ;
	MessageBox(login_param, _T("登录信息"), MB_OK);
	*/
	
	int cur_sel = m_cbServers.GetCurSel();//获取服务器组合框中的选中项
	if (CB_ERR == cur_sel)
	{
		return;//如果一项都没有选中的话，我们直接返回
	}
	CString str_sel_broker;//保存选中的经纪商名，其是map<string, ServerInfo>中的第一个string参数选项
	m_cbServers.GetLBText(cur_sel,str_sel_broker);//将选中项的文本赋给str_sel_broker变量
	USES_CONVERSION;
	string broker_name = T2A(str_sel_broker);
	map<string, ServerInfo>& broker_server = get_config_reader().GetBrokers();	//获取了从xml文档中读取的经纪商信息
	map<string, ServerInfo>::iterator find_iter = broker_server.find(broker_name); //从broker_server这个map中根据选定的broker_name找到相应的经纪商信息
	if (broker_server.end() != find_iter)//通过看是否等于超尾迭代器来看是否找到了
	{
		UpdateData(TRUE);//必须要刷新控件中的数据到控件变量中
		if (get_global_mgr().GetService()->islogined()==false)
		{//防止重复登录
			get_global_mgr().GetService()->login(T2A(m_strusername), T2A(m_strpassword), find_iter->second);
		}
		else
		{
			::MessageBox(NULL, _T("已经登录或已经申请登录"), _T("出错"), MB_OK);
			return;
		}
	}
}


void CLoginDlg::OnBnClickedStorePassword()//复选框勾选事件
{
	// TODO: Add your control notification handler code here
	CButton *pCheckbox = (CButton*)GetDlgItem(IDC_CHECK1);
	if (pCheckbox->GetCheck())
	{//要求记住密码
		m_storepassword = true;
	}
	else
	{//不记住密码
		m_storepassword = false;
	}
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLoginDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)//接收到的信息在wParam及lParam中
{
	if (wParam == ctp_msg::RspFrontConnected)
	{
		::PostMessage(m_pbLoginProgress.GetSafeHwnd(), PBM_SETPOS, 50, 0);//传给进度条PBM_SETPOS消息及50%的进度，因为只是前置连接成功，尚未登录成功
	}
	//m_pbLoginProgress.SetPos(50);
	if (wParam == ctp_msg::RspUserLogined)//当登录成功后,我们会主动关闭登录窗口
	{
		//此处要注意，一旦登录对话框被关闭，则与此对话框相关的类对象将被析构，此时若有消息还想传给该类对象，则会出现access-violation-reading-location的非法访问错误
		get_CtpMsgDistributor()->DelReceiver(this);//在销毁对话框之前,应先将其从消息接收容器receivers中剔除，防止后续再将消息传递过来，但是该对话框已被销毁
		PostMessage(WM_CLOSE, 0, 0);
	}
}

void CLoginDlg::OnLoginProgress(NMHDR *pNMHDR, LRESULT *pResult)
{//此处是进度条事件
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CLoginDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);						//将控件中的值刷新到绑定的控件变量中去
	CIniFile::Create(m_strConfigfile);	//在当前目录创建ini配置文件，Create是静态函数，可使用类名::静态函数名
	CIniFile::AddSection(LAST_LOGIN_USER, m_strConfigfile);		//添加一个段section
	USES_CONVERSION;	//使用atlconv.h及T2A函数实现从string到char类型的转换；
	CIniFile::SetValue("username", T2A(m_strusername), LAST_LOGIN_USER, m_strConfigfile);	//T2A是指从TCHAR转换为CHAR类型
	if (m_storepassword==true)
	{//保存密码
		CIniFile::SetValue("password", T2A(m_strpassword), LAST_LOGIN_USER, m_strConfigfile);
	}
	if (m_storepassword == false)
	{//不保存密码
		if (CIniFile::RecordExists("password", LAST_LOGIN_USER, m_strConfigfile) == true)
		{
			CIniFile::DeleteRecord("password", LAST_LOGIN_USER, m_strConfigfile);
		}
		else
		{

		}
	}
	get_CtpMsgDistributor()->DelReceiver(this);
	__super::OnClose();
}

