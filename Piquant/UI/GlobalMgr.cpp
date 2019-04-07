#include "stdafx.h"
#include "GlobalMgr.h"
#include "../ServiceLayer/IService.h"
#include "../ServiceLayer/CtpService.h"


CGlobalMgr::CGlobalMgr()
	:m_Service(NULL),
	m_DataStore(NULL)
{
	Timequery_switch = true;
}


CGlobalMgr::~CGlobalMgr()
{
	if (NULL != m_Service)
	{
		delete m_Service;
		m_Service = NULL;
	}

	if (NULL != m_DataStore)
	{
		delete m_DataStore;
		m_DataStore = NULL;
	}

	if (NULL != m_log)
	{
		delete m_log;
		m_log = NULL;
	}
}

IService* CGlobalMgr::GetService()
{
	if (NULL == m_Service)
	{
		m_Service = new CtpService();//申请个ctpservice类对象
	}
	return m_Service;
}

IDataStore* CGlobalMgr::GetDataStore()
{
	if (NULL == m_DataStore)
	{
		m_DataStore = new IDataStore();

		//直接在创建的时候登录上数据库
		ServerInfo si;//此处需填充相关的sql server数据库服务器信息
		si.protocol = "SQL Server";//数据库类型
		si.name = "MultiTrader";//ODBC数据源名,请自行定义
		si.id = "MultiTraderDB";//数据库名,请自行定义
		m_DataStore->login("mt", "w123123", si);//填写数据库用户名及密码
	}
	return m_DataStore;
}

Logger<TextDecorator>* CGlobalMgr::GetLog()
{
	if (NULL == m_log)
	{
		m_log = new Logger<TextDecorator>("log", "日志记录", true, true); //创建logger
	}
	return m_log;
}

//此为友元函数
CGlobalMgr& get_global_mgr()
{//创建并返回CGlobalMgr类对象
	static CGlobalMgr g_GlobalMgr;
	return g_GlobalMgr;
}
