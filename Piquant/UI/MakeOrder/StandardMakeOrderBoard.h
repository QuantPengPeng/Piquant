#pragma once
#include "../resource.h"　//添加与资源相关的头文件
#include "afxwin.h"
#include "atltime.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "Uxtheme.h"
#pragma comment(lib, "Uxtheme.lib")
#include "../LocalMessageEvent/LocalMessage.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"

// CStandardMakeOrderBoard form view

class CStandardMakeOrderBoard : public CFormView, public CtpMsgReceiver
{//多重继承自其中的接收者容器类
	DECLARE_DYNCREATE(CStandardMakeOrderBoard)

public:
	CStandardMakeOrderBoard(CWnd* pParent = NULL);  //我们会在下单板类的构造函数中去创建下单板实体
	virtual ~CStandardMakeOrderBoard();

public:
	enum { IDD = FORMVIEW_STANDARD_MAKEORDER_BOARD };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOrderInsert();
	// 从复选框中获取的合约ID存放于此
	CString m_strInstrumentID;
	// 绑定手数文本框的值变量，记录下单手数
	UINT m_nInstrumentVolume;
	// 绑定指定价控件的控件变量, 用于切换价格类型
	CButton m_btnPriceType;
	afx_msg void OnBnClickedPricetype();
	virtual void OnInitialUpdate();     //当下单板被初次创建的时候，会调用该重载的虚函数，我们可在其中做些初始化的操作

	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

	// 绑定复选框的控件变量
	CComboBox m_cbInstruments;
	// 绑定交易所复选框的控件变量
	CComboBox m_cbExchanges;
	// 交易复选框中选定交易所的文本值变量
	CString m_strExchange;
	// 触价类型复选框的值变量
	CString m_strTouchPriceType;
	// 触价类型复选框的控件变量 
	CComboBox m_cbTouchPriceType;
	// 关联价格文本框的值变量
	double m_InstrumentPrice;
	// 触发价格变量
	double m_TouchPrice;
	// 条件单的报单价格
	double m_TradePrice;
	// 绑定有效期类型框的控件变量
	CComboBox m_cbTimeCondition;
	// 绑定有效期类型的值变量
	CString m_strTimeCondition;
	// 绑定日期选择控件的控件变量	// 绑定日期选择控件的控件变量
	CDateTimeCtrl m_GTD;
	afx_msg void OnBnClickedTouchpricePricetype();
	// 绑定触及单价格类型控件的控件变量
	CButton m_btnPriceType_TouchPrice;
	// 绑定限价类型下拉框的控件变量
	CComboBox m_cbPriceType;
	// 绑定交易量变动按钮的控件变量
	CSpinButtonCtrl m_SpinVolumeChange;
	afx_msg void OnDeltaposVolume(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeVolume();
	// 价格类型复选框控件变量,用于非预埋的限市价单
	CComboBox m_cbPriceType_Normal;
	afx_msg void OnEnChangePrice();
	// 价格变动按钮控件变量
	CSpinButtonCtrl m_SpinPriceChange;
	afx_msg void OnDeltaposPrice(NMHDR *pNMHDR, LRESULT *pResult);
	// 绑定触发价格变动按钮控件变量
	CSpinButtonCtrl m_SpinTouchPriceChange;
	afx_msg void OnDeltaposTouchprice(NMHDR *pNMHDR, LRESULT *pResult);
	// 绑定触价交易报单价格变动按钮的控件变量
	CSpinButtonCtrl m_SpinPriceChange_Touch;
	afx_msg void OnDeltaposPriceTouch(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeStopprice();
	afx_msg void OnEnChangeTradeprice();
	afx_msg void OnBnClickedRetset();
	// 绑定价格明细信息表框的控件变量
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	// 绑定价格明细框架的控件变量
	CStatic m_gbPriceDetail;
	afx_msg LRESULT  OnUpdateTickDetailDisp(WPARAM wParam, LPARAM lParam);//自定义的消息映射函数
	afx_msg void OnClose();
};


