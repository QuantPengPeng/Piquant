#pragma once
#include "afxwin.h"
#include "../http/HttpDataEntry.h" //获取数据的数据结构
#include "../http/HttpHelper.h"	  //http连接类
#include "../http/HttpDataSource.h" //http下的一些具体操作
#include "afxdialogex.h"
//图形库
#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")

// CRealtimePriceDlg dialog

class CRealtimePriceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRealtimePriceDlg)

public:
	CRealtimePriceDlg(CString InstrumentID_eastMoneyneed, CString InstrumentID_sinaneed, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRealtimePriceDlg();

// Dialog Data
	enum { IDD = DIALOG_REALTIMEPRICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	// 重命名 数据类型方便用
	//typedef CMRKJDatabase::TupleStockInfo INFO, *PINFO;
	// 存储 实时数据
	data_http::CHttpDataEntry m_httpData{};
	// 下载的K线图路径
	std::wstring m_strFilePath;
	CString m_strCode;
	std::string rowInstrumentID_eastmoney;
	std::string rowInstrumentID_sina;

public:
	enum WM_REAL_TIME 
	{
		sc_MSG_MIN = (WM_USER + 101),
		sc_MSG_HTTP_QUERY_END, //数据查询完毕响应消息
	};

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);  //消息分发
	afx_msg void OnPaint();	    //重绘
	afx_msg void OnDestroy();	//当窗口销毁时调用
	afx_msg LRESULT OnMsgHttpQueryEnd(WPARAM wParam, LPARAM lParam);		// 当查询线程查询结束后.会触发此消息

	DECLARE_MESSAGE_MAP()
	
};
