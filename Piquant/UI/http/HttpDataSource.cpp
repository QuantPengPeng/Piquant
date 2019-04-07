#include "stdafx.h"

#include "HttpDataSource.h"
#include "HttpHelper.h"
#include "HttpDataEntry.h"
#include <Afx.h>
#include <regex>
#include <iostream>
#include <fstream>

namespace data_http
{

    // 查询 实时数据 URL
    const std::string CHttpDataSource::m_strQueryURL = "http://hq.sinajs.cn/list="; //+TA1905 //来自新浪财经的数据

    // 查询实时报价的URL
	const std::string CHttpDataSource::m_strQueryURLGif = "http://webquotepic.eastmoney.com/GetPic.aspx?id=";  //来自东方财富，前缀

    CHttpDataSource::CHttpDataSource(void)
        : m_http(nullptr)
    {
        Init();
    }


    CHttpDataSource::~CHttpDataSource(void)
    {
        Release();
    }

    // 初始化数据库
    bool CHttpDataSource::Init(void)
    {
        try {
            // http访问工具初始化
            m_http = new CHttpHelper;
            return true;
        }
        catch(const std::bad_alloc& e) {
            TRACE("%s", e.what());
        }
        catch(const std::exception& e) {
            TRACE("%s", e.what());
        }
        catch(...) {
            TRACE("发生未知异常");
        }
        return false;
    }

    // 清理资源
    void CHttpDataSource::Release(void)
    {
        if (m_http!=nullptr)
        {
			delete m_http;
			m_http = nullptr;
        }
    }

    // 查询期货实时数据
    bool CHttpDataSource::QueryStockRealTimeData(const std::string& Code, CHttpDataEntry& data)
    {
        using namespace std;
        //1.获取网页信息
        // 拼接成完整的 URL: http://hq.sinajs.cn/list=TA1905
        std::string strURL = m_strQueryURL;
		strURL.append(Code);
        // 获取数据
        std::string strContent;
        if(!m_http->Get(strURL, strContent)) 
		{
            return false;
        }
        //2.拆分获取,内容
        /* 结果内容: var hq_str_TA1905="PTA1905,150004,6410.00,6428.00,6288.00,6430.00,6294.00,6296.00,6296.00,6366.00,6460.00,204,19,1050908,2120722,郑,PTA,2019-03-18,1,6638.000,6288.000,6848.000,6288.000,6848.000,6288.000,6848.000,5560.000,142.233";
           使用正则表达式进行拆分
           v[0] = "var hq_str_TA1905"
           v[1] = "PTA1905"
           v[2] = "150004"
		   v[3] = "6410.00"
		   v[4] = "6428.00"
		   v[5] = "6288.00"
		   v[6] = "6430.00"
		   v[7] = "6294.00"
		   v[8] = "6294.00"
		   v[9] = "6296.00"
		   v[10] = "6296.00"
		   v[11] = "6366.00"
		   v[12] = "6460.00"
		   v[13] = "204"
		   v[14] = "19"
		   v[15] = "1050908"
		   v[16] = "2120722"
		   v[17] = "郑"
           ...
           */
        regex reg("=|,|;|\""); // 以'=' or ',' or '"' or ';' 作为分割符进行拆分
        sregex_token_iterator it(strContent.begin(), strContent.end(), reg, -1);
        sregex_token_iterator end;
        std::vector<std::string> v;
        while(it != end) 
		{//填充数据进vector
            //提取不为空的项,存储
            if(*it != "") 
			{
                v.push_back(*it);
            }
            ++it;
        }
		if (v.size()<15) //因为只接收15个数据过来，因此判断界限数在15
		{//经检查，新浪财经期货API是有可能返回空数据的，为此必须做检查，否则会报vector相关错误
			return false;
		}
        //3.给返回的结构体赋值
        data.name = v[1]; // 期货名字
		data.datasourceID = v[2]; //数据提供商编号
		data.open = v[3]; // 开盘价
		data.maxPrice = v[4]; //最高价
		data.minPrice = v[5]; //最低价
		data.close = v[6]; // 昨日收盘价
		data.buyPrice01 = v[7]; // 买1报价
		data.sellPrice01 = v[8]; // 卖1报价
		data.price = v[9]; //最新价
		data.settlementprice = v[10];//结算价
		data.presettlementprice = v[11];//昨结算价
		data.buyCount01 = v[12]; // 买1数量
		data.sellCount01 = v[13]; // 卖1数量
		data.openinstrest = v[14]; //持仓量
		data.volumevalue = v[15];//成交量
        // TODO: 检查数据格式是否合法
        return true;
    }
	

	
	 // 下载股票分钟K线图
	 bool CHttpDataSource::QueryStockKLineMin(const std::string& stockCode, const std::string& filename)
	 {
		using namespace std;
		std::string strURL = m_strQueryURLGif; //先填前缀
		strURL.append(stockCode);					 //添加产品ID，请按照东方财富的ID要求
		strURL.append("&imageType=rf&token=44c9d251add88e27b65ed86506f6e5da"); //再填后缀，东方财富此处是数据源的密码之类
		// 获取数据
		std::string content;
		 if (!m_http->Get(strURL, content, nullptr)) 
		 {
			 return false;
		 }
		// 2.保存文件内容到filename指定位置
		try 
		{
			// 如果文件存在,则删除文件重建
			ofstream file(filename, ios::out | ios::binary);
			if(file) 
			{//以二进制数据写gif
				file.write(content.c_str(), content.size());
				file.close();
			}
		}
		catch(const std::exception& e) 
		{
			TRACE("%s", e.what());
		}
		catch(...) 
		{
			TRACE("发生未知异常");
		}
		 return true;
	 }
}
