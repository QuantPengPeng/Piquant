
#include "stdafx.h"
#include "../App.h"
#include "RealtimePriceDlg.h"
#include "afxdialogex.h"
#include "StringHelper.h"
#include "DrawKLineImagePanel.h"
#include "DrawBuySellPanel.h"

// CRealtimePriceDlg dialog

static CCriticalSection cs;
static std::string ProductCode_eastmoney = "";	//记录产品代码, 东方财富用
static std::string ProductCode_Sina = "";				//记录产品代码, 新浪财经用
static void GlobalSetStockCode(string code_1,  string code_2)
{
	cs.Lock();
	ProductCode_eastmoney = code_1; //用以设置产品代码名，此处做了访问冲突控制
	ProductCode_Sina = code_2;
	cs.Unlock();
}
static std::string GlobalGetStockCode(int index)
{//index=0为东方财富用， index=1为新浪财经用
	std::string code;
	cs.Lock();
	if (index==0)
	{
		code = ProductCode_eastmoney;
	}
	else
	{
		code = ProductCode_Sina;
	}
	cs.Unlock();
	return code;
}

// http 数据查询线程
namespace http_thread
{
	unsigned __stdcall ThreadRoutine(PVOID pArgs);
	bool bAlive = false;
	unsigned threadID = 0;
	HANDLE hThread = INVALID_HANDLE_VALUE;
	HANDLE hEvent = INVALID_HANDLE_VALUE;

	// 开始查询线程
	void Start(CRealtimePriceDlg * pThis)
	{
		if (bAlive) 
		{//线程是否已经创建过
			return;
		}
		if (INVALID_HANDLE_VALUE != hEvent) 
		{//句柄为空
			return;
		}
		if (INVALID_HANDLE_VALUE != hThread) 
		{//线程ID也为空
			return;
		}
		bAlive = true;
		hEvent = CreateEvent(NULL
			, FALSE //自动重置
			, FALSE // 初始是无信号状态
			, NULL);
		hThread = (HANDLE)(::_beginthreadex(nullptr, 0, ThreadRoutine, pThis, 0, &threadID));
	}

	// 停止查询线程
	void Stop(DWORD dwTimeout = 1000 * 3 + 200)
	{
		if (bAlive) 
		{
			bAlive = false;
		}
		if (INVALID_HANDLE_VALUE != hEvent) 
		{
			CloseHandle(hEvent);
			hEvent = INVALID_HANDLE_VALUE;
		}
		if (INVALID_HANDLE_VALUE != hThread) 
		{
			WaitForSingleObject(hThread, dwTimeout);
			TerminateThread(hThread, 0);
			CloseHandle(hThread);
			hThread = INVALID_HANDLE_VALUE;
		}
	}

	// 触发事件,进行一次查询
	void FireEvent()
	{
		SetEvent(hEvent);
	}

	// 查询线程:
	// 每3秒查询一次(或调用 FireEvent), 查询成功后发送消息
	unsigned __stdcall ThreadRoutine(PVOID pArgs)
	{
		// 获得参数
		auto * pThis = (CRealtimePriceDlg*)pArgs;
		// 查询辅助工具
		data_http::CHttpDataSource http;
		// 查到的数据
		data_http::CHttpDataEntry httpData;
		while (bAlive) 
		{//检查线程是否已经创建
			// 每2秒取一次数据
			DWORD dwWait = ::WaitForSingleObject(hEvent, 3 * 1000);
			// 如果超时,或者有信号, 则进行查询
			if (WAIT_TIMEOUT == dwWait || WAIT_OBJECT_0 == dwWait) 
			{
				// 拼接查询时用到的URL
				std::string Code_eastMoney="";
				CString Code_eastMoney_cstr = _T("");
				std::string Code_sina = "";
				CString Code_sina_cstr = _T("");

				{
					std::string code = GlobalGetStockCode(0);//获取东方财富用的产品ID
					if (code.size() == 0)
					{
						continue;
					}
					Code_eastMoney = code;
					Code_eastMoney_cstr = code.c_str();

					code = GlobalGetStockCode(1);//获取新浪财经用的的产品ID
					if (code.size() == 0)
					{
						continue;
					}
					Code_sina = code;
					Code_sina_cstr = code.c_str();
				}
				// 拼接查询用到的URL
				CString strGIFPath;
				string GIFPath;
				{
					CString strTempPath;
					GetTempPath(MAX_PATH, strTempPath.GetBuffer(MAX_PATH)); //GetTempPath是API，获取的是%Temp%的变量值。
					strTempPath.ReleaseBuffer();
					//strTempPath.AppendFormat(_T("%08d_%s_minutes.gif"), GetTickCount(), Code_eastMoney_cstr.GetString());
					strTempPath.AppendFormat(_T("%s_minutes.gif"), Code_eastMoney_cstr.GetString());
					strGIFPath = strTempPath.GetString();
					USES_CONVERSION;
					GIFPath = CW2A(strGIFPath.GetString());
				}
				//请求数据
				http.QueryStockKLineMin(Code_eastMoney, GIFPath);
				http.QueryStockRealTimeData(StringHelper::UnicodeToANSI(Code_sina_cstr.GetString()), httpData); //将请求到的数据装入httpData
				// 传给窗口的数据,在窗口接收消息时释放
				auto p = new data_http::CHttpDataEntry(httpData);
				auto pFile = new CString(strGIFPath); //gif图片路径名
				pThis->PostMessage(CRealtimePriceDlg::sc_MSG_HTTP_QUERY_END, (WPARAM)p, (LPARAM)pFile);
			}
			else 
			{
				continue; // 继续
			}
		}
		//__end:
		_endthreadex(0);
		return 0;
	}
}


IMPLEMENT_DYNAMIC(CRealtimePriceDlg, CDialogEx)

CRealtimePriceDlg::CRealtimePriceDlg(CString InstrumentID_eastMoneyneed, CString InstrumentID_sinaneed, CWnd* pParent /*=NULL*/)
	: CDialogEx(CRealtimePriceDlg::IDD, pParent)
	, m_strCode(_T(""))
{
	//构造函数中接收所在期货行的instrumentID
	USES_CONVERSION;
	this->rowInstrumentID_eastmoney = CW2A(InstrumentID_eastMoneyneed.GetString()); //接收东方财富所需的产品ID
	this->rowInstrumentID_sina = CW2A(InstrumentID_sinaneed.GetString()); //接收新浪财经所需的产品ID
}

CRealtimePriceDlg::~CRealtimePriceDlg()
{
}

BOOL CRealtimePriceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	std::string instrumentID_eastmoney = this->rowInstrumentID_eastmoney; //获取东方财富品种名
	std::string instrumentID_sina = this->rowInstrumentID_sina; //获取新浪财经品种名
	// 给查询线程传递数据
	GlobalSetStockCode(instrumentID_eastmoney, instrumentID_sina);
	// 开始查询线程(如果未开启,会开启线程)
	http_thread::Start(this);
	// 触发查询事件,使线程立即开始一次新查询
	http_thread::FireEvent();
	return TRUE;  
}

BOOL CRealtimePriceDlg::PreTranslateMessage(MSG* pMsg)
{
	// 防止 ESC/回车键 自动退出
	if (pMsg->message == WM_KEYFIRST) 
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE; // 返回非0,代表此消息不继续进行分发
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CRealtimePriceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CRealtimePriceDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: Add your message handler code here
	http_thread::Stop();
	// 销毁窗口前 清理数据
	CDialogEx::OnDestroy();
}

LRESULT CRealtimePriceDlg::OnMsgHttpQueryEnd(WPARAM wParam, LPARAM lParam)
{
	// 存储实时数据
	auto httpData = (data_http::CHttpDataEntry *)wParam;
	// 下载的K线图路径
	auto strFilePath = (CString *)lParam;
	// 保存数据,并删除指针
	if (httpData) 
	{
		m_httpData = *httpData;
		delete httpData;
		httpData = nullptr;
	}

	if (strFilePath) 
	{
		m_strFilePath = *strFilePath;
		delete strFilePath;
		strFilePath = nullptr;
	}
	
	// 更新图片显示，此处会触发OnPaint
	CRect r;
	GetDlgItem(STATIC_REALTIMEPRICEDISP)->GetWindowRect(r);
	ScreenToClient(&r);
	InvalidateRect(&r, TRUE);
	return 0;
}

BEGIN_MESSAGE_MAP(CRealtimePriceDlg, CDialogEx)
	ON_MESSAGE(sc_MSG_HTTP_QUERY_END, &CRealtimePriceDlg::OnMsgHttpQueryEnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CRealtimePriceDlg message handlers


void CRealtimePriceDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// 放一个图片控件是为了便于调整位置
	CRect rc;
	GetDlgItem(STATIC_REALTIMEPRICEDISP)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	// 起始位置
	const int x = rc.left;
	const int y = rc.top;
	// 如果有数据
	if (true) //!m_httpData.buyCount01.empty()通过买一价来检查是否有数据, 若有才更新，此处暂不做处理
	{
		using namespace Gdiplus;
		//创建一个bmp
		Bitmap bmp(rc.Width(), rc.Height());
		//创建bmp的 gh
		Graphics bmpGraphics(&bmp);
		bmpGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
		// 在bmp上面画图
		// 画 K线图
		static CDrawKLineImagePanel kLine;
		kLine.Update(bmpGraphics, x, y, m_strFilePath.c_str());

		// 画买卖报价,也即tick切片数据
		static CDrawBuySellPanel dp;
		dp.Update(bmpGraphics, 610, y, m_httpData);	//注意调节两图位置

		// 创建dc的gdi对象
		Graphics graphics(dc.GetSafeHdc());
		CachedBitmap cachedBmp(&bmp, &graphics);
		// 将 bmp 画到真正的设备上面
		graphics.DrawCachedBitmap(&cachedBmp, 0, 0);
	}
}



