#pragma once
#include "../../ServiceLayer/CtpMsgDistributor.h"

// CQryInstrumentMarginRateDlg dialog

class CQryInstrumentMarginRateDlg : public CDialogEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CQryInstrumentMarginRateDlg)

public:
	CQryInstrumentMarginRateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQryInstrumentMarginRateDlg();

// Dialog Data
	enum { IDD = DIALOG_QRYINSTRUMENTMARGINRATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

public:
	afx_msg void OnClose();

	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedQrymarginrate();
};
