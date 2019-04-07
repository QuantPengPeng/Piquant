#include "stdafx.h"
#include "DrawBuySellPanel.h"
#include "StringHelper.h"
#include "../http/HttpDataEntry.h"
#include "../resource.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)


CDrawBuySellPanel::CDrawBuySellPanel()
{
}


CDrawBuySellPanel::~CDrawBuySellPanel()
{
}

// 更新 画面: 真正的画图,供 public: Update调用
void CDrawBuySellPanel::Update(Gdiplus::Graphics& gh, int x, int y, const std::vector<std::string>& vSellBuy, const std::string& open)
{
    using namespace Gdiplus;
    FontFamily fontFamily(L"宋体");
    Gdiplus::Font font(&fontFamily, 10, FontStyleRegular, UnitPoint);
    SolidBrush redBrush(Color(0xFF, 0xFF, 0, 0));
    SolidBrush greenBrush(Color(0xFF, 0, 0xFF, 0));
    SolidBrush whiteBrush(Color(0xFF, 0xFF, 0xFF, 0xFF));
    SolidBrush yellowBrush(Color(0xFF, 0xFF, 0xFF, 0));
    int dy = 20; //行高
    int line_w = 200; //横线的长度
    int line_h = 14 * dy; //坚线的高度, 14为格子数
    int y0 = y; // 第一条线
    int y1 = y + 1 * dy; //第二条线
    int y2 = y + 14 * dy; //第三条线, 14为离起始横线的格子数
    //画背景
    {
        // 黑色不透明
        SolidBrush brush(Color(255, 0, 0, 0));
        gh.FillRectangle(&brush, Rect(x, y, line_w, line_h));
    }
    // 画明细数据表头
    {
		CString firstLineTextInfo;
		firstLineTextInfo.LoadStringW(IDS_STRING_DRAW_INFO);
		gh.DrawString(firstLineTextInfo.GetString(), -1, &font, PointF(x, y0 + 3), &whiteBrush);
    }
    // 画各行行头名
    {
        CString sellBuyInfo[13];
		sellBuyInfo[0].LoadString(IDS_STRING_DRAW_OPEN);
        sellBuyInfo[1].LoadString(IDS_STRING_DRAW_MAXPRICE);
        sellBuyInfo[2].LoadString(IDS_STRING_DRAW_MINPRICE);
        sellBuyInfo[3].LoadString(IDS_STRING_DRAW_PRECLOSE);
        sellBuyInfo[4].LoadString(IDS_STRING_DRAW_LASTPRICE);
        sellBuyInfo[5].LoadString(IDS_STRING_DRAW_BUYPRICE01);
        sellBuyInfo[6].LoadString(IDS_STRING_DRAW_SELLPRICE01);
        sellBuyInfo[7].LoadString(IDS_STRING_DRAW_BUYCOUNT01);
        sellBuyInfo[8].LoadString(IDS_STRING_DRAW_SELLCOUNT01);
        sellBuyInfo[9].LoadString(IDS_STRING_DRAW_SETTLEMENTPRICE);
		sellBuyInfo[10].LoadString(IDS_STRING_DRAW_PRESETTLEMENTPRICE);
		sellBuyInfo[11].LoadString(IDS_STRING_DRAW_OPENINSTREST);
		sellBuyInfo[12].LoadString(IDS_STRING_DRAW_VOLUME);
        for(int index = 0; index < _countof(sellBuyInfo); ++index) 
		{
            gh.DrawString(sellBuyInfo[index].GetString(), -1, &font, PointF(x, y1 + 3 + index * dy), &whiteBrush);
        }
    }
    //画外框及分割线
    {
        Pen pen(&redBrush);
        // 画三条横线
        gh.DrawLine(&pen, Point(x, y0), Point(x + line_w, y0));
        gh.DrawLine(&pen, Point(x, y1), Point(x + line_w, y1));
        gh.DrawLine(&pen, Point(x, y2), Point(x + line_w, y2));
        // 画坚线
        gh.DrawLine(&pen, Point(x, y), Point(x, y + line_h));
        gh.DrawLine(&pen, Point(x + line_w, y), Point(x + line_w, y + line_h));
    }
    // 画传入的报价等
    for(int index = 0; index != vSellBuy.size(); index += 1) 
	{
        SolidBrush* pBrush = nullptr;
		pBrush = &redBrush; //除&redBrush，还有&greenBrush、&whiteBrush
        gh.DrawString(StringHelper::ANSIToUnicode(vSellBuy[index]).c_str(), -1,
			&font, PointF(x + 60, y1 + 3 + index  * dy), pBrush);
    }
}


/*
  更新画面
  x,y 起始位置
*/

void CDrawBuySellPanel::Update(Gdiplus::Graphics& gh, int x, int y, const data_http::CHttpDataEntry& data)
{//注意push_back的顺序应该与行显示的顺序一致
    using namespace std;
    vector<string> v;
	v.push_back(data.open);
	v.push_back(data.maxPrice);
	v.push_back(data.minPrice);
	v.push_back(data.close);
	v.push_back(data.price);
	v.push_back(data.buyPrice01);
	v.push_back(data.sellPrice01);
	v.push_back(data.buyCount01);
	v.push_back(data.sellCount01);
	v.push_back(data.settlementprice);
	v.push_back(data.presettlementprice);
	v.push_back(data.openinstrest);
	v.push_back(data.volumevalue);
    Update(gh, x, y, v, data.open);
}
