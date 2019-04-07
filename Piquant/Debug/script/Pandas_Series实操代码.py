
# coding: utf-8

# In[36]:


#对list、ndarray、serise加以区别，并着重阐述series的创建方式
import numpy as np
import pandas as pd

list_0=[1,2,3,0.05]#前3个为int64，第4个是float64
print(list_0) #list内元素可以是不同数据类型，异构容器

#ndarray类似于C语言中的数组，但最大区别在于向量化操作
ndarr_0=np.array(list_0)
print(ndarr_0)#ndarray内元素需相同数据类型，同构容器
print(ndarr_0.dtype)#都强转为float64

#series不仅包含值(一维同构numpy数组)，还有索引/标签数组，具有字典特性
#series可输入列表、字典、ndarray及标量(scalar value)来创建，并可指定标签
#通过列表list来创建
series_0=pd.Series(list_0,index=['a','b','c','d'])
print(series_0)#series也是同构容器，会强转为float64，除值尚有索引/标签
#通过字典来创建
series_1=pd.Series({'a':1, 'b':2, 'c':3})
print(series_1)
#通过ndarray来创建
series_2=pd.Series(np.array(list_0),index=['a','b','c','d'])
print(series_2)
#通过标量来创建
series_3=pd.Series(5.0,index=['a','b','c','d'])
print(series_3)


# In[40]:


#series具有ndarray数组及dict字典的双重特性，可进行两类基本操作、向量化运算
import numpy as np
import pandas as pd
#先创建series
list_0=[1,2,3,0.5]
series_0=pd.Series(list_0,index=['a','b','c','d'])
print(series_0)

#进行ndarray数组基本操作
print(series_0[0])       #索引取值
print(series_0[0:2])     #切片掐头去尾
print(series_0**2)      #向量化操作
print(series_0+2)       #广播操作
print(np.amin(series_0))#函数操作

#进行dict字典基本操作
print(series_0['a'])    #键值对取值
print(series_0.get('b'))#字典的get方法

#当然series本身也可向量化运算，无需for循环逐元素处理
print(series_0+series_0)#元素间相加
print(series_0**2)     #平方


# In[ ]:


#

