#pragma once


// CRemoveParkedOrderDlg dialog

class CRemoveParkedOrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoveParkedOrderDlg)

public:
	CRemoveParkedOrderDlg(CString InstrumentID, CString ParkOrderID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoveParkedOrderDlg();

// Dialog Data
	enum { IDD = DIALOG_REMOVEPARKEDORDER };


	virtual BOOL OnInitDialog();

private:
	CString InstrumentID;
	CString ParkOrderID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRemoveparkedorder();
};
