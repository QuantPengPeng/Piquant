
# coding: utf-8

# In[89]:


#list多维数组及相关操作
import numpy as np
list=[[1,2,3],            #2*3 array
      [4,5,6]]
merge_list=list*3         #以二维数组为单位按列拼接
print(merge_list)
ndarr=np.array(merge_list)#将python中的list转换为ndarray
print(ndarr)
print(ndarr.shape)        #按列拼接，6*3数组
print(merge_list[5][2])   #行列都是从0开始


# In[60]:


#list一维数组及相关操作
import numpy as np
list=[1,2,3,4,5,6]        #一维数组，向量
merge_list=list*2
print(merge_list)         #将向量按行拼接
ndarr=np.array(merge_list)#将python中的list转换为ndarray
print(ndarr)
print(ndarr.shape)        #12个元素构成的向量
print(merge_list[11])     #行列都是从0开始


# In[72]:


#list相关方法
list_0=[1,2]
list_0.append(3)          #append方法
print(list_0)

list_1=[4,5]
list_0.extend(list_1)     #extend方法，末尾追加list
print(list_0)

list_0.insert(2,2.5)      #insert方法
print(list_0)             #在索引为2的位置插入2.5

list_0.pop(-1)            #pop方法
print(list_0)

list_0.reverse()          #reverse方法
print(list_0)

list_0.sort(reverse=False)#list.sort(cmp=None, key=None, reverse=False)
print(list_0)


# In[85]:


#list基本操作及常用函数
list_0=[x for x in range(5)]
print(list_0)

list_1=[y for y in range(6,11,1)]#包括6，不包括11
print(list_1)

#list索引从0开始，包括起始索引元素，不包括结束索引元素
print(list_0[0:1]) #切片操作
print(list_0[0:2])
print(list_0[2:4])
print(len(list_0)) #len函数
print(max(list_0)) #max函数
print(min(list_0)) #min函数


# In[109]:


#ndarray创建
import numpy as np
ndarr_0=np.zeros((2,3),dtype=np.int32) #zeros创建方法
print(ndarr_0)

ndarr_1=np.ones((2,4),dtype=np.int32)  #ones创建方法
print(ndarr_1)

ndarr_2=np.arange(1, 10, 2)            #arange创建方法
print(ndarr_2)

ndarr_3=np.array([2,3,4])      #array创建方法,输入列表
print(ndarr_3)

ndarr_4=np.array(((2,3,4),
                  (5,6,7)))   #array创建方法,输入2*3元组
print(ndarr_4)

ndarr_5=np.linspace(1,5,5)   #linspace创建方法
print(ndarr_5)

ndarr_6=np.arange(15).reshape(3, 5)#reshape方法
print(ndarr_6)
print(ndarr_6.shape)#shape方法
print(ndarr_6.dtype)#dtype方法
print(ndarr_6.size)#size方法


# In[18]:


#ndarray索引、切片、迭代、修改、删除、结构调整、拼接、分割、深拷贝、类型转换、条件判断、排序
import numpy as np
ndarr_0=np.array([[2,3,4],
                  [5,6,7],
                 [8,9,10]])
#################################索引################################
print(ndarr_0[1,1])    
print(ndarr_0[1][1])   #列表只允许该索引取值
#################################切片################################
print(ndarr_0[0:3,1])  #切片，numpy包括起始索引，不含结束索引
print(ndarr_0[0:3,0:2])#切片
print(ndarr_0[:,0:2])  #切片
#################################迭代################################
for row in ndarr_0:    #Iterating over multidimensional arrays is done with respect to the first axis
    print(row)
for element in ndarr_0.flat:#iterator over all the elements of the ndarray
    print(element)
#################################删除################################    
print(np.delete(ndarr_0,1,axis = 0))#删除ndarray相应行，返回副本
print(np.delete(ndarr_0,1,axis = 1))#删除ndarray相应列，返回副本
#################################修改################################
ndarr_0[0,:]=[1,2,3]#修改对应行，修改原数组
print(ndarr_0)
ndarr_0[:,0]=[1,2,3]#修改对应列，修改原数组
print(ndarr_0)
ndarr_0[1,1]=3      #修改对应元素，修改原数组
print(ndarr_0)
#################################结构调整################################
print(ndarr_0.reshape(1,9))#返回副本
ndarr_0.resize(1,9)        #直接重构原数组
print(ndarr_0)
#################################拼接################################
#print(np.vstack((ndarr_0,ndarr_0)))#返回副本
print(np.hstack((ndarr_0,ndarr_0))) #返回副本
#################################分割################################
print(np.hsplit(ndarr_0,3))  #按列切，返回副本
#print(np.vsplit(ndarr_0,3)) #按行切，返回副本
#################################深拷贝################################
ndarr_1=ndarr_0
print(ndarr_1 is ndarr_0)#默认传地址
ndarr_1=ndarr_0.copy()#深拷贝，传副本
print(ndarr_1 is ndarr_0)
#################################类型转换################################
ndarr_2=np.array([1.1,1.2,1.23])
print(ndarr_2.astype(np.int32))#astype方法，将float转换为int
#################################条件判断################################
#np.where函数类似C语言中的三元操作符?a:b，也即是a if condition else b
#np.where的典型应用是在数据分析中，基于一个ndarray生成一个新的ndarray
ndarr_3=np.array([[2,3,4],
                  [5,6,7],
                 [8,9,10]])
print(np.where(ndarr_3>3,2,-2))#ndarray是向量化操作
#np.where有个不错的技巧，可以用np.where表示一个嵌套的判断，附链接：https://blog.csdn.net/lemonwyc/article/details/40860237
#其写成 np.where(cond1 & cond2,0,
#                np.where(cond1,1,
#                         np.where(cond2,3)))
#################################排序################################
ndarr_4 = np.array([[0, 3], [6, 2]])
print(np.sort(ndarr_4,axis=0))#按列排序，默认升序
#print(np.sort(ndarr_4,axis=1))#按行排序，默认升序
#print(-np.sort(-ndarr_4,axis=1))#按行排序，给方法及ndarray前加上负号，表示降序
print(ndarr_4)#使用np.sort不会改变ndarray本身，但是list的sort会改变list本身


# In[8]:


#numpy向量化操作，ndarrays提供了批处理数据而不需要写for循环即可方便对ndarray中的元素做相应的计算，即向量化操作
import numpy as np
ndarr_0=np.array([1,2,3,4,5,6])
ndarr_1=np.array([7,8,9,10,11,12])
print(ndarr_1-ndarr_0)#减
print(ndarr_1+ndarr_0)#加
print(ndarr_1*ndarr_0)#对应元素相乘
print(ndarr_1/ndarr_0)#除
print(ndarr_0**2)#平方
print(ndarr_0<3)#布尔
print(ndarr_1@ndarr_0)#矩形乘法，等价于ndarr_1.dot(ndarr_0)
print(ndarr_0+1)#依次+1的广播运算
#可采用ufunc函数，相比python的math,其可进行快速向量化运算


# In[177]:


#numpy逻辑运算
import numpy as np
#########比较运算########
print(np.array_equal([1, 2], [1, 2]))#True if two arrays have the same shape and elements, False otherwise
print(np.greater([4,2],[2,2]))       #Return the truth value of (x1 > x2) element-wise
print(np.greater_equal([4, 2, 1], [2, 2, 2]))#Return the truth value of (x1 >= x2) element-wise
print(np.less([1, 2], [2, 2]))#Return the truth value of (x1 < x2) element-wise
print(np.less_equal([4, 2, 1], [2, 2, 2]))#Return the truth value of (x1 =< x2) element-wise
print(np.equal([0, 1, 3], np.arange(3)))#Return (x1 == x2) element-wise
print(np.not_equal([1.,2.], [1., 3.]))#Return (x1 != x2) element-wise
#########逻辑运算########
ndarr = np.arange(5)
print(np.logical_and(ndarr>1, ndarr<4))#Compute the truth value of x1 AND x2 element-wise
print(np.logical_or(ndarr < 1, ndarr > 3))#Compute the truth value of x1 OR x2 element-wise
print(np.logical_not(ndarr<3))#Compute the truth value of NOT x element-wise
print(np.logical_xor(ndarr < 1, ndarr > 3))#Compute the truth value of x1 XOR x2, element-wise
print(np.all([[True,False],[True,True]], axis=0))#Test whether all array elements along a given axis evaluate to True
print(np.any([[True, False], [False, False]], axis=0))#Test whether any array element along a given axis evaluates to True
#########异常值处理########
print(np.isinf([np.inf, -np.inf, 1.0, np.nan]))#Test element-wise for positive or negative infinity
print(np.isnan([np.log(-1.),1.,np.log(0)]))#Test element-wise for NaN and return result as a boolean array
ndarr_1=np.array([np.nan,np.nan,1.2,2.6])
print(np.nan_to_num(ndarr_1))#判断某元素是否是nan,inf,neginf,如果是，nan换为0，inf换为一个非常大的数，neginf换为非常小的数


# In[18]:


#numpy数据统计常用函数
import numpy as np
ndarr=np.arange(4).reshape((2,2))
print(np.sum(ndarr))#总和
print(np.cumsum(ndarr))#累加
print(np.cumprod(ndarr))#累乘
print(np.amin(ndarr))#Return the minimum of an array or minimum along an axis
print(np.amin(ndarr, axis=0))#沿列
print(np.amin(ndarr, axis=1))#沿行
print(np.amax(ndarr))#Return the maximum of an array or maximum along an axis
print(np.argmin(ndarr,axis=0))#index of minimum elements along an axis,axis=0为沿列
print(np.argmax(ndarr,axis=0))#index of maximum elements along an axis,axis=0为沿列
print(np.ptp(ndarr, axis=0))#Range of values (maximum - minimum) along an axis
print(np.median(ndarr))#Compute the median along the specified axis
print(np.average(ndarr))#Compute the weighted average along the specified axis
print(np.mean(ndarr))#Compute the arithmetic mean along the specified axis
print(np.std(ndarr))#Compute the standard deviation along the specified axis
print(np.var(ndarr))#方差
#numpy.corrcoef
print(np.percentile(ndarr, 50))#Compute the qth percentile of the data along the specified axis

