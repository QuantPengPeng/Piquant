#coding:utf-8
#author:QuantPengPeng; datetime:2018/09/16
import tushare as ts
from pandas import Series
from pandas import DataFrame
from datetime import datetime

#*****************************************输入参数************************************#
input_symbol='600000'        #选定品种
input_endyear=2018
input_endmonth=9
input_endday=14
input_backdaycount=10
input_bartype='30'              #5\15\30\60；
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

def StartToEnd(end_year,end_month,end_day,backdaycount):
    month = end_month
    year = end_year
    for i in range(backdaycount):
        if i==0:
            startday=end_day
        else:
            startday=startday-1
            if startday==0 and month!=1:
                month=month-1
                startday=month_daycount(year,month)
            if startday==0 and month==1:
                month=12
                year=year-1
                startday = month_daycount(year, month)
    endday=end_day+1
    startdate=datetime(year,month,startday)
    enddate=datetime(end_year,end_month,endday)
    return startdate,enddate

def DateList(end_year,end_month,end_day,backdaycount):
    datelist=[]
    month = end_month
    year = end_year
    for i in range(backdaycount):
        if i==0:
            startday=end_day
        else:
            startday=startday-1
            if startday==0 and month!=1:
                month=month-1
                startday=month_daycount(year,month)
            if startday==0 and month==1:
                month=12
                year=year-1
                startday = month_daycount(year, month)
        startdate=datetime(year,month,startday)
        datelist.append(startdate)
    return datelist

def main():
    dataload_start_dt =StartToEnd(input_endyear,input_endmonth,input_endday,input_backdaycount)[0]
    dataload_end_dt = StartToEnd(input_endyear,input_endmonth,input_endday,input_backdaycount)[1]
    str_start_dt = dataload_start_dt.strftime('%Y-%m-%d')
    str_end_dt = dataload_end_dt.strftime('%Y-%m-%d')
    df = ts.get_hist_data(input_symbol, str_start_dt, str_end_dt, input_bartype)
    for i in DateList(input_endyear,input_endmonth,input_endday,input_backdaycount):
        if i.weekday()+1 ==6 or i.weekday()+1 ==7:
            print('{0}为周末，无交易数据'.format(i.strftime('%Y%m%d')))
            continue
        match_str=i.strftime('%Y-%m-%d')
        df_split=DataFrame(columns=['open', 'high', 'close', 'low', 'volume', 'price_change', 'p_change',
       'ma5', 'ma10', 'ma20', 'v_ma5', 'v_ma10', 'v_ma20', 'turnover'])
        datetime_list=[]
        count=0
        for k in range(df.shape[0]):
            if match_str in df.index[k]:
                datetime_list.append(df.index[k])
                count=count+1
                df_split=df_split.append(df[k:k+1],ignore_index=True)
        datetime_df=DataFrame({'datetime':datetime_list})
        df_split['datetime']=datetime_df
        df_split=df_split.set_index('datetime')
        df_split=df_split.sort_values('datetime')
        csvname = '{0}_{1}.csv'.format(input_symbol,i.strftime('%Y%m%d'))
        df_split.to_csv(csvname)


if __name__ == '__main__':
    main()
