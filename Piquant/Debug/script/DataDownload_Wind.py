#coding:utf-8
from WindPy import*
from pandas import Series
from pandas import DataFrame
from datetime import datetime

#*****************************************输入参数************************************#
input_symbol='600000.SH'        #选定品种
input_startyear=2018            #年
input_startmonth=9              #月
input_startday=14               #日
input_backdaycount=20         #以年月日为起始点，往前回溯N天提取每天的tick数据
input_datatype=2              #1==tick数据；2==分钟数据；
#*************************************************************************************#

def february_daycount(year):
    daycount = 28 + ((year%4==0 and year%100!=0) or (year%400==0))
    return daycount

def month_daycount(year,month):
    if month==1:
        daycount=31
    elif month==2:
        daycount = february_daycount(year)
    elif month==3:
        daycount =31
    elif month==4:
        daycount = 30
    elif month==5:
        daycount = 31
    elif month==6:
        daycount=30
    elif month==7:
        daycount=31
    elif month==8:
        daycount=31
    elif month==9:
        daycount=30
    elif month==10:
        daycount=31
    elif month==11:
        daycount=30
    else:
        daycount=31
    return daycount

def main():
    w.start()
    if w.isconnected:
        print("连接成功")
    else:
        print("连接失败")
        w.stop()

    dataload_start_dt = datetime(input_startyear,input_startmonth,input_startday,9,30,0)
    dataload_end_dt = datetime(input_startyear,input_startmonth,input_startday,15,0,0)
    startdate = datetime(input_startyear,input_startmonth,input_startday)
    day_flag=input_startday
    day=input_startday
    month_flag=input_startmonth
    month=input_startmonth
    year_flag=input_startyear
    year=input_startyear

    for i in range(input_backdaycount):
        if i==0:
            day=input_startday
        else:
            day=day_flag-1
            day_flag=day_flag-1
            if day==0 and month!=1:
                month=month_flag-1
                month_flag=month_flag-1
                day=month_daycount(year,month)
                day_flag=day
            if day==0 and month==1:
                month=12
                month_flag=12
                year=year_flag-1
                year_flag=year_flag-1
                day = month_daycount(year, month)
                day_flag=day
        if datetime(year,month,day).weekday()+1 ==6 or datetime(year,month,day).weekday()+1 ==7:
            print('{0}为周末，无交易数据'.format(datetime(year,month,day).strftime('%Y%m%d')))
            continue
        dataload_start_dt=dataload_start_dt.replace(year,month,day,9,30,0)
        dataload_end_dt=dataload_end_dt.replace(year,month,day,15,0,0)
        date=startdate.replace(year,month,day)

        str_start_dt = dataload_start_dt.strftime('%Y-%m-%d %H:%M:%S')
        str_end_dt = dataload_end_dt.strftime('%Y-%m-%d %H:%M:%S')
        trading_date = date.strftime('%Y%m%d')
        #print(str_start_dt)
#'''
        if input_datatype==1:
            wstdata=w.wst(input_symbol, "last,ask1,bid1,asize1,bsize1", str_start_dt, str_end_dt, "")
            df = DataFrame(
                {'last':wstdata.Data[0],
                 'ask1': wstdata.Data[1],
                 'bid1': wstdata.Data[2],
                 'asize1': wstdata.Data[3],
                'bsize1': wstdata.Data[4]},
                index=wstdata.Times,
                columns=wstdata.Fields)
        if input_datatype==2:
            wstdata =w.wsi(input_symbol,"open,high,low,close,volume,oi",str_start_dt,str_end_dt)
            df = DataFrame(
                {'open': wstdata.Data[0],
                 'high': wstdata.Data[1],
                 'low': wstdata.Data[2],
                 'close': wstdata.Data[3],
                 'volume': wstdata.Data[4],
                 'oi': wstdata.Data[5]},
                index=wstdata.Times,
                columns=wstdata.Fields)

        csvname='{0}_{1}.csv'.format(input_symbol,trading_date);
        df.to_csv(csvname)
#'''
if __name__ == '__main__':
    main()



