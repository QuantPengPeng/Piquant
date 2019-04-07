#pragma once
#include "../../ServiceLayer/CtpMsgDistributor.h"

// CQryExchangeRateDlg dialog

class CQryExchangeRateDlg : public CDialogEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CQryExchangeRateDlg)

public:
	CQryExchangeRateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQryExchangeRateDlg();

// Dialog Data
	enum { IDD = DIALOG_QRYEXCHANGERATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedQryexchangerate();
};
