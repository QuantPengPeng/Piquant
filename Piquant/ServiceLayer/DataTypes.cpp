#include "stdafx.h"
#include "DataTypes.h"
#include "../../sdk/ThostFtdcUserApiDataType.h"

vector<BaseData> instruments_data; //全局变量，靠回调函数赋值
int GetInstrumentVolumeMultiple(string instrumentID) //获取特定的合约乘数
{
	if (instruments_data.empty() != true)
	{//要求不为空
		for (vector<BaseData>::iterator iter = instruments_data.begin(); iter != instruments_data.end(); iter++)
		{
			string vector_instrumentID = (*iter).code;
			if (vector_instrumentID == instrumentID)
			{
				return (*iter).VolumeMultiple;
			}
		}
		return 0; //找不到则返回0，不过不可能找不到
	}
}




string GetTextById(UserInfo& ui, int field_id)//根据字段ID来从UserInfo用户信息结构体中获取具体值
{
	switch (field_id)
	{
	case FIELD_ID_UserInfo::FIELD_UserID:
			return ui.UserID;
			break;
	case FIELD_ID_UserInfo::FIELD_TradingDay:
			return ui.TradingDay;
			break;
	case FIELD_ID_UserInfo::FIELD_LoginTime:
			return ui.LoginTime;
			break;
	case FIELD_ID_UserInfo::FIELD_BrokerID:
			return ui.BrokerID;
			break;
	case FIELD_ID_UserInfo::FIELD_SystemName:
			return ui.SystemName;
			break;
	case FIELD_ID_UserInfo::FIELD_FrontID:
			return to_string(ui.FrontID);
			break;
	case FIELD_ID_UserInfo::FIELD_SessionID:
			return to_string(ui.SessionID);
			break;
	case FIELD_ID_UserInfo::FIELD_MaxOrderRef:
			return ui.MaxOrderRef;
			break;
	case FIELD_ID_UserInfo::FIELD_ExchangeTime:
			return ui.ExchangeTime;
			break;
	default:
			return "--";
			break;
	}
}



string SERVICE_API GetTextById(BaseData& bd, int field_id)
{
	switch (field_id)
	{
	case FIELD_ID_BaseData::FIELD_Code:
		return bd.code;
		break;
	case FIELD_ID_BaseData::FIELD_ExchangeID :
		return bd.ExchangeID;
		break;
	case FIELD_ID_BaseData::FIELD_InstrumentName:
		return bd.InstrumentName;
		break;
	case FIELD_ID_BaseData::FIELD_ExchangeInstID:
		return bd.ExchangeInstID;
		break;
	case FIELD_ID_BaseData::FIELD_ProductID:
		return bd.ProductID;
		break;
	case FIELD_ID_BaseData::FIELD_ProductClass:
		switch (bd.ProductClass)
		{
			case THOST_FTDC_PC_Futures:
				return "期货";
				break;
			case THOST_FTDC_PC_Options:
				return "期货期权";
				break;
			case THOST_FTDC_PC_Combination:
				return "组合";
				break;
			case THOST_FTDC_PC_Spot:
				return "即期";
				break;
			case THOST_FTDC_PC_EFP:
				return "期转现";
				break;
			case THOST_FTDC_PC_SpotOption:
				return "现货期权";
				break;
			default:
				return "---";
				break;
		}
	case FIELD_ID_BaseData::FIELD_DeliveryYear:
		return to_string(bd.DeliveryYear);
		break;
	case FIELD_ID_BaseData::FIELD_DeliveryMonth:
		return to_string(bd.DeliveryMonth);
		break;
	case FIELD_ID_BaseData::FIELD_MaxMarketOrderVolume:
		return to_string(bd.MaxMarketOrderVolume);
		break;
	case FIELD_ID_BaseData::FIELD_MinMarketOrderVolume:
		return to_string(bd.MinMarketOrderVolume);
		break;
	case FIELD_ID_BaseData::FIELD_MaxLimitOrderVolume:
		return to_string(bd.MaxLimitOrderVolume);
		break;
	case FIELD_ID_BaseData::FIELD_MinLimitOrderVolume:
		return to_string(bd.MinLimitOrderVolume);
		break;
	case FIELD_ID_BaseData::FIELD_VolumeMultiple:
		return to_string(bd.VolumeMultiple);
		break;
	case FIELD_ID_BaseData::FIELD_PriceTick:
		return to_string(bd.PriceTick);
		break;
	case FIELD_ID_BaseData::FIELD_CreateDate:
		return bd.CreateDate;
		break;
	case FIELD_ID_BaseData::FIELD_OpenDate:
		return bd.OpenDate;
		break;
	case FIELD_ID_BaseData::FIELD_ExpireDate:
		return bd.ExpireDate;
		break;
	case FIELD_ID_BaseData::FIELD_StartDelivDate:
		return bd.StartDelivDate;
		break;
	case FIELD_ID_BaseData::FIELD_EndDelivDate:
		return bd.EndDelivDate;
		break;
	case FIELD_ID_BaseData::FIELD_InstLifePhase:
		switch (bd.InstLifePhase)
		{
			case THOST_FTDC_IP_NotStart:
				return "未上市";
				break;
			case THOST_FTDC_IP_Started:
				return "上市";
				break;
			case THOST_FTDC_IP_Pause:
				return "停牌";
				break;
			case THOST_FTDC_IP_Expired:
				return "到期";
				break;
			default:
				return "---";
				break;
		}
	case FIELD_ID_BaseData::FIELD_IsTrading:
		if (bd.IsTrading == 0) return "否";
		if (bd.IsTrading != 0) return "是";
		break;
	case FIELD_ID_BaseData::FIELD_PositionType:
		switch (bd.PositionType)
		{
			case THOST_FTDC_PT_Net:
				return "净持仓";
				break;
			case THOST_FTDC_PT_Gross:
				return "综合持仓";
				break;
			default:
				return "---";
				break;
		}
	case FIELD_ID_BaseData::FIELD_PositionDateType:
		switch (bd.PositionDateType)
		{
			case THOST_FTDC_PDT_UseHistory:
				return "使用历史持仓";
				break;
			case THOST_FTDC_PDT_NoUseHistory:
				return "不使用历史持仓";
				break;
			default:
				return "---";
				break;
		}
	case FIELD_ID_BaseData::FIELD_LongMarginRatio:
		return to_string(bd.LongMarginRatio);
		break;
	case FIELD_ID_BaseData::FIELD_ShortMarginRatio:
		return to_string(bd.ShortMarginRatio);
		break;
	case FIELD_ID_BaseData::FIELD_MaxMarginSideAlgorithm:
		if (bd.MaxMarginSideAlgorithm == 0) return "否";
		if (bd.MaxMarginSideAlgorithm != 0) return "是";
		break;
	case FIELD_ID_BaseData::FIELD_UnderlyingInstrID:
		return bd.UnderlyingInstrID;
		break;
	case FIELD_ID_BaseData::FIELD_StrikePrice:
		return to_string(bd.StrikePrice);
		break;
	case FIELD_ID_BaseData::FIELD_OptionsType:
		switch (bd.OptionsType)
		{
			case THOST_FTDC_CP_CallOptions:
				return "看涨";
				break;
			case THOST_FTDC_CP_PutOptions:
				return "看跌";
				break;
			default:
				return "---";
				break;
		}
	case FIELD_ID_BaseData::FIELD_UnderlyingMultiple:
		return to_string(bd.UnderlyingMultiple);
		break;
	case FIELD_ID_BaseData::FIELD_CombinationType:
		switch (bd.CombinationType)
		{
			case THOST_FTDC_COMBT_Future:
				return "期货组合";
				break;
			case THOST_FTDC_COMBT_BUL:
				return "垂直价差BUL";
				break;
			case THOST_FTDC_COMBT_BER:
				return "垂直价差BER";
				break;
			case THOST_FTDC_COMBT_STD:
				return "跨式组合";
				break;
			case THOST_FTDC_COMBT_STG:
				return "宽跨式组合";
				break;
			case THOST_FTDC_COMBT_PRT:
				return "备兑组合";
				break;
			case THOST_FTDC_COMBT_CLD:
				return "时间价差组合";
				break;
			default:
				return "---";
				break;
		}
	default:
		return "--";
		break;
	}
}



string SERVICE_API GetTextById(InvestorPosition& ip, int field_id)
{
	switch (field_id)
	{
	case FIELD_ID_InvestorPosition::FIELD_InstrumentID_Position:
		return ip.InstrumentID;
		break;
	case FIELD_ID_InvestorPosition::FIELD_BrokerID_Position:
		return ip.BrokerID;
		break;
	case FIELD_ID_InvestorPosition::FIELD_InvestorID_Position:
		return ip.InvestorID;
		break;
	case FIELD_ID_InvestorPosition::FIELD_PosiDirection_Position:
	{
		switch (ip.PosiDirection)
		{
		case THOST_FTDC_PD_Net:
			return"净";
			break;
		case THOST_FTDC_PD_Long:
			return"多";
			break;
		case THOST_FTDC_PD_Short:
			return "空";
			break;
		default:
			return"---";
			break;
		}
	}
	case FIELD_ID_InvestorPosition::FIELD_HedgeFlag_Position:
	{
		switch (ip.HedgeFlag)
		{
		case THOST_FTDC_HF_Speculation:
			return "投机";
			break;
		case THOST_FTDC_HF_Arbitrage:
			return "套利";
			break;
		case THOST_FTDC_HF_Hedge:
			return "套保";
			break;
		case THOST_FTDC_HF_MarketMaker:
			return "做市商";
			break;
		default:
			return "---";
			break;
		}
	}
	case FIELD_ID_InvestorPosition::FIELD_PositionDate_Position:
	{
		switch (ip.PositionDate)
		{
		case THOST_FTDC_PSD_Today:
			return"今日持仓";
			break;
		case THOST_FTDC_PSD_History:
			return "历史持仓";
			break;
		default:
			return "---";
			break;
		}
	}
	case FIELD_ID_InvestorPosition::FIELD_YdPosition_Position:
		return to_string(ip.YdPosition);
		break;
	case FIELD_ID_InvestorPosition::FIELD_Position_Position:
		return to_string(ip.Position);
		break;
	case FIELD_ID_InvestorPosition::FIELD_LongFrozen_Position:
		return to_string(ip.LongFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_ShortFrozen_Position:
		return to_string(ip.ShortFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_LongFrozenAmount_Position:
		return to_string(ip.LongFrozenAmount);
		break;
	case FIELD_ID_InvestorPosition::FIELD_ShortFrozenAmount_Position:
		return to_string(ip.ShortFrozenAmount);
		break;
	case FIELD_ID_InvestorPosition::FIELD_OpenVolume_Position:
		return to_string(ip.OpenVolume);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CloseVolume_Position:
		return to_string(ip.CloseVolume);
		break;
	case FIELD_ID_InvestorPosition::FIELD_OpenAmount_Position:
		return to_string(ip.OpenAmount);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CloseAmount_Position:
		return to_string(ip.CloseAmount);
		break;
	case FIELD_ID_InvestorPosition::FIELD_PositionCost_Position:
		return to_string(ip.PositionCost);
		break;
	case FIELD_ID_InvestorPosition::FIELD_PreMargin_Position:
		return to_string(ip.PreMargin);
		break;
	case FIELD_ID_InvestorPosition::FIELD_UseMargin_Position:
	{
		char usemargin[100];
		sprintf(usemargin, "%0.2f", ip.UseMargin); //截取为两位精度后转化为字符串类型
		return usemargin;
		break;
	}
	case FIELD_ID_InvestorPosition::FIELD_FrozenMargin_Position:
		return to_string(ip.FrozenMargin);
		break;
	case FIELD_ID_InvestorPosition::FIELD_FrozenCash_Position:
		return to_string(ip.FrozenCash);
		break;
	case FIELD_ID_InvestorPosition::FIELD_FrozenCommission_Position:
		return to_string(ip.FrozenCommission);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CashIn_Position:
		return to_string(ip.CashIn);
		break;
	case FIELD_ID_InvestorPosition::FIELD_Commission_Position:
		return to_string(ip.Commission);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CloseProfit_Position:
		return to_string(ip.CloseProfit);
		break;
	case FIELD_ID_InvestorPosition::FIELD_PositionProfit_Position:
	{
		char positionprofit[100];
		sprintf(positionprofit, "%0.2f", ip.PositionProfit); //截取为两位精度后转化为字符串类型
		return positionprofit;
		break;
	}
	case FIELD_ID_InvestorPosition::FIELD_PreSettlementPrice_Position:
		return to_string(ip.PreSettlementPrice);
		break;
	case FIELD_ID_InvestorPosition::FIELD_SettlementPrice_Position:
	{
		char settlementprice[100];
		sprintf(settlementprice, "%0.2f", ip.SettlementPrice); //截取为两位精度后转化为字符串类型
		return settlementprice;
		break;
	}
	case FIELD_ID_InvestorPosition::FIELD_TradingDay_Position:
		return ip.TradingDay;
		break;
	case FIELD_ID_InvestorPosition::FIELD_SettlementID_Position:
		return to_string(ip.SettlementID);
		break;
	case FIELD_ID_InvestorPosition::FIELD_OpenCost_Position:
		return to_string(ip.OpenCost);
		break;
	case FIELD_ID_InvestorPosition::FIELD_ExchangeMargin_Position:
		return to_string(ip.ExchangeMargin);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CombPosition_Position:
		return to_string(ip.CombPosition);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CombLongFrozen_Position:
		return to_string(ip.CombLongFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CombShortFrozen_Position:
		return to_string(ip.CombShortFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CloseProfitByDate_Position:
		return to_string(ip.CloseProfitByDate);
		break;
	case FIELD_ID_InvestorPosition::FIELD_CloseProfitByTrade_Position:
		return to_string(ip.CloseProfitByTrade);
		break;
	case FIELD_ID_InvestorPosition::FIELD_TodayPosition_Position:
		return to_string(ip.TodayPosition);
		break;
	case FIELD_ID_InvestorPosition::FIELD_MarginRateByMoney_Position:
		return to_string(ip.MarginRateByMoney);
		break;
	case FIELD_ID_InvestorPosition::FIELD_MarginRateByVolume_Position:
		return to_string(ip.MarginRateByVolume);
		break;
	case FIELD_ID_InvestorPosition::FIELD_StrikeFrozen_Position:
		return to_string(ip.StrikeFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_StrikeFrozenAmount_Position:
		return to_string(ip.StrikeFrozenAmount);
		break;
	case FIELD_ID_InvestorPosition::FIELD_AbandonFrozen_Position:
		return to_string(ip.AbandonFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_ExchangeID_Position:
		return ip.ExchangeID;
		break;
	case FIELD_ID_InvestorPosition::FIELD_YdStrikeFrozen_Position:
		return to_string(ip.YdStrikeFrozen);
		break;
	case FIELD_ID_InvestorPosition::FIELD_InvestUnitID_Position:
		return ip.InvestUnitID;
		break;
	case FIELD_ID_InvestorPosition::FIELD_FloatProfit_Position:
	{
		if (GetInstrumentVolumeMultiple(ip.InstrumentID) != 0)
		{
			double FloatProfit = (ip.SettlementPrice - ip.OpenCost / GetInstrumentVolumeMultiple(ip.InstrumentID)/ip.Position) * ip.Position *GetInstrumentVolumeMultiple(ip.InstrumentID);
			char floatprofit[100];
			if (ip.PosiDirection == THOST_FTDC_PD_Long) //即多头
			{
				sprintf(floatprofit, "%0.2f", FloatProfit); //截取为两位精度后转化为字符串类型
				return floatprofit;
			}
			if (ip.PosiDirection == THOST_FTDC_PD_Short) //即空头
			{
				sprintf(floatprofit, "%0.2f", -1 * FloatProfit); //截取为两位精度后转化为字符串类型
				return floatprofit;
			}
			break;
		}
		else
		{
			return "---";
			break;
		}
		/*
		多头浮动盈亏=（本次结算价-开仓成本/合约乘数/手数）*手数*合约乘数
		空头浮动盈亏=-1*（本次结算价-开仓成本/合约乘数/手数）*手数*合约乘数
		*/
	}
	case FIELD_ID_InvestorPosition::FIELD_AvgPrice_Position:
	{
		if (GetInstrumentVolumeMultiple(ip.InstrumentID) != 0)
		{
			char avgprice[100];
			sprintf(avgprice, "%0.2f", ip.OpenCost / GetInstrumentVolumeMultiple(ip.InstrumentID)/ip.Position); //截取为两位精度后转化为字符串类型
			return avgprice;
			break;
		}
		else
		{
			return "---";
			break;
		}
		/*
		开仓均价=开仓成本/合约乘数/手数
		*/
	}
	default:
		return "---";
		break;
	}
}


string SERVICE_API GetTextById(Order& o, int field_id)
{
	switch (field_id)
	{
	case FIELD_BrokerID_Order:
		return o.BrokerID;
		break;
	case FIELD_InvestorID_Order:
		return o.InvestorID;
		break;
	case FIELD_InstrumentID_Order:
		return o.InstrumentID;
		break;
	case FIELD_OrderRef_Order:
		return o.OrderRef;
		break;
	case FIELD_UserID_Order:
		return o.UserID;
		break;
	case FIELD_OrderPriceType_Order:
	{
		if (o.OrderPriceType == THOST_FTDC_OPT_AnyPrice)	return"市价";
		else if (o.OrderPriceType == THOST_FTDC_OPT_LimitPrice) return "限价";
		else return"---";
		break;
	}
	case FIELD_Direction_Order:
	{
		if (o.Direction == THOST_FTDC_D_Buy) return "多";
		else if (o.Direction == THOST_FTDC_D_Sell) return "空";
		else return "---";
		break;
	}
	case FIELD_CombOffsetFlag_Order:
	{
		if (o.CombOffsetFlag[0] == '0')	return "开仓";
		else if (o.CombOffsetFlag[0] == '1')	return "平仓";
		else if (o.CombOffsetFlag[0] == '3')	return "平今";
		else return "---";
		break;
	}
	case FIELD_CombHedgeFlag_Order:
		return (char*)o.CombHedgeFlag[0];
		break;
	case FIELD_LimitPrice_Order:
	{
		char LimitPrice[100];
		sprintf(LimitPrice, "%0.2f", o.LimitPrice); //截取为两位精度后转化为字符串类型
		return LimitPrice;
		break;
	}
	case FIELD_VolumeTotalOriginal_Order:
		return to_string(o.VolumeTotalOriginal);
		break;
	case FIELD_TimeCondition_Order:
		return (char*)o.TimeCondition;
		break;
	case FIELD_GTDDate_Order:
		return o.GTDDate;
		break;
	case FIELD_VolumeCondition_Order:
	{
		if (o.VolumeCondition == THOST_FTDC_VC_AV) return"任意数量";
		else if (o.VolumeCondition == THOST_FTDC_VC_MV) return"最小数量";
		else if (o.VolumeCondition == THOST_FTDC_VC_CV) return"全部数量";
		else return "---";
		break;
	}
	case FIELD_MinVolume_Order:
		return to_string(o.MinVolume);
		break;
	case FIELD_ContingentCondition_Order:
	{
		if (o.ContingentCondition == THOST_FTDC_CC_Immediately)  return "立即";
		else if (o.ContingentCondition == THOST_FTDC_CC_Touch) return "止损";
		else if (o.ContingentCondition == THOST_FTDC_CC_TouchProfit) return "止盈";
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice) return "向上突破";
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice) return "向下突破";
		else return "---";
		break;
	}
	case FIELD_StopPrice_Order:
	{
		char StopPrice[100];
		sprintf(StopPrice, "%0.2f", o.StopPrice); //截取为两位精度后转化为字符串类型
		return StopPrice;
		break;
	}
	case FIELD_ForceCloseReason_Order:
		return (char*)o.ForceCloseReason;
		break;
	case FIELD_IsAutoSuspend_Order:
		return (char*)o.IsAutoSuspend;
		break;
	case FIELD_BusinessUnit_Order:
		return o.BusinessUnit;
		break;
	case FIELD_RequestID_Order:
		return to_string(o.RequestID);
		break;
	case FIELD_OrderLocalID_Order:
		return o.OrderLocalID;
		break;
	case FIELD_ExchangeID_Order:
		return o.ExchangeID;
		break;
	case FIELD_ParticipantID_Order:
		return o.ParticipantID;
		break;
	case FIELD_ClientID_Order:
		return o.ClientID;
		break;
	case FIELD_ExchangeInstID_Order:
		return o.ExchangeInstID;
		break;
	case FIELD_TraderID_Order:
		return o.TraderID;
		break;
	case FIELD_InstallID_Order:
		return to_string(o.InstallID);
		break;
	case FIELD_OrderSubmitStatus_Order:
	{
		if (o.OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted) return "已经提交";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_CancelSubmitted) return "撤单已经提交";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_ModifySubmitted) return "修改已经提交";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_Accepted) return "已经接受";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected) return "报单已经被拒绝";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected) return "撤单已经被拒绝";
		else if (o.OrderSubmitStatus == THOST_FTDC_OSS_ModifyRejected) return "改单已经被拒绝";
		else return"---";
		break;
	}
	case FIELD_NotifySequence_Order:
		return to_string(o.NotifySequence);
		break;
	case FIELD_TradingDay_Order:
		return o.TradingDay;
		break;
	case FIELD_SettlementID_Order:
		return to_string(o.SettlementID);
		break;
	case FIELD_OrderSysID_Order:
		return o.OrderSysID;
		break;
	case FIELD_OrderSource_Order:
		return (char*)o.OrderSource;
		break;
	case FIELD_OrderStatus_Order:
	{
		if (o.OrderStatus == THOST_FTDC_OST_AllTraded) return "全部成交";
		else if (o.OrderStatus == THOST_FTDC_OST_PartTradedQueueing) return "部分成交还在队列中";
		else if (o.OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing) return "部分成交不在队列中";
		else if (o.OrderStatus == THOST_FTDC_OST_NoTradeQueueing) return "未成交还在队列中";
		else if (o.OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing) return "未成交不在队列中";
		else if (o.OrderStatus == THOST_FTDC_OST_Canceled) return "撤单";
		else if (o.OrderStatus == THOST_FTDC_OST_Unknown) return "未知";
		else if (o.OrderStatus == THOST_FTDC_OST_NotTouched) return "尚未触发";
		else if (o.OrderStatus == THOST_FTDC_OST_Touched) return "已触发";
		else return "---";
		break;
	}
	case FIELD_OrderType_Order:
	{
		if (o.OrderType == THOST_FTDC_ORDT_Normal) return "正常";
		else if (o.OrderType == THOST_FTDC_ORDT_DeriveFromQuote) return "报价衍生";
		else if (o.OrderType == THOST_FTDC_ORDT_DeriveFromCombination) return "组合衍生";
		else if (o.OrderType == THOST_FTDC_ORDT_Combination) return "组合报单";
		else if (o.OrderType == THOST_FTDC_ORDT_ConditionalOrder) return "条件单";
		else if (o.OrderType == THOST_FTDC_ORDT_Swap) return "互换单";
		else return "---";
		break;
	}
	case FIELD_VolumeTraded_Order:
		return to_string(o.VolumeTraded);
		break;
	case FIELD_VolumeTotal_Order:
		return to_string(o.VolumeTotal);
		break;
	case FIELD_InsertDate_Order:
		return o.InsertDate;
		break;
	case FIELD_InsertTime_Order:
		return o.InsertTime;
		break;
	case FIELD_ActiveTime_Order:
		return o.ActiveTime;
		break;
	case FIELD_SuspendTime_Order:
		return o.SuspendTime;
		break;
	case FIELD_UpdateTime_Order:
		return o.UpdateTime;
		break;
	case FIELD_CancelTime_Order:
		return o.CancelTime;
		break;
	case FIELD_ActiveTraderID_Order:
		return o.ActiveTraderID;
		break;
	case FIELD_ClearingPartID_Order:
		return o.ClearingPartID;
		break;
	case FIELD_SequenceNo_Order:
		return to_string(o.SequenceNo);
		break;
	case FIELD_FrontID_Order:
		return to_string(o.FrontID);
		break;
	case FIELD_SessionID_Order:
		return to_string(o.SessionID);
		break;
	case FIELD_UserProductInfo_Order:
		return o.UserProductInfo;
		break;
	case FIELD_StatusMsg_Order:
		return o.StatusMsg;
		break;
	case FIELD_UserForceClose_Order:
		return to_string(o.UserForceClose);
		break;
	case FIELD_ActiveUserID_Order:
		return o.ActiveUserID;
		break;
	case FIELD_BrokerOrderSeq_Order:
		return to_string(o.BrokerOrderSeq);
		break;
	case FIELD_RelativeOrderSysID_Order:
		return o.RelativeOrderSysID;
		break;
	case FIELD_ZCETotalTradedVolume_Order:
		return to_string(o.ZCETotalTradedVolume);
		break;
	case FIELD_IsSwapOrder_Order:
		return to_string(o.IsSwapOrder);
		break;
	case FIELD_BranchID_Order:
		return o.BranchID;
		break;
	case FIELD_InvestUnitID_Order:
		return o.InvestUnitID;
		break;
	case FIELD_AccountID_Order:
		return o.AccountID;
		break;
	case FIELD_CurrencyID_Order:
		return o.CurrencyID;
		break;
	case FIELD_IPAddress_Order:
		return o.IPAddress;
		break;
	case FIELD_MacAddress_Order:
		return o.MacAddress;
		break;
	case FIELD_OrderLocalType_Order:
	{
		if (o.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice && o.Direction == THOST_FTDC_D_Buy && o.CombOffsetFlag[0] == '0')
		{
			return "BuyStop";
			break;
		}
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice && o.Direction == THOST_FTDC_D_Sell  && o.CombOffsetFlag[0] == '0')
		{
			return "SellLimit";
			break;
		}
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice && o.Direction == THOST_FTDC_D_Buy && o.CombOffsetFlag[0] == '0')
		{
			return "BuyLimit";
			break;
		}
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice && o.Direction == THOST_FTDC_D_Sell  && o.CombOffsetFlag[0] == '0')
		{
			return "SellStop";
			break;
		}
		else if (o.ContingentCondition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice || o.ContingentCondition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice)
		{
			if (o.CombOffsetFlag[0] == '1' || o.CombOffsetFlag[0] == '3' )//平仓及平今
			{//暂时简化，不区分止盈及止损
				return "ProfitLoss";
				break;
			}
		}
		else
		{
			return "---";
		}
	}
	default:
		break;
	}
}


string SERVICE_API GetTextById(Trade& t, int field_id)
{
	switch (field_id)
	{
	case FIELD_BrokerID_Trade:
		return t.BrokerID;
		break;
	case FIELD_InvestorID_Trade:
		return t.InvestorID;
		break;
	case FIELD_InstrumentID_Trade:
		return t.InstrumentID;
		break;
	case FIELD_OrderRef_Trade:
		return t.OrderRef;
		break;
	case FIELD_UserID_Trade:
		return t.UserID;
		break;
	case FIELD_ExchangeID_Trade:
		return t.ExchangeID;
		break;
	case FIELD_TradeID_Trade:
		return t.TradeID;
		break;
	case FIELD_Direction_Trade:
	{
		switch (t.Direction)
		{
			case THOST_FTDC_D_Buy:
				return "多";
				break;
			case THOST_FTDC_D_Sell:
				return "空";
				break;
			default:
				return "---";
				break;
		}
	}
	case FIELD_OrderSysID_Trade:
		return t.OrderSysID;
		break;
	case FIELD_ParticipantID_Trade:
		return t.ParticipantID;
		break;
	case FIELD_ClientID_Trade:
		return t.ClientID;
		break;
	case FIELD_TradingRole_Trade:
	{
		switch (t.TradingRole)
		{
			case THOST_FTDC_ER_Broker:
				return "代理";
				break;
			case THOST_FTDC_ER_Host:
				return "自营";
				break;
			case THOST_FTDC_ER_Maker:
				return "做市商";
				break;
			default:
				return "---";
				break;
		}
	}
	case FIELD_ExchangeInstID_Trade:
		return t.ExchangeInstID;
		break;
	case FIELD_OffsetFlag_Trade:
	{
		switch (t.OffsetFlag)
		{
			case THOST_FTDC_OF_Open:
				return "开仓";
				break;
			case THOST_FTDC_OF_Close:
				return "平仓";
				break;
			case THOST_FTDC_OF_ForceClose:
				return "强平";
				break;
			case THOST_FTDC_OF_CloseToday:
				return "平今";
				break;
			case THOST_FTDC_OF_CloseYesterday:
				return "平昨";
				break;
			case THOST_FTDC_OF_ForceOff:
				return "强减";
				break;
			case THOST_FTDC_OF_LocalForceClose:
				return "本地强平";
				break;
			default:
				return "---";
				break;
		}
	}
	case FIELD_HedgeFlag_Trade:
	{
		switch (t.HedgeFlag)
		{
			case THOST_FTDC_HF_Speculation:
				return "投机";
				break;
			case THOST_FTDC_HF_Arbitrage:
				return "套利";
				break;
			case THOST_FTDC_HF_Hedge:
				return "套保";
				break;
			case THOST_FTDC_HF_MarketMaker:
				return "做市商";
				break;
			default:
				return "---";
				break;
		}
	}
		break;
	case FIELD_Price_Trade:
	{
		char Price[100];
		sprintf(Price, "%0.2f", t.Price); //截取为两位精度后转化为字符串类型
		return Price;
		break;
	}	
	case FIELD_Volume_Trade:
		return to_string(t.Volume);
		break;
	case FIELD_TradeDate_Trade:
		return t.TradeDate;
		break;
	case FIELD_TradeTime_Trade:
		return t.TradeTime;
		break;
	case FIELD_TradeType_Trade:
		return (char*)t.TradeType;
		break;
	case FIELD_PriceSource_Trade:
	{
		switch (t.PriceSource)
		{
		case THOST_FTDC_PSRC_LastPrice:
			return "前成交价";
			break;
		case THOST_FTDC_PSRC_Buy:
			return "买委托价";
			break;
		case THOST_FTDC_PSRC_Sell:
			return "卖委托价";
			break;
		default:
			return "---";
			break;
		}
	}
		break;
	case FIELD_TraderID_Trade:
		return t.TraderID;
		break;
	case FIELD_OrderLocalID_Trade:
		return t.OrderLocalID;
		break;
	case FIELD_ClearingPartID_Trade:
		return t.ClearingPartID;
		break;
	case FIELD_BusinessUnit_Trade:
		return t.BusinessUnit;
		break;
	case FIELD_SequenceNo_Trade:
		return to_string(t.SequenceNo);
		break;
	case FIELD_TradingDay_Trade:
		return t.TradingDay;
		break;
	case FIELD_SettlementID_Trade:
		return to_string(t.SettlementID);
		break;
	case FIELD_BrokerOrderSeq_Trade:
		return to_string(t.BrokerOrderSeq);
		break;
	case FIELD_TradeSource_Trade:
		return (char*)t.TradeSource;
		break;
	case FIELD_InvestUnitID_Trade:
		return t.InvestUnitID;
		break;
	default:
		return "---";
		break;
	}
}



string SERVICE_API GetTextById(InvestorPositionDetail& ipd, int field_id)
{
	switch (field_id)
	{
	case FIELD_InstrumentID_PositionDetail:
		return ipd.InstrumentID;
		break;
	case FIELD_BrokerID_PositionDetail:
		return ipd.BrokerID;
		break;
	case FIELD_InvestorID_PositionDetail:
		return ipd.InvestorID;
		break;
	case FIELD_Direction_PositionDetail:
	{
		switch (ipd.Direction)
		{
		case THOST_FTDC_D_Buy:
			return "多";
			break;
		case THOST_FTDC_D_Sell:
			return "空";
			break;
		default:
			return "---";
			break;
		}
	}
	case FIELD_HedgeFlag_PositionDetail:
	{
		switch (ipd.HedgeFlag)
		{
		case THOST_FTDC_HF_Speculation:
			return "投机";
			break;
		case THOST_FTDC_HF_Arbitrage:
			return "套利";
			break;
		case THOST_FTDC_HF_Hedge:
			return "套保";
			break;
		case THOST_FTDC_HF_MarketMaker:
			return "做市商";
			break;
		default:
			return "---";
			break;
		}
	}
	case FIELD_OpenDate_PositionDetail:
		return ipd.OpenDate;
		break;
	case FIELD_TradeID_PositionDetail:
		return ipd.TradeID;
		break;
	case FIELD_Volume_PositionDetail:
		return to_string(ipd.Volume);
		break;
	case FIELD_OpenPrice_PositionDetail:
	{
		char Price[100];
		sprintf(Price, "%0.2f", ipd.OpenPrice); //截取为两位精度后转化为字符串类型
		return Price;
		break;
	}
	case FIELD_TradingDay_PositionDetail:
		return ipd.TradingDay;
		break;
	case FIELD_SettlementID_PositionDetail:
		return to_string(ipd.SettlementID);
		break;
	case FIELD_TradeType_PositionDetail:
	{
		switch (ipd.TradeType)
		{
		case THOST_FTDC_TRDT_SplitCombination:
			return "组合持仓拆分为单一持仓";
			break;
		case THOST_FTDC_TRDT_Common:
			return "普通成交";
			break;
		case THOST_FTDC_TRDT_OptionsExecution:
			return "期权执行";
			break;
		case THOST_FTDC_TRDT_OTC:
			return "OTC成交";
			break;
		case THOST_FTDC_TRDT_EFPDerived:
			return "期转现衍生成交";
			break;
		case THOST_FTDC_TRDT_CombinationDerived:
			return "组合衍生成交";
			break;
		default:
			return "---";
			break;
		}
	}
	case FIELD_CombInstrumentID_PositionDetail:
		return ipd.CombInstrumentID;
		break;
	case FIELD_ExchangeID_PositionDetail:
		return ipd.ExchangeID;
		break;
	case FIELD_CloseProfitByDate_PositionDetail:
	{
		char CloseProfitByDate[100];
		sprintf(CloseProfitByDate, "%0.2f", ipd.CloseProfitByDate); //截取为两位精度后转化为字符串类型
		return CloseProfitByDate;
		break;
	}
	case FIELD_CloseProfitByTrade_PositionDetail:
	{
		char CloseProfitByTrade[100];
		sprintf(CloseProfitByTrade, "%0.2f", ipd.CloseProfitByTrade); //截取为两位精度后转化为字符串类型
		return CloseProfitByTrade;
		break;
	}
	case FIELD_PositionProfitByDate_PositionDetail:
	{
		char PositionProfitByDate[100];
		sprintf(PositionProfitByDate, "%0.2f", ipd.PositionProfitByDate); //截取为两位精度后转化为字符串类型
		return PositionProfitByDate;
		break;
	}
	case FIELD_PositionProfitByTrade_PositionDetail:
	{
		char PositionProfitByTrade_value[100];
		sprintf(PositionProfitByTrade_value, "%0.2f", ipd.PositionProfitByTrade); //截取为两位精度后转化为字符串类型
		return PositionProfitByTrade_value;
		break;
	}
	/*
	逐日盯市平仓盈亏=（平仓价－昨结算价）＊手数＊合约单位　；以开多历史仓为例
	逐笔对冲平仓盈亏=（平仓价-开仓价）*手数*合约单位；			
	逐日盯市持仓盈亏＝（当日结算价－昨结算价）＊手数＊合约单位　；以开多历史仓为例
	逐笔对冲持仓盈亏=（当日结算价-开仓价）*手数*合约单位； 
	*/
	case FIELD_Margin_PositionDetail:
	{
		char Margin[100];
		sprintf(Margin, "%0.2f", ipd.Margin); //截取为两位精度后转化为字符串类型
		return Margin;
		break;
	}
	case FIELD_ExchMargin_PositionDetail:
	{
		char ExchMargin[100];
		sprintf(ExchMargin, "%0.2f", ipd.ExchMargin); //截取为两位精度后转化为字符串类型
		return ExchMargin;
		break;
	}
	case FIELD_MarginRateByMoney_PositionDetail:
	{
		char MarginRateByMoney[100];
		sprintf(MarginRateByMoney, "%0.2f", ipd.MarginRateByMoney); //截取为两位精度后转化为字符串类型
		return MarginRateByMoney;
		break;
	}
	case FIELD_MarginRateByVolume_PositionDetail:
	{
		char MarginRateByVolume[100];
		sprintf(MarginRateByVolume, "%0.2f", ipd.MarginRateByVolume); //截取为两位精度后转化为字符串类型
		return MarginRateByVolume;
		break;
	}
	case FIELD_LastSettlementPrice_PositionDetail:
	{
		char LastSettlementPrice[100];
		sprintf(LastSettlementPrice, "%0.2f", ipd.LastSettlementPrice); //截取为两位精度后转化为字符串类型
		return LastSettlementPrice;
		break;
	}
	case FIELD_SettlementPrice_PositionDetail:
	{
		char SettlementPrice[100];
		sprintf(SettlementPrice, "%0.2f", ipd.SettlementPrice); //截取为两位精度后转化为字符串类型
		return SettlementPrice;
		break;
	}
	case FIELD_CloseVolume_PositionDetail:
	{
		return to_string(ipd.CloseVolume);
		break;
	}
	case FIELD_CloseAmount_PositionDetail:
	{
		char CloseAmount[100];
		sprintf(CloseAmount, "%0.2f", ipd.CloseAmount); //截取为两位精度后转化为字符串类型
		return CloseAmount;
		break;
	}
	case FIELD_InvestUnitID_PositionDetail:
	{
		return ipd.InvestUnitID;
		break;
	}
	default:
		return "---";
		break;
	}
}


