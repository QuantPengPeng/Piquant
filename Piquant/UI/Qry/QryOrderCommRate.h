#pragma once
#include "../../ServiceLayer/CtpMsgDistributor.h"

// CQryOrderCommRate dialog

class CQryOrderCommRate : public CDialogEx, public CtpMsgReceiver
{
	DECLARE_DYNAMIC(CQryOrderCommRate)

public:
	CQryOrderCommRate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CQryOrderCommRate();

// Dialog Data
	enum { IDD = DIALOG_QRYORDERCOMMRATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedQrycommrate();
};
