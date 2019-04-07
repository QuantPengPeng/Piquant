#include "stdafx.h"
#include "ConfigReader.h"
#include "tinyxml\tinystr.h"
#include "tinyxml\tinyxml.h"
#include<vector>


ConfigReader::ConfigReader()
	:document_brokers(NULL), root_brokers(NULL), document_config(NULL), root_config(NULL)
{
		
}
ConfigReader::~ConfigReader()
{
}

ConfigReader& get_config_reader()
{//友元函数
	static ConfigReader g_ConfigReader;
	return g_ConfigReader;
}

bool ConfigReader::load(const string& brokers_file, const string& config_file, bool bForceLoad)
{//bForceLoad是否强制重新配置读写xml的环境
	if (bForceLoad)
	{//若强制重新配置读写xml的环境,则在此之前要先析构旧资源
		unload();
	}
	if (document_brokers != NULL && document_config != NULL)
	{
		return true;
	}
	{//处理brokers.xml
		document_brokers = new TiXmlDocument(brokers_file.c_str());			//创建指向xml文档的指针
		if (NULL == document_brokers){ return false; }									//创建失败返回false
		document_brokers->LoadFile();														//加载xml文档
		root_brokers = document_brokers->RootElement();							//获取根结点
		if (NULL == root_brokers){ return false; }											//获取根结点失败返回false
		LoadBrokers();																					//加载经纪商信息进m_brokers
	}
	{//处理config.xml
		document_config = new TiXmlDocument(config_file.c_str());
		if (NULL == document_config){ return false; }
		document_config->LoadFile();
		root_config = document_config->RootElement();
		if (NULL == root_config){ return false; }
		LoadAllListHeaders();																//加载表头信息进m_all_list_header
		LoadMyInstrumets();																//加载订阅合约信息进m_instruments
	}
	return true;
}

bool ConfigReader::unload()
{
	if (NULL != document_brokers)
	{
		delete document_brokers;
		document_brokers = NULL;
	}
	if (NULL != document_config)
	{
		delete document_config;
		document_config = NULL;
	}
	if (root_brokers != NULL)
	{
		delete root_brokers;
		root_brokers = NULL;
	}
	if (root_config != NULL)
	{
		delete root_config;
		root_config = NULL;
	}
	m_brokers.clear();
	m_all_list_header.clear();
	m_instruments.clear();
	return true;
}

bool ConfigReader::LoadBrokers()
{//此处需要根据xml文件的具体结构来写代码
	//获取完根结点后，开始获取配置文件中的有用信息
	string broker_tag_name = "broker";
	TiXmlElement* elem = root_brokers->FirstChildElement(broker_tag_name.c_str());//从根结点出发，找到"broker"孩子结点
	while (NULL != elem)//若找到相应孩子节点，则陷入循环
	{
		//获取最基本的经纪商信息,如经纪商ID/经纪商名
		string broker_name = GetAttribute(elem, "BrokerName");//获取BrokerName属性值
		ServerInfo si;
		si.name = broker_name;
		si.id = elem->Attribute("BrokerID");
		TiXmlElement* servers_elem = elem->FirstChildElement("Servers");									//往下取broker父节点的Servers子节点
		TiXmlElement* front_server_elem = servers_elem->FirstChildElement("FrontServer");		//再往下取Servers子节点的FrontServer子节点
		TiXmlElement* name_elem = front_server_elem->FirstChildElement("Name");					//再往下取FrontServer子节点的Name子节点
		TiXmlElement* protocol_elem = front_server_elem->FirstChildElement("Protocol");			//再往下取FrontServer子节点的Protocol子节点
		si.protocol = protocol_elem->GetText();//获取对应结点的文本内容,即协议
		TiXmlElement* trading_elem = front_server_elem->FirstChildElement("Trading");				//再往下取FrontServer子节点的Trading子节点
		vector<TiXmlElement*> trading_addrs = GetSubElements(trading_elem);							//或有多个交易前置，因此从trading_elem结点开始获取所有孩子节点，即要获取多个交易前置
		for (int i = 0; i < trading_addrs.size(); ++i)
		{
			si.trade_server_front.push_back(string(trading_addrs[i]->GetText()));
		}
		TiXmlElement* market_elem = front_server_elem->FirstChildElement("MarketData");		//再往下取FrontServer子节点的MarketData子节点
		vector<TiXmlElement*> market_addrs = GetSubElements(market_elem);							//或有多个行情前置
		for (int j = 0; j < market_addrs.size(); ++j)
		{
			si.market_server_front.push_back(string(market_addrs[j]->GetText()));
		}
		m_brokers.insert(make_pair(broker_name, si));																	//将该经纪商信息以broker_name为键, ServerInfo为值压入map
		elem = elem->NextSiblingElement(broker_tag_name.c_str());											//找寻兄弟结点，直到找不到为止
	}
	return true;
}

bool ConfigReader::LoadAllListHeaders()
{//用于获取表头数据
	if (NULL != root_config)
	{
		TiXmlElement* all_list_column_elem = root_config->FirstChildElement("m_ListCtrlColumns");					//由root根节点往下取特定m_ListCtrlColumns孩子结点
		vector<TiXmlElement*> vec_list_column_elem = GetSubElements(all_list_column_elem);				//由m_ListCtrlColumns孩子结点往下取所有孩子结点
		for (int i = 0; i < vec_list_column_elem.size(); ++i)
		{
			TiXmlElement* one_list_column = vec_list_column_elem[i];		//循环获取孩子结点
			string list_name = one_list_column->Attribute("_key");				//获取特定属性值
			vector<TiXmlElement*> one_list_all_columns = GetSubElements(one_list_column);					//进一步获取该孩子结点下的所有孩子结点
			vector<ColumnInfo> one_list_col_vec;										//用于存取属性键值对
			for (int j = 0; j < one_list_all_columns.size(); ++j)
			{
				ColumnInfo ci;
				ci.column_name = one_list_all_columns[j]->Attribute("Name");			//注意区分大小写
				ci.column_width = atoi(one_list_all_columns[j]->Attribute("Width"));
				string id = GetAttribute(one_list_all_columns[j], ("Id"));
				ci.column_id = atoi(id.c_str());
				ci.column_color = 0;																			//xml文档尚未有该属性，因此默认填0
				one_list_col_vec.push_back(ci);
			}
			m_all_list_header.insert(make_pair(list_name, one_list_col_vec));
		}
	}
	return false;
}

bool ConfigReader::LoadMyInstrumets()
{
	string my_instruments_tag_name = "MyInstruments";
	TiXmlElement* elem = root_config->FirstChildElement(my_instruments_tag_name.c_str());	//由root根结点开始获取特定MyInstruments孩子结点
	if (NULL == elem)
	{
		return false;
	}
	vector<TiXmlElement*> instruments_elems = GetSubElements(elem);						//由MyInstruments孩子结点往下取所有孩子结点
	for (int i = 0; i < instruments_elems.size(); ++i)
	{
		m_instruments.push_back(GetInnerText(instruments_elems[i]));								//提取特定结点的文本内容
	}
	return true;
}


//工具函数集
vector<TiXmlElement*> ConfigReader::GetSubElements(TiXmlElement* parent)
{
	vector<TiXmlElement*> sub_elements;
	if (NULL != parent)
	{
		TiXmlElement* elem = parent->FirstChildElement();//从父结点出发，找到下一个孩子结点，当然也可以指明要查找的孩子结点
		while (NULL != elem)//若找到子节点，则陷入循环
		{
			sub_elements.push_back(elem);			//将孩子结点压入vector中
			elem = elem->NextSiblingElement();//找到相应兄弟结点，直到找不到为止
		}
	}
	return sub_elements;
}

std::string ConfigReader::GetAttribute(TiXmlElement* elem, const string& attr_name)
{//由特定结点，根据特定键来获取值
	if (NULL != elem)//检查结点入参是否有效
	{
		const char* attr_val = elem->Attribute(attr_name.c_str());//由相应键来获取值
		if (NULL == attr_val)
		{
			return "";
		}
		return attr_val;
	}
	return "";
}

std::string ConfigReader::GetInnerText(TiXmlElement* elem)
{
	if (NULL != elem)
	{
		const char* val = elem->GetText();
		if (NULL == val)
		{
			return "";
		}
		return val;
	}
	return "";
}


vector<ColumnInfo>& ConfigReader::GetHeaderColumn(const string& header_name)
{
	map<string, vector<ColumnInfo>>::iterator iter = m_all_list_header.find(header_name);
	if (iter != m_all_list_header.end())
	{
		return iter->second;
	}
	return vector<ColumnInfo>();//否则就返回空;
}