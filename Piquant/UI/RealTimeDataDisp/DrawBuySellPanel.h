#pragma once
#include <vector>

// 前向声明
namespace data_http
{
    class CHttpDataEntry;
}

class CDrawBuySellPanel
{
public:
    explicit CDrawBuySellPanel();
    virtual ~CDrawBuySellPanel();
    void Update(Gdiplus::Graphics& gh, int x, int y, const data_http::CHttpDataEntry& data); //更新报价明细的外部接口

private:
    // 更新画面: 真正的画图,供 public: Update调用
    void Update(Gdiplus::Graphics& gh, int x, int y, const std::vector<std::string>& vSellBuy, const std::string& open);

    // 保存目标
    HDC m_hdc;
};

