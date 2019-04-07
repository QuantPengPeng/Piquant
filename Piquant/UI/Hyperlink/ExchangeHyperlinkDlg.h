#pragma once


// CExchangeHyperlinkDlg dialog

class CExchangeHyperlinkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExchangeHyperlinkDlg)

public:
	CExchangeHyperlinkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExchangeHyperlinkDlg();

// Dialog Data
	enum { IDD = DIALOG_LINKEXCHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink4(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink6(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink7(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSyslink8(NMHDR *pNMHDR, LRESULT *pResult);
	

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

};
