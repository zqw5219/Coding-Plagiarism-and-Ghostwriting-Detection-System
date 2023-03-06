# this script is for testing and development purpose only, no need for running this script

#!/usr/bin/env python
# coding: utf-8

# In[228]:


import pandas as pd
import numpy as np
import sys
import difflib


# In[229]:


df1 = pd.read_csv('/Users/ziyu/Desktop/bits_TJ.csv')
df1=df1.replace(np.nan, ' ')
#df1


# In[230]:


df2 = pd.read_csv('/Users/ziyu/Desktop/bits_TJ2.csv')
df2=df2.replace(np.nan, ' ')
#df2


# In[231]:


s1=df1["pureCode"].tolist()
s2=df2["pureCode"].tolist()
#s1


# In[232]:


difference_list=[]
element=[]
for i in s1:
    a=i
    for j in s2:
        difference_list.append(difflib.SequenceMatcher(None, a, j).ratio())
        element.append(a+" VS "+j)
print(difference_list)


# In[233]:


match_list=[]
for i in difference_list:
    if i == 1:
        match_list.append(1)
    else:
        match_list.append(0)
#match_list


# In[234]:


match = pd.DataFrame(element, columns=['element'])
match["ratio"]=difference_list
match["match_result"]=match_list


# In[236]:


print(match)


# In[ ]:




