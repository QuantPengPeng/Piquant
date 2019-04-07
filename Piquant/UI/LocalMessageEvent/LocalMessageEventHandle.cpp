#include "stdafx.h"
#include "LocalMessageEventHandle.h"


void SqlDataStore_BaseData(WPARAM wParam, LPARAM lParam)
{
	vector<BaseData>* instruments_list = reinterpret_cast<vector<BaseData>*>(lParam);
	if (instruments_list != NULL)
	{
		for (int i = 0; i < instruments_list->size(); ++i)
		{
			BaseData base_data = (*instruments_list)[i];
			get_global_mgr().GetDataStore()->StoreBaseData(&base_data);
		}
	}
}

