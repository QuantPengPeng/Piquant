#ifndef _CTPMSG_DISTRIBUTOR_H_
#define _CTPMSG_DISTRIBUTOR_H_

#include "stdafx.h"
#include <vector>
#include"../../sdk/ThostFtdcUserApiStruct.h"
#include "DataTypes.h"
using namespace std;

//观察者
class SERVICE_API CtpMsgReceiver
{
public:
	virtual void OnReceiveData(WPARAM wParam, LPARAM lParam) = 0;
};

//分发者
class SERVICE_API CtpMsgDistributor
{
protected:
	CtpMsgDistributor();
	virtual ~CtpMsgDistributor();
public:
	void OnResponse(int response_id, LPARAM lParam) ;
	void AddReceiver(CtpMsgReceiver* receiver);
	void DelReceiver(CtpMsgReceiver* receiver);
	friend SERVICE_API CtpMsgDistributor*  get_CtpMsgDistributor();//友元函数
protected:
	vector<CtpMsgReceiver*> receivers;
};
#endif
