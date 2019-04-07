#pragma once
#include "../resource.h"

// CCancelOrderDlg dialog

class CCancelOrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCancelOrderDlg)

public:
	CCancelOrderDlg(CString OrderSysID,
								CString InstrumentID,
								CString Direction,
								CString OpenClose,
								CString Price,
								CString insertVolume,
								CString ExchangeID,
								CWnd* pParent = NULL);   // standard constructor
	virtual ~CCancelOrderDlg();

// Dialog Data
	enum { IDD = DIALOG_CANCELORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

public:
	// 绑定撤单文本框的值变量
	CString m_strOrderSysID;
	// 绑定合约文本框的值变量
	CString m_strInstrumentID;
	// 绑定方向文本框的值变量
	CString m_strDirection;
	// 绑定开平文本框的值变量
	CString m_strOpenClose;
	// 绑定交易量框的值变量
	CString m_strVolume;
	// 绑定价格文本框的值变量
	CString m_strPrice;
	//记录交易所,非控件变量
	CString exchangeID;
	afx_msg void OnBnClickedCancelorder();
	afx_msg void OnBnClickedBatchorderaction();
	afx_msg void OnBnClickedCancelparkedorder();
};
