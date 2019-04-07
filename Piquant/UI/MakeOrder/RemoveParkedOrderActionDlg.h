#pragma once


// CRemoveParkedOrderActionDlg dialog

class CRemoveParkedOrderActionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoveParkedOrderActionDlg)

public:
	CRemoveParkedOrderActionDlg(CString InstrumentID, CString ParkOrderActionID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoveParkedOrderActionDlg();

// Dialog Data
	enum { IDD = DIALOG_REMOVEPARKEDORDERACTION };


	virtual BOOL OnInitDialog();

private:
	CString InstrumentID;
	CString ParkOrderActionID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRemoveparkedorder();
};
