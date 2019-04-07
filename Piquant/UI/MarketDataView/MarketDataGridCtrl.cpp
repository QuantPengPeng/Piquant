// MarketDataGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "../App.h"
#include "MarketDataGridCtrl.h"
#include "../RealTimeDataDisp/RealtimePriceDlg.h"
#include <string>

// CMarketDataGridCtrl

IMPLEMENT_DYNAMIC(CMarketDataGridCtrl, CGridCtrl)

CMarketDataGridCtrl::CMarketDataGridCtrl()
{

}

CMarketDataGridCtrl::~CMarketDataGridCtrl()
{
}


void CMarketDataGridCtrl::OnFixedColumnClick(CCellID& cell)
{
	USES_CONVERSION;
	int row = cell.row;
	CString InstrumentID_eastMoneyneed = this->GetItemText(row, 0);//获取合约ID，确保第0列一定是合约ID，此处之后用作东方财富产品ID
	CString InstrumentID_sinaneed;
	CString ExchangeID = this->GetItemText(row, 19);//获取交易所ID
	std::string instrument;
	//此处要进行东方财富的合约ID转换
	if (ExchangeID==_T("SHFE"))
	{//如AG19061
		std::string instrument_str = CW2A(InstrumentID_eastMoneyneed.GetString());
		for (int i = 0; i < instrument_str.length(); ++i)
		{
			if (isalpha(instrument_str[i]))
			{
				instrument += toupper(instrument_str[i]);
			}
			else
			{
				instrument += instrument_str[i];
			}
		}
		InstrumentID_sinaneed = instrument.c_str(); //新浪财经期货API所需产品ID，字母已大写转换过
		instrument += "1";  //东方财富上期所产品编号后面要加1
	}
	else if (ExchangeID == _T("INE"))
	{//如SC2001_INE
		std::string instrument_str = CW2A(InstrumentID_eastMoneyneed.GetString());
		for (int i = 0; i < instrument_str.length(); ++i)
		{
			if (isalpha(instrument_str[i]))
			{
				instrument += toupper(instrument_str[i]);
			}
			else
			{
				instrument += instrument_str[i];
			}
		}
		InstrumentID_sinaneed = instrument.c_str(); //新浪财经期货API所需产品ID，字母已大写转换过
		instrument += "_INE";  //东方财富上期所产品编号后面要加_INE
	}
	else if (ExchangeID == _T("DCE"))
	{//如J20013
		std::string instrument_str = CW2A(InstrumentID_eastMoneyneed.GetString());
		for (int i = 0; i < instrument_str.length(); ++i)
		{
			if (isalpha(instrument_str[i]))
			{
				instrument += toupper(instrument_str[i]);
			}
			else
			{
				instrument += instrument_str[i];
			}
		}
		InstrumentID_sinaneed = instrument.c_str(); //新浪财经期货API所需产品ID，字母已大写转换过
		instrument += "3";  //东方财富大商所产品编号后面要加3
	}
	else if (ExchangeID == _T("CZCE"))
	{//如JR0034
		std::string instrument_str = CW2A(InstrumentID_eastMoneyneed.GetString());
		for (int i = 0; i < instrument_str.length(); ++i)
		{
			if (isalpha(instrument_str[i]))
			{
				instrument += toupper(instrument_str[i]);
			}
			else
			{
				instrument += instrument_str[i];
			}
		}
		InstrumentID_sinaneed = instrument.c_str(); //新浪财经期货API所需产品ID，字母已大写转换过
		instrument += "4";  //东方财富大商所产品编号后面要加4
	}
	else
	{//中金所是最难处理的，分类很多
		std::string instrument_str = CW2A(InstrumentID_eastMoneyneed.GetString());
		std::string product;
		std::string month;
		int cnt = 0;
		for (int i = 0; i < instrument_str.length(); ++i)
		{//获得产品ID及交割月份
			if (isalpha(instrument_str[i]))
			{
				product += instrument_str[i];
			}
			else
			{//只取月份, 沪深300期货一般为IF1906，4位数字
				cnt+=1;
				if (cnt>=3)
				{
					month += instrument_str[i];
				}
			}
		}
		if (product=="IF")
		{
			instrument += "0411";//沪深300的前缀
			instrument += month;
			instrument += "_ZJ";//后缀
		}
		else if (product == "IC")
		{
			instrument += "0611";//IC的前缀
			instrument += month;
			instrument += "_ZJ";//后缀
		}
		else if (product == "IH")
		{
			instrument += "0711";//IC的前缀
			instrument += month;
			instrument += "_ZJ";//后缀
		}
		else if (product == "T")
		{//10年期国债期货
			instrument += "1111";//IC的前缀
			instrument += month;
			instrument += "_ZJ";//后缀
		}
		else if (product == "TF")
		{//5年期国债期货
			instrument += "0511";//IC的前缀
			instrument += month;
			instrument += "_ZJ";//后缀
		}
		else
		{//CTP可以返回2年期国债期货，但东方财富无此数据
			//不做处理
		}
		InstrumentID_sinaneed = InstrumentID_eastMoneyneed; //新浪财经期货API所需产品ID，字母无需大写转换，因为CTP底层传回的中金所ID中字母就是大写的
	}
	InstrumentID_eastMoneyneed = instrument.c_str(); //形成东方财富产品编号
	CRealtimePriceDlg realtimeprice_dlg(InstrumentID_eastMoneyneed, InstrumentID_sinaneed);
	realtimeprice_dlg.DoModal();
}

BEGIN_MESSAGE_MAP(CMarketDataGridCtrl, CGridCtrl)
END_MESSAGE_MAP()



// CMarketDataGridCtrl message handlers


