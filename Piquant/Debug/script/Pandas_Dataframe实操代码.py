
# coding: utf-8

# In[83]:


import numpy as np
import pandas as pd
#可通过输入列表字典、元组字典、ndarray字典、序列字典来创建DataFrame
#create from dict of list,字典每个key对应一条序列，也即该序列名，
df_0=pd.DataFrame({'one': [1,2,3],
                  'two':[4,5,6]})
print(df_0)

#create from dict of tuples,字典每个key对应一条序列，也即该序列名，
df_1=pd.DataFrame({'one': (1,2,3),
                  'two':(4,5,6)})
print(df_1)

#create from dict of ndarray,字典每个key对应一条序列，也即该序列名，
df_2=pd.DataFrame({'one': np.array([1,2,3],dtype=np.int32),
                  'two':np.array([4,5,6],dtype=np.int32)})
print(df_2)

#create from dict of series,字典每个key对应一条序列，也即该序列名，
df_3=pd.DataFrame({'one': pd.Series([1,2,3],index=['a','b','c']),
                  'two': pd.Series([4,5,6],index=['a','b','c'])})
print(df_3)
print(df_3.index)
print(df_3.columns)


# In[12]:


#切片索引、修改、添加、删除、转置、子表合并、排序、向量化运算、存储
import numpy as np
import pandas as pd
df_0=pd.DataFrame({'one': pd.Series([1,2,3],index=['a','b','c']),
                  'two': pd.Series([4,5,6],index=['a','b','c'])})
########################索引查找，先列名后序列索引名########################
print(df_0['one'])#获取单列series,获取列直接用标签即可
print(df_0[['one','two']])#获取多列series，入参应为列名列表
print(df_0[0:2])#可采用索引切片获取多行
print(df_0[df_0.index=='a'])#select rows by boolean vector,布尔获取在特殊获取中很重要
print(df_0.iloc[0])#select row by integer location,根据索引获取行序列
print(df_0.loc['a'])#select row by label,根据标签获取单行序列
print(df_0.loc[['a','b']])#获取多行序列，根据行名列表
print(df_0['one'][1])#获得元素
print(df_0.loc[['a','b'],['one','two']])#获取子表，先指定两行再指定两列

########################修改########################
df_0['one']['a']=3#修改元素，逐级查找
df_0['one']=[7,8,9]#修改对应序列
df_0.loc[['a','b'],['one','two']]=pd.DataFrame({'one': pd.Series([10,10],index=['a','b']),'two':pd.Series([10,10],index=['a','b'])})#修改子表
print(df_0)

########################添加########################
#df_0['three']=[7,8,9]#默认在列尾添加
df_0.insert(1,'three',[7,8,9])#在相应位置插入序列
print(df_0)

########################删除########################
#df_temp=df_0.drop(['a','b'],axis=0)#沿行，删除'a'及'b'行
df_temp=df_0.drop(['one','two'],axis=1)#沿列，删除'one'及'two'列
print(df_temp)#drop方法不改动原数据
#del df_0['three']
df_0.pop('three')#采用pop或del会改动原数据

print(df_0)

########################转置########################
print(df_0.T)#整体转置
print(df_0[0:2].T)#选取部分行后，部分转置

########################子表合并########################
df_2=pd.DataFrame({'A':[1,2,3,4],
                  'B':[5,6,7,8],
                  'C':[9,10,11,12],
                  'D':[13,14,15,16]},
                 index=[0,1,2,3])
df_3=pd.DataFrame({'A':[17,18,19,20],
                  'B':[21,22,23,24],
                  'C':[25,26,27,28],
                  'D':[29,30,31,32]},
                 index=[4,5,6,7])
df_4=pd.DataFrame({'A':[33,34,35,36],
                  'B':[37,38,39,40],
                  'C':[41,42,43,44],
                  'D':[45,46,47,48]},
                 index=[8,9,10,11])
df_5=pd.DataFrame({'E':[1,2,3,4],
                  'F':[5,6,7,8],
                  'G':[9,10,11,12],
                  'H':[13,14,15,16]},
                 index=[0,1,2,3])
list_df_0=[df_2,df_3]
print(pd.concat(list_df_0,axis=0))#沿列合并字表，金融数据一般沿列合并数据表，

list_df_1=[df_2,df_5]
print(pd.concat(list_df_1,axis=1))#axis=1为沿行合并子表,一般添加更多属性数据会采用该方式

print(df_2.append([df_3,df_4]))#沿列合并多个子表
print(df_2.append(df_3.iloc[2],ignore_index=True))#append rows to a dataframe,选择忽略原索引

########################排序########################
df_6=pd.DataFrame({'A':[2,1,3,4],
                  'B':[6,5,8,7],
                  'C':[9,10,11,12],
                  'D':[13,14,15,16]},
                 index=[0,1,2,3])
print(df_6.sort_index(axis=0,ascending=False,by='A'))#沿列降序重排DataFrame，以'A'列的值为基准

########################向量化运算########################
index_series=pd.date_range('01/01/2018',periods=6)
df_7=pd.DataFrame({'one': pd.Series([1,2,3,4,5,6],index=index_series),
                  'two':pd.Series([4,5,6,7,8,9],index=index_series)})
print(df_7)
print(df_7**2*2+1)#对dataframe中的每个元素进行算数运算
print(df_7**2-df_7)#可对相同维度的dataframe进行元素间运算，当然即使维度不同，也会自动对齐运算
print(df_7.sub(df_7['one'],axis=0))#通过axis来指定沿列，还是沿行，dataframe各列/各行都相应减去序列series,相减一定要使用sub方法，不用用-号直接相减，会出错
#除了算数向量化运算之外，还可进行逻辑向量化运算，两个dataframe之间可以进行各元素间的逻辑与、逻辑或、逻辑非等操作，

########################存储########################
#csv是最通用的一种文件格式，input可用pd.read_csv(),而output可用pd.to_csv()


# In[81]:


#获取特定分组、通过迭代对不同分组进行操作、数据聚合
import pandas as pd
import numpy as np
from pandas import Series,DataFrame
df = DataFrame({'key1':['a','a','b','b','a'],
               'key2':['one','two','one','two','one'],
               'data1':[1,1,2,2,1],
               'data2':[2,2,2,2,2]})
print(df)#分组的前提是dataframe中除了数据集外，还应该有可用的分组键，其本质是一种分类，

#根据键-值配对获取特定分组（单键模式）
dict_grouped_0=dict(list(df.groupby('key1')))#将dataframe按照key1进行分组切割
print(dict_grouped_0['a'])#根据key1中的不同键来获取相应分组
print(dict_grouped_0['a']['data1'].sum())#获取的分组dict_grouped_0['a']是dataframe格式，可对其进行dataframe操作

#根据键-值配对获取特定分组（多键模式）
dict_grouped_1=dict(list(df.groupby(['key1','key2'])))#将dataframe按照key1及key2进行分组切割
print(dict_grouped_1[('a', 'one')])#根据key1及key2中不同键值配对来获取相应分组


#根据单个分组键来分组，并对不同分组做不同操作
for name,group in df.groupby('key1'):
    if name=='a':
        print(group['data1'].mean())
    if name=='b':
        print(group['data1'].mean())
        
#根据多个分组键来分组，并对不同分组做不同操作，for循环的第一个元素其格式是由键组合而成的元组(name1,name2)
for (name1,name2),group in df.groupby(['key1','key2']):
    if name1=='a' and name2=='one':
        print(group)
        print(group['data1'].mean())
    if name1=='a' and name2=='two':
        print(group)
        print(group['data1'].sum())
    if name1=='b' and name2=='one':
        print(group)
        print(group['data1'].sum())
    if name1=='b' and name2=='two':
        print(group)
        print(group['data1'].sum())
        
#数据聚合操作,agg即是aggregate聚合之意，应给其传函数，可传内置函数或自定义函数
#内置数据统计方法有count/sum/mean/median/std/var/min/max等
def max_func(series):
    return(series.max())

#根据key1分组后，对每个分组只取'data1'及'data2'列序列，并对每个序列进行sum及mean、max_func多函数操作，
print(df.groupby('key1')['data1','data2'].agg(['sum','mean',max_func]))#内置数据统计方法用字符串表示，自定义函数直接取其函数名

#根据key1分组后，针对不同列序列，调用不同多函数,传给agg函数的实参为字典，键为列序列名，值为函数列表
print(df.groupby('key1').agg({'data1':['sum'],'data2':['sum','mean',max_func]}))

#根据key1及key2分组后，针对不同列序列，调用不同多函数，更加特异化
df_statistic=df.groupby(['key1','key2']).agg({'data1':['sum'],'data2':['sum','mean',max_func]})
print(df_statistic)
print(df_statistic['data1']['sum']['a']['one'])#多重索引，默认应先列后行多重索引


# In[22]:


#多重索引dataframe下的数据重构、pivot创建派生表、多重索引dataframe的创建
import pandas as pd
import numpy as np
from pandas import Series,DataFrame
df_0=pd.DataFrame(np.random.randint(0,10,(5,2)),columns=['data1','data2'])
print(df_0)
print(df_0.stack())#将最内层的列索引转换成最内层的行索引

df_1=pd.DataFrame(np.random.randint(0,10,(5,2)),columns=['data1','data2'])
print(df_1.unstack())#将最内层的行索引转换为最内层的列索引

df_2=pd.DataFrame({'A': ['one', 'one', 'two', 'three'] * 6,
                        'B': ['A', 'B', 'C'] * 8,
                        'C': ['foo', 'foo', 'foo', 'bar', 'bar', 'bar'] * 4,
                        'D': np.random.randn(24),
                        'E': np.random.randn(24)})
pd.pivot_table(df_2,values='D',index=['A','B'],columns=['C'])#以['A','B']作为多层行索引，['C']作为列索引，行列相交处填['D']中的相应值

########################多重索引########################
df_3=pd.DataFrame([np.array([1,2,3,4]),np.array([1,2,3,4]),np.array([1,2,3,4]),np.array([1,2,3,4])],
                 index=pd.MultiIndex.from_product([['A','B'],['first','second']]),
                 columns=pd.MultiIndex.from_product([['one','two'],['first','second']]))
print(df_3)
print(df_3['one']['first']['A']['first'])#依层次逐级索引查找，先列后行


# In[35]:


#缺失值处理
import pandas as pd
import numpy as np
from pandas import Series,DataFrame
#判断Series及DataFrame中是否存在缺失值isnull
series_0=pd.Series([1,2,3,np.nan])
print(series_0.isnull())
df_0=pd.DataFrame({'one':[1,2,np.nan],
                 'two':[1,np.nan,3]})
print(df_0.isnull())

#滤除缺失值，应该与isnull联合使用
print(series_0.dropna())#滤除序列中的缺失值
print(df_0.dropna(axis=0))#滤除dataframe中的缺失值，可指定axis

#填充缺失值，应该与isnull联合使用
print(series_0.fillna(0))#将序列中的缺失值填充为0
print(df_0.fillna(0))#将dataframe中的缺失值填充为0

