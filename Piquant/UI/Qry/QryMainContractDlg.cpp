// QryMainContractDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "QryMainContractDlg.h"
#include "afxdialogex.h"
#include "../GlobalMgr.h"
#include "../../ServiceLayer/CtpService.h"
#include "../../ServiceLayer/CtpMsgDistributor.h"
#include "../resource.h"
#include "../../ServiceLayer/CtpMsg.h"

// CQryMainContractDlg dialog

IMPLEMENT_DYNAMIC(CQryMainContractDlg, CDialogEx)

CQryMainContractDlg::CQryMainContractDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQryMainContractDlg::IDD, pParent)
{
	Product_InvaildInstrument_cnt = 0;
	insertInstrument_cnt = 0;
	button_first_click=false;
	button_second_click = false;
}

CQryMainContractDlg::~CQryMainContractDlg()
{
}

void CQryMainContractDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQryMainContractDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(MFCBUTTON_QRYMAINCONTRACT, &CQryMainContractDlg::OnBnClickedQrymaincontract)
	ON_BN_CLICKED(BUTTON_SPECIFICQRYMAINCONTRACT, &CQryMainContractDlg::OnBnClickedSpecificqrymaincontract)
END_MESSAGE_MAP()


BOOL CQryMainContractDlg::OnInitDialog()
{
	get_global_mgr().SetStopTimeQry(); //停止定时查询
	get_CtpMsgDistributor()->AddReceiver(this);
	vector<BaseData> instruments;
	set<string> productSet;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_basic_data(instruments);
		for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
		{
			productSet.insert((*it).ProductID);
		}
		if (!productSet.empty())
		{
			if (((CComboBox*)GetDlgItem(COMBO_PRODUCT))->GetCount() != 0)
			{//防止二次加载,为此再加载前若不为空,先清除所有先
				((CComboBox*)GetDlgItem(COMBO_PRODUCT))->ResetContent();
			}
			for (set<string>::iterator it = productSet.begin(); it != productSet.end(); ++it)
			{
				string str = *it;
				((CComboBox*)GetDlgItem(COMBO_PRODUCT))->AddString(CString(str.c_str()));
			}
			for (set<string>::iterator it = productSet.begin(); it != productSet.end(); ++it)
			{
				string str = *it;
				((CListBox*)GetDlgItem(LIST_PRODUCTSET))->AddString(CString(str.c_str()));
			}
		}
	}
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc_UpdateDlg, NULL, 0, &ThreadID); //创建子线程
	return TRUE;
}


// CQryMainContractDlg message handlers


void CQryMainContractDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	get_CtpMsgDistributor()->DelReceiver(this);
	if (hThread != NULL)
	{
		CloseHandle(hThread); //关闭线程句柄
		TerminateThread(hThread, 0); //强行终止线程
	}
	get_global_mgr().SetStartTimeQry(); //恢复定时查询
	__super::OnClose();
}

void CQryMainContractDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ctp_msg::RspQryDepthMarketData)
	{//向子线程传递消息
		static MSG_Param_QryMainContract param;
		param.pQryMainContractDlg = this;
		param.wParam = wParam;
		param.lParam = lParam;
		::PostThreadMessage(ThreadID, MSG_QRYMAINCONTRACT, 0, (LPARAM)&param);
	}
}

void CQryMainContractDlg::OnBnClickedQrymaincontract()
{
	button_first_click = true;
	USES_CONVERSION;
	CString product_cstr;
	((CComboBox*)GetDlgItem(COMBO_PRODUCT))->GetWindowText(product_cstr);
	string product = T2A(product_cstr);
	string exchangeID;
	vector<BaseData> instruments_product; //获取指定产品下的正在交易合约
	vector<BaseData> instruments;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_basic_data(instruments);
		for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
		{
			if (strcmp(product.c_str(), it->ProductID)==0)
			{
				instruments_product.push_back(*it);
			}
		}
		for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
		{
			if (strcmp(product.c_str(), it->ProductID)==0)
			{
				exchangeID = it->ExchangeID;
				break;
			}
		}
	}
	Product_InvaildInstrument_cnt = instruments_product.size();
	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		static QryDepthMarketDataＰaram req;
		for (int i = 0; i < instruments_product.size(); i++)
		{
			req.InstrumentID = instruments_product[i].code;
			req.ExchangeID = instruments_product[i].ExchangeID;
			get_global_mgr().GetService()->request(ctp_msg::QryDepthMarketData, &req);
			//PostMessage(WM_CLOSE, NULL, NULL);
		}
	}
}




void CQryMainContractDlg::OnBnClickedSpecificqrymaincontract()
{
	button_second_click = true;
	USES_CONVERSION;
	int nSel = ((CListBox*)GetDlgItem(LIST_PRODUCTSET))->GetSelCount();
	CArray< int, int& > arrayListSel;
	arrayListSel.SetSize(nSel);
	((CListBox*)GetDlgItem(LIST_PRODUCTSET))->GetSelItems(nSel, arrayListSel.GetData());
	CString product_cstr;
	vector<string> product_set; //产品集合
	for (int i = 0; i < nSel; i++)
	{
		((CListBox*)GetDlgItem(LIST_PRODUCTSET))->GetText(arrayListSel[i], product_cstr);
		product_set.push_back(T2A(product_cstr));
	}

	multimap<string, BaseData> product_InvaildInstrument_multimap;
	vector<BaseData> instruments;
	if (NULL != get_global_mgr().GetService())
	{
		get_global_mgr().GetService()->get_basic_data(instruments);
		for (vector<BaseData>::iterator it = instruments.begin(); it != instruments.end(); ++it)
		{
			for (int i = 0; i < product_set.size(); ++i)
			{
				if (strcmp(product_set[i].c_str()  ,  it->ProductID) == 0)
				{
					product_InvaildInstrument_multimap.insert(pair<string, BaseData>(product_set[i], *it));
				}
			}
		}
		for (int j = 0; j < product_set.size(); ++j)
		{
			int cnt=product_InvaildInstrument_multimap.count(product_set[j]); //由键查数
			Product_InvaildInstrument_cnt_multimap.insert(pair<string, int>(product_set[j], cnt));
		}
	}

	if (::MessageBox(NULL, _T("是否确认查询"), _T("确认"), MB_OKCANCEL) == IDOK)
	{
		QryDepthMarketDataＰaram req;
		for (multimap<string, BaseData>::iterator it = product_InvaildInstrument_multimap.begin(); it != product_InvaildInstrument_multimap.end(); ++it)
		{
			req.InstrumentID = it->second.code;
			req.ExchangeID = it->second.ExchangeID;
			get_global_mgr().GetService()->request(ctp_msg::QryDepthMarketData, &req);
		}
	}
}



void ThreadFunc_UpdateDlg()
{
	while (true)
	{
		MSG msg;//消息结构体,用于接收
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{//自建消息循环::GetMessage(&msg, NULL, 0, 0)
			if (msg.message == MSG_QRYMAINCONTRACT)
			{
				MSG_Param_QryMainContract messageStruct = *((MSG_Param_QryMainContract*)msg.lParam);

				if (messageStruct.wParam == ctp_msg::RspQryDepthMarketData)
				{
					if (messageStruct.pQryMainContractDlg->button_first_click == true)
					{//处理按钮1
						DepthMarketData* depthmarkert_Data = reinterpret_cast<DepthMarketData*>(messageStruct.lParam);
						messageStruct.pQryMainContractDlg->product_invaildInstrumentSet.push_back(*depthmarkert_Data);
						messageStruct.pQryMainContractDlg->insertInstrument_cnt++;
						if (messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt == messageStruct.pQryMainContractDlg->insertInstrument_cnt)
						{
							double MaxOpenInterest = 0;
							string maincontract;
							for (vector<DepthMarketData>::iterator it = messageStruct.pQryMainContractDlg->product_invaildInstrumentSet.begin(); it != messageStruct.pQryMainContractDlg->product_invaildInstrumentSet.end(); ++it)
							{
								if (it->OpenInterest > MaxOpenInterest)
								{
									MaxOpenInterest = it->OpenInterest;
									maincontract = it->InstrumentID;
								}
							}
							CString maincontract_cstr = (CString)maincontract.c_str();


							//使用win32 sdk怎样都过不了,总是报m_hwnd==NULL的错误,暂时解决不了,用等价的第二句即可
							//((CEdit*)::GetDlgItem(messageStruct.pQryMainContractDlg->m_hWnd, EDIT_MAINCONTRACT))->SetWindowText(maincontract_cstr);
							((CEdit*)messageStruct.pQryMainContractDlg->GetDlgItem(EDIT_MAINCONTRACT))->SetWindowText(maincontract_cstr);

							messageStruct.pQryMainContractDlg->product_invaildInstrumentSet.clear();
							messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt = 0;
							messageStruct.pQryMainContractDlg->insertInstrument_cnt = 0;
							messageStruct.pQryMainContractDlg->button_first_click = false;
						}
					}

					if (messageStruct.pQryMainContractDlg->button_second_click == true)
					{//处理按钮2
						DepthMarketData* depthmarkert_Data = reinterpret_cast<DepthMarketData*>(messageStruct.lParam);
						char* instrumentID = depthmarkert_Data->InstrumentID;
						string product = "";
						for (int i = 0; i < strlen(instrumentID); ++i)
						{
							if (isalpha(instrumentID[i]))
							{
								product += instrumentID[i];//拼接字符串
							}
						}
						messageStruct.pQryMainContractDlg->product_invaildInstrument_insert_multimap.insert(pair<string, DepthMarketData>(product, *depthmarkert_Data));
						messageStruct.pQryMainContractDlg->Product_insertInstrument_cnt[product]++;
						for (map<string, int>::iterator it = messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt_multimap.begin(); it != messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt_multimap.end(); ++it)
						{
							if (it->first == product && it->second == messageStruct.pQryMainContractDlg->Product_insertInstrument_cnt[product])
							{
								double MaxOpenInterest = 0;
								string maincontract;
								for (multimap<string, DepthMarketData>::iterator it = messageStruct.pQryMainContractDlg->product_invaildInstrument_insert_multimap.begin(); it != messageStruct.pQryMainContractDlg->product_invaildInstrument_insert_multimap.end(); ++it)
								{
									if (it->first == product && it->second.OpenInterest > MaxOpenInterest)
									{
										MaxOpenInterest = it->second.OpenInterest;
										maincontract = it->second.InstrumentID;
									}
								}
								CString maincontract_cstr = (CString)maincontract.c_str();
								messageStruct.pQryMainContractDlg->maincontract_vector.push_back(maincontract_cstr);
							}
						}
						CString outMainContract = _T("");
						if (messageStruct.pQryMainContractDlg->maincontract_vector.size() == messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt_multimap.size())
						{
							CString outMainContract;
							for (vector<CString>::iterator it = messageStruct.pQryMainContractDlg->maincontract_vector.begin(); it != messageStruct.pQryMainContractDlg->maincontract_vector.end(); ++it)
							{
								outMainContract += *it;
								outMainContract += _T("\r\n");
							}
							((CEdit*)messageStruct.pQryMainContractDlg->GetDlgItem(EDIT_MULTIPLEMAINCONTRACT))->SetWindowText(_T(""));//若上次查询存在遗留，则先清除
							((CEdit*)messageStruct.pQryMainContractDlg->GetDlgItem(EDIT_MULTIPLEMAINCONTRACT))->SetWindowText(outMainContract);
							((CEdit*)messageStruct.pQryMainContractDlg->GetDlgItem(EDIT_MULTIPLEMAINCONTRACT))->UpdateWindow();
							messageStruct.pQryMainContractDlg->Product_InvaildInstrument_cnt_multimap.clear();
							messageStruct.pQryMainContractDlg->Product_insertInstrument_cnt.clear();
							messageStruct.pQryMainContractDlg->product_invaildInstrument_insert_multimap.clear();
							messageStruct.pQryMainContractDlg->maincontract_vector.clear();
							messageStruct.pQryMainContractDlg->button_second_click = false;
						}
					}
				}
			}
		}
	}
}





