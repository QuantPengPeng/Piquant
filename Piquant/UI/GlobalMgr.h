#ifndef _GLOBAL_MGR_H_
#define _GLOBAL_MGR_H_ 

#include "../ServiceLayer/IDataStore.h"
#include "../Tool/minilogger.h"

class IService;//前向声明

class CGlobalMgr
{
protected:
	CGlobalMgr();
	virtual ~CGlobalMgr();

public:
	IService* GetService();
	IDataStore* GetDataStore();				//获取数据库操作对象
	Logger<TextDecorator>* GetLog();  //获取日志记录器
	friend  CGlobalMgr& get_global_mgr();
	bool GetTimeQry_Switch(){ return Timequery_switch; }//获取定时查询的调度切换状态
	void SetStopTimeQry(){ Timequery_switch = false; }
	void SetStartTimeQry(){ Timequery_switch = true; }

	void SetLocalMessageThreadID(DWORD threadID){ ThreadID_LocalMessageThread = threadID; };
	DWORD GetLocalMessageThreadID(){return ThreadID_LocalMessageThread;}

protected:
	IService* m_Service;
	IDataStore* m_DataStore;
	bool Timequery_switch;//定时查询的调度切换状态
	DWORD  ThreadID_LocalMessageThread;	 //本地消息线程ID，供应用程序全局访问
	Logger<TextDecorator>* m_log;  //日志记录器      
};

#endif
