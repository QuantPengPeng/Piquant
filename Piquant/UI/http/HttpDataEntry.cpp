#include "stdafx.h"
#include "HttpDataEntry.h"


namespace data_http
{
    CHttpDataEntry::CHttpDataEntry()
    {
    }


    CHttpDataEntry::~CHttpDataEntry()
    {
    }

    // 拷贝构造
    CHttpDataEntry::CHttpDataEntry(const CHttpDataEntry& rhs)
    {
        if(this != &rhs) {
            this->name = rhs.name; 
			this->datasourceID = rhs.datasourceID;
            this->open = rhs.open; // 今日开盘价
            this->close = rhs.close; // 昨日收盘价
            this->price = rhs.price; // 当前价格
            this->maxPrice = rhs.maxPrice; // 今日最高价
            this->minPrice = rhs.minPrice; // 今日最低价
			this->buyCount01 = rhs.buyCount01; // 买1数量
			this->buyPrice01 = rhs.buyPrice01; // 买1报价
			this->sellCount01 = rhs.sellCount01; // 卖1数量
			this->sellPrice01 = rhs.sellPrice01; // 卖1报价
			this->presettlementprice = rhs.presettlementprice; 
			this->settlementprice = rhs.settlementprice; 
			this->openinstrest = rhs.openinstrest; 
			this->volumevalue = rhs.volumevalue; 
        }
    }
    // 赋值
    CHttpDataEntry& CHttpDataEntry::operator=(const CHttpDataEntry& rhs)
    {
        if(this != &rhs) {
			this->name = rhs.name;
			this->datasourceID = rhs.datasourceID;
			this->open = rhs.open; // 今日开盘价
			this->close = rhs.close; // 昨日收盘价
			this->price = rhs.price; // 当前价格
			this->maxPrice = rhs.maxPrice; // 今日最高价
			this->minPrice = rhs.minPrice; // 今日最低价
			this->buyCount01 = rhs.buyCount01; // 买1数量
			this->buyPrice01 = rhs.buyPrice01; // 买1报价
			this->sellCount01 = rhs.sellCount01; // 卖1数量
			this->sellPrice01 = rhs.sellPrice01; // 卖1报价
			this->presettlementprice = rhs.presettlementprice;
			this->settlementprice = rhs.settlementprice;
			this->openinstrest = rhs.openinstrest;
			this->volumevalue = rhs.volumevalue;
        }
        return *this;
    }
}

