#ifndef _CONFIG_READER_H
#define _CONFIG_READER_H

#include "stdafx.h"
#include "../ServiceLayer/ServerInfo.h"
#include <map>
#include <vector>
#include <string>
using namespace std;

struct ColumnInfo
{//列头结构体
	int column_id;
	int column_width;
	unsigned int column_color;
	string column_name;
};

class TiXmlDocument;	//前向声明,对应于XML的整个文档
class TiXmlElement;		//前向声明,对应于XML的元素

class TOOL_API ConfigReader
{
protected:
	ConfigReader();
	virtual ~ConfigReader();
	//工具函数,由LoadXXX函数所使用
	vector<TiXmlElement*> GetSubElements(TiXmlElement* parent);			//从xml文档对象的某父节点开始，获取其子节点集合
	string GetAttribute(TiXmlElement* elem, const string& attr_name);		//获取特定结点的特定属性值
	string GetInnerText(TiXmlElement* elem);												//获取特定结点的文本内容
	bool LoadBrokers();				//从xml文档中读取经纪商配置信息,如经纪商ID及前置服务器地址,将这些信息存入m_brokers中
	bool LoadAllListHeaders();		//从xml文档中读取列头信息,如ListCtrl用户信息/ListCtrl报价表等,将这些存入m_all_list_header中
	bool LoadMyInstrumets();		//从xml文档中读取所需订阅合约, 该功能默认弃用
	
protected:
	TiXmlDocument* document_brokers;														//指向xml文档(存储经纪商信息的xml)的指针
	TiXmlElement*     root_brokers;																	//xml根结点指针(存储经纪商信息的xml)
	TiXmlDocument* document_config;															//指向xml文档(存储配置信息的xml)的指针
	TiXmlElement*     root_config;																	//xml根结点指针(存储配置信息的xml)
	map<string, ServerInfo> m_brokers;										//存储经纪商信息的map
	map<string, vector<ColumnInfo>> m_all_list_header;				//存储表头的map
	vector<string> m_instruments;												//存储订阅合约的vector

public:
	friend TOOL_API ConfigReader& get_config_reader();//友元函数

public://外部接口
	bool load(const string& brokers_file = "brokers.xml", const string& config_file="config.xml",  bool bForceLoad = false);		
	bool unload();
	map<string, ServerInfo>&  GetBrokers(){ return m_brokers; }					//获取map<string, ServerInfo> m_brokers
	vector<ColumnInfo>& GetHeaderColumn(const string& header_name); //获取map<string, vector<ColumnInfo>> m_all_list_header中相应header_name的列头信息集合
	vector<string>& GetMyInstruments(){ return m_instruments; }               //获取vector<string> m_instruments
};

#endif