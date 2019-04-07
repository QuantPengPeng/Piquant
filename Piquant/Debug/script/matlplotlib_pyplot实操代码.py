
# coding: utf-8

# In[35]:


import matplotlib.pyplot as plt
from pylab import *
import numpy as np

main_image=plt.figure(figsize=(10,10))
subplots_adjust(hspace=0.3,wspace=0.3)#控制子图间的行间距、列间距

#子图1-单线
x_0=np.linspace(0,2*np.pi,20) #自变量X的取值范围
sub_image_1=plt.subplot(2,2,1)
plt.xlabel('X value')
plt.ylabel('Sin value')
plt.grid(True)
sub_image_1.plot(x_0, np.sin(x), 'r--o',label='Sin(x)')
sub_image_1.legend()#展示图例
sub_image_1.annotate('sin wave', xy=(3,0.25), xytext=(4,0.5), arrowprops=dict(facecolor='black',shrink=0.05))#特定文本注释
sub_image_1.set_title('Sin Waves')

#子图2-多线
x_1=np.linspace(0,2*np.pi,20) 
sub_image_2=plt.subplot(2,2,2)
plt.xlabel('X value')
plt.ylabel('Cos and Sin value')
plt.grid(True)
sub_image_2.plot(x_1, np.cos(x), color='blue', linestyle='--',linewidth=1, marker='o', markerfacecolor='red', markersize='6', label='Cos(x)')
sub_image_2.plot(x_1, np.sin(x), color='green', linestyle='-.', linewidth=3, marker='^', markerfacecolor='yellow', markersize='8', label='Sin(x)')
sub_image_2.legend()
sub_image_2.set_title('Cos and Sin Waves')

#子图3-直方图
bins_count=10
mu,sigma=100,20
x_hist=mu+sigma*np.random.randn(1000,1)#randn用于生成符合标准正态分布的包含1000个元素的列序列
sub_image_3=plt.subplot(2,2,3)
plt.xlabel('value')
plt.ylabel('count')
plt.grid(False)
tuple_return=sub_image_3.hist(x_hist, bins=bins_count, facecolor='red', alpha=0.8, edgecolor='black',normed=0)#normed=0画频数直方图，normed=1画频率直方图
sub_image_3.set_title('Frequency Histogram')
plt.xlim((floor(x_hist.min()),ceil(x_hist.max())))
bar_width=(x_hist.max()-x_hist.min())/bins_count
plt.xticks(np.arange(floor(x_hist.min()),ceil(x_hist.max()),round(bar_width)))#刻度设置                 
for i in range(bins_count):
    sub_image_3.text(x_hist.min()+(bar_width*i)+(bar_width/2), tuple_return[0][i], str(tuple_return[0][i]), horizontalalignment='center', verticalalignment='bottom')


#子图3-分段函数
x_part_1=np.linspace(-10,-1,10)#分段函数的离散取值
x_part_2=np.linspace(0,10,11)
sub_image_4=plt.subplot(2,2,4)
plt.xlabel('X value')
plt.ylabel('Y value')
plt.grid(False)
sub_image_4.plot(x_part_1,x_part_1*2+1,'b--o',label='y=2x+1')
sub_image_4.plot(x_part_2,x_part_2**2,'r--o',label='y=x^2')
sub_image_4.legend()
sub_image_4.set_title('PieceWise Function')

#展示
plt.show()

