
// Doc.h : CDoc 类的接口
//


#pragma once
#include "../ServiceLayer/CtpService.h"
#include "MarketDataView/ConcurrentQueue.h"
#include "../ServiceLayer/DataTypes.h"
#include "../ServiceLayer/CtpMsgDistributor.h"

class CDoc : public CDocument, public CtpMsgReceiver
{
protected: // 仅从序列化创建
	CDoc();
	DECLARE_DYNCREATE(CDoc)

// 操作
public:
	CFrameWnd* CreateNewWindow(CDocTemplate* pTemplate, CDocument* pDocument);
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam);

//数据中心
	ConcurrentQueue<MarketData> DataQueue; //线程安全的数据队列
	HANDLE  hMonitorThread;   //监听线程句柄
	DWORD  MonitorThreadID;  //监听线程ID

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

//监听线程函数
void MonitorThreadFunc(CDoc* pDoc);