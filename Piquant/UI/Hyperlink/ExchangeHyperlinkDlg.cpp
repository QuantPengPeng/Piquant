// ExchangeHyperlinkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "ExchangeHyperlinkDlg.h"
#include "afxdialogex.h"

#define		SHFE		L"http://www.shfe.com.cn/"
#define    CFFEX       L"http://www.cffex.com.cn/"
#define    DCE			L"http://www.dce.com.cn/"
#define    CZCE			L"http://www.czce.com.cn/"
#define    INE           L"http://www.ine.cn/"
#define    FUTUREDAILY          L"http://www.qhrb.com.cn/"
#define    CTP          L"http://www.sfit.com.cn/"
#define    SIMNOW   L"http://www.simnow.com.cn/"


// CExchangeHyperlinkDlg dialog

IMPLEMENT_DYNAMIC(CExchangeHyperlinkDlg, CDialogEx)

CExchangeHyperlinkDlg::CExchangeHyperlinkDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExchangeHyperlinkDlg::IDD, pParent)
{

}

CExchangeHyperlinkDlg::~CExchangeHyperlinkDlg()
{
}

void CExchangeHyperlinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExchangeHyperlinkDlg, CDialogEx)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, SYSLINK_SHFE, &CExchangeHyperlinkDlg::OnNMClickSyslink1)
	ON_NOTIFY(NM_CLICK, SYSLINK_CFFEX, &CExchangeHyperlinkDlg::OnNMClickSyslink2)
	ON_NOTIFY(NM_CLICK, SYSLINK3_DCE_, &CExchangeHyperlinkDlg::OnNMClickSyslink3)
	ON_NOTIFY(NM_CLICK, SYSLINK_CZCE, &CExchangeHyperlinkDlg::OnNMClickSyslink4)
	ON_NOTIFY(NM_CLICK, SYSLINK_INE, &CExchangeHyperlinkDlg::OnNMClickSyslink5)
	ON_NOTIFY(NM_CLICK, SYSLINK_FUTUREDAILY, &CExchangeHyperlinkDlg::OnNMClickSyslink6)
	ON_NOTIFY(NM_CLICK, SYSLINK_CTP, &CExchangeHyperlinkDlg::OnNMClickSyslink7)
	ON_NOTIFY(NM_CLICK, SYSLINK_SIMNOW, &CExchangeHyperlinkDlg::OnNMClickSyslink8)
END_MESSAGE_MAP()

BOOL CExchangeHyperlinkDlg::OnInitDialog()
{
	GetDlgItem(SYSLINK_SHFE)->SetWindowText(L" 上海期货交易所: <a href=\"" SHFE L"\">web site</a>.");
	GetDlgItem(SYSLINK_CFFEX)->SetWindowText(L" 中国金融期货交易所: <a href=\"" CFFEX L"\">web site</a>.");
	GetDlgItem(SYSLINK3_DCE_)->SetWindowText(L" 大连期货交易所: <a href=\"" DCE L"\">web site</a>.");
	GetDlgItem(SYSLINK_CZCE)->SetWindowText(L" 郑州期货交易所: <a href=\"" CZCE L"\">web site</a>.");
	GetDlgItem(SYSLINK_INE)->SetWindowText(L" 上海国际能源交易中心: <a href=\"" INE L"\">web site</a>.");
	GetDlgItem(SYSLINK_FUTUREDAILY)->SetWindowText(L" 期货日报: <a href=\"" FUTUREDAILY L"\">web site</a>.");
	GetDlgItem(SYSLINK_CTP)->SetWindowText(L" 上期技术: <a href=\"" CTP L"\">web site</a>.");
	GetDlgItem(SYSLINK_SIMNOW)->SetWindowText(L" Simnow: <a href=\"" SIMNOW L"\">web site</a>.");
	return true;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, SHFE) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, CFFEX) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink3(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, DCE) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink4(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, CZCE) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink5(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, INE) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink6(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, FUTUREDAILY) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink7(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, CTP) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}

void CExchangeHyperlinkDlg::OnNMClickSyslink8(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	if (wcscmp(pNMLink->item.szUrl, SIMNOW) == 0)
	{
		ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	*pResult = 0;
}


// CExchangeHyperlinkDlg message handlers