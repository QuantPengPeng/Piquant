#pragma once
#include "../stdafx.h"
#include "afxwin.h"
#include "afxext.h"
#include "afxcmn.h"
#include "afxdialogex.h"
#include "../resource.h"//资源头文件必须包含，对于mfc程序而言
#include "../../ServiceLayer/CtpMsgDistributor.h"		
#include "../../Tool/IniFile.h"							


// CLoginDlg 对话框
class CLoginDlg : public CDialogEx, public CtpMsgReceiver
{
// 构造
public:
	CLoginDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = DIALOG_LOGIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExitApp();
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedStorePassword();
	afx_msg void OnClose();
	afx_msg void OnLoginProgress(NMHDR *pNMHDR, LRESULT *pResult);

	CEdit m_edtUsername;
	CEdit m_edtPassword;
	CComboBox m_cbServers;
	CString m_strusername;
	CString m_strpassword;
	BOOL m_bSaveUserName;
	CProgressCtrl m_pbLoginProgress;
	std::string m_strConfigfile;//ini文件名
	bool m_storepassword;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);
};
