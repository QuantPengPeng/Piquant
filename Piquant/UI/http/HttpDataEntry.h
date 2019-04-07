#pragma once
#include <string>
namespace data_http
{
    using namespace std;
    class CHttpDataEntry
    {
    public:
        CHttpDataEntry();
        ~CHttpDataEntry();

        // 拷贝构造
        CHttpDataEntry(const CHttpDataEntry& rhs);

        // 重载赋值操作符
        CHttpDataEntry& operator=(const CHttpDataEntry& rhs);

    public:
        string name; // 期货名字
		string datasourceID; //数据提供商编号
        string open; // 开盘价
		string maxPrice; //最高价
		string minPrice; //最低价
		string close; // 昨收盘价

		string buyPrice01; // 买1报价
		string sellPrice01; // 卖1报价
		string price; //最新价

		string settlementprice;//结算价
		string presettlementprice;//昨结算
		string buyCount01; // 买1数量
		string sellCount01; // 卖1数量
		string openinstrest;//持仓量
		string volumevalue;//成交量
    };
}



