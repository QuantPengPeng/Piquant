#ifndef _IDATASTORE_H_
#define _IDATASTORE_H_

#include "IService.h"
#include "DataTypes.h"
#include "CtpService.h"
#include <memory>
#include "CtpMsgDistributor.h"
using namespace std;

enum StoreDataType
{
	Store_MarketData = 100,
	Store_BaseData=101,
	Store_UserInfo=102,
	Store_Exchange=103
};

class SERVICE_API IDataStore  
{
public:
	IDataStore();
	virtual ~IDataStore();
	virtual bool login(const string& user_name, const string& password, const ServerInfo& bs);
	virtual bool logout();
	virtual bool isLogined();

	virtual bool StoreData(int type, void* param);
	virtual bool StoreMarketData(MarketData* pMarketData);			//存储深度行情
	virtual bool StoreBaseData(BaseData* pBaseData);						//存储合约信息
	virtual bool StoreUserInfo(UserInfo* pUserInfo);							//存储登录过的用户信息
	//...等等，未完待续
	virtual bool request(int request_id, void* param);
	virtual bool LoadMarketData(vector<MarketData>& out_data, int start_time, int end_time);				//取某段时间的数据，并存进vector中
	virtual bool LoadBaseData(BaseData& out_data, std::string instrumentID);																				//取某合约的合约信息

private:
	class DatabaseImpl;
	std::shared_ptr<DatabaseImpl> pImpl;//pImpl为指向DatabaseImpl类的智能指针
};

#endif
