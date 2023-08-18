# -*- coding: utf-8 -*-
"""
Created on Mon Dec 20 19:08:35 2021

@author: X-Z
"""
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def read_data(name:str()) -> np.ndarray:#读取数据
    tmp_file = pd.read_csv("%s.csv"%name, header=0)
    tmp_series = pd.Series(tmp_file['FD'])
    tmp_numpy = tmp_series.to_numpy()
    return tmp_numpy

plt_ct = 0
def plt_show(data, title:str):
    global plt_ct
    plt.figure(plt_ct)
    plt.plot(data)
    plt.title(title)
    plt_ct += 1

ws = 6
m2m = np.zeros(60)
ptr = 0
def m2m_load(num):
    global ptr
    m2m[ptr] = num
    ptr = 0 if ptr==59 else ptr+1

def m2m_read():
    return m2m.max()-m2m.min()

#特征
local_min = []
b_less_than_0 = []
c_greaterthan_0 = []
c_greater_than_a = []
bar = []
blink = []
def feature(f0,f1,f2,f3,f4,f5):
    local_min.append(f0)
    b_less_than_0.append(f1)
    c_greaterthan_0.append(f2)
    c_greater_than_a.append(f3)
    bar.append(f4)
    blink.append(f5)

def blink_detect(data) ->int:
    f0 = True if data[ws] < data[ws-1] and data[ws] < data[ws+1] else False
    pre, post = 0, ws*2
    for i in range(ws, ws*2,1):
        if data[i]>data[i+1]:
            post = i
            break
    for i in reversed(range(ws+1)):
        if data[i]>data[i-1]:
            pre = i
            break
    a, b, c = data[pre], data[ws], data[post]
    f1 = True if b<0 else False
    f2 = True if c>0 else False
    f3 = True if c>a else False
    f4 = (c-b)/m2m_read()
    if f0 and f1 and f2 and f3 and f4>0.4:
        feature(f0,f1,f2,f3,f4,True)
        return 50
    elif f0:
        feature(f0,f1,f2,f3,f4,False)
        return 0
    return 0

first_difference = read_data("data")
plt_show(first_difference, "First Difference")
blink_detect_result = []
for i in range(ws):
    m2m_load(first_difference[i])
for i in range(len(first_difference)):
    if i+ws<len(first_difference):
        m2m_load(first_difference[i+ws])
    if i<ws or i>len(first_difference)-ws-1:
        blink_detect_result.append(0)
    else:
        blink_detect_result.append(blink_detect(first_difference[i-ws:i+ws+1]))
plt.plot(blink_detect_result)

to_csv = {"local_min":local_min,"b_less_than_0":b_less_than_0,"c_greaterthan_0":c_greaterthan_0,"c_greater_than_a":c_greater_than_a,"bar":bar,"blink":blink}
to_csv = pd.DataFrame(to_csv)
to_csv.to_csv("feature.csv")