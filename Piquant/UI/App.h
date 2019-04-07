//应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


class CApp : public CWinAppEx
{
public:
	CApp();

	CMultiDocTemplate* pDocTemplate_CFFEX;
	CMultiDocTemplate* pDocTemplate_ShowInstrument;
	CMultiDocTemplate* pDocTemplate_SHFEINEView;
	CMultiDocTemplate* pDocTemplate_DCEView;
	CMultiDocTemplate* pDocTemplate_CZCEView;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
};

extern CApp theApp;
