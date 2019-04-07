#pragma once
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../../ServiceLayer/DataTypes.h"
#include <vector>
#include <set>
#include <map>
#include <utility>//pair类模板使用
#define MSG_QRYMAINCONTRACT WM_USER+1001 //用于给PostThreadMessage使用的自定义消息

// CQryMainContractDlg dialog

class CQryMainContractDlg : public CDialogEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CQryMainContractDlg)

public:
	CQryMainContractDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQryMainContractDlg();

// Dialog Data
	enum { IDD = DIALOG_QRYMAINCONTRACT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

	virtual BOOL OnInitDialog();

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedQrymaincontract();

public:
	bool button_first_click;
	int Product_InvaildInstrument_cnt;//该产品类型下有效合约数目
	int insertInstrument_cnt;
	vector<DepthMarketData> product_invaildInstrumentSet;

	bool button_second_click;
	map<string, int> Product_InvaildInstrument_cnt_multimap;
	map<string, int> Product_insertInstrument_cnt;
	multimap<string, DepthMarketData> product_invaildInstrument_insert_multimap;
	vector<CString> maincontract_vector;

	HANDLE  hThread;   //线程句柄
	DWORD  ThreadID;  //线程ID

public:
	afx_msg void OnBnClickedSpecificqrymaincontract();

public:
	friend void ThreadFunc_UpdateDlg(); //友元线程函数
};

struct  MSG_Param_QryMainContract
{//线程间传送的消息结构体
	CQryMainContractDlg* pQryMainContractDlg;
	WPARAM wParam;
	LPARAM lParam;
};