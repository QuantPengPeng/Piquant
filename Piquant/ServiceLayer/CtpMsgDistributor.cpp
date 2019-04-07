#include "stdafx.h"
#include "CtpMsgDistributor.h"

CtpMsgDistributor::CtpMsgDistributor()
{
}


CtpMsgDistributor::~CtpMsgDistributor()
{
}

void CtpMsgDistributor::AddReceiver(CtpMsgReceiver* receiver)
{
	vector<CtpMsgReceiver*>::iterator it = std::find(receivers.begin(), receivers.end(), receiver);
	if (it == receivers.end())
	{//若等于超尾迭代器,说明没找到,才压入接收器容器中
		receivers.push_back(receiver);
	}
}

void CtpMsgDistributor::DelReceiver(CtpMsgReceiver* receiver)
{
	//将某个接收容器从容器集合中剔除出去
	for (vector<CtpMsgReceiver*>::iterator it = receivers.begin(); it != receivers.end(); ++it)
	{
		if (*it == receiver)//若地址相同
		{
			it = receivers.erase(it);//根据指定的迭代器,删除指定的接收者
			break;
		}
	}
}

void  CtpMsgDistributor::OnResponse(int response_id, LPARAM lParam)
{
	//对receivers这个vector容器中的所有接收者都调用各自重载的OnReceiveData函数来传递数据，当然数据是从这里给与，但是OnReceiveData重载函数的版本与具体的接收类对象有关；
	for (int i = 0; i<receivers.size(); ++i)
	{
		if (NULL != receivers[i])
		{
			receivers[i]->OnReceiveData(response_id, lParam);
		}
	}
}

CtpMsgDistributor* get_CtpMsgDistributor()
{
	static CtpMsgDistributor* g_CtpMsgDistributor = NULL;
	if (NULL == g_CtpMsgDistributor)
	{
		g_CtpMsgDistributor = new CtpMsgDistributor();
	}
	return g_CtpMsgDistributor;
}
