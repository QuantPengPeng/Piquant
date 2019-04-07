#pragma once
#include "afxwin.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"


// CClosePositionDlg dialog

class CClosePositionDlg : public CDialogEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CClosePositionDlg)

public:
	CClosePositionDlg(CString InstrumentID,
								 CString Direction,
								 CString Position,
								 CString PrePosition,
								 CString TodayPosition,
								 CString SettlementPrice,
								CWnd* pParent = NULL);   // standard constructor
	virtual ~CClosePositionDlg();

// Dialog Data
	enum { IDD = DIALOG_POSITION };


	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

public:
	// 合约框的cstring值变量
	CString m_strInstrument;
	// 绑定方向框的ctring值变量
	CString m_strDirection;
	// 绑定交易量框的ctring值变量
	CString m_strVolume;
	afx_msg void OnBnClickedPricetype();
	// 价格类型选择的控件按钮
	CButton m_bnPriceType;
	// 绑定交易价格框的double值变量
	double m_Price;
	afx_msg void OnBnClickedClose();
	// 绑定交易所复选框的ctring值变量
	CString m_strExchange;
	// 绑定下拉框的控件变量
	CComboBox m_cbExchange;

	CString SettlementPrice;//自定义的，与控件无关
	afx_msg void OnEnChangeVolume();
	afx_msg void OnEnChangePrice();
	afx_msg void OnEnChangeTouchprice();
	afx_msg void OnClose();
};
