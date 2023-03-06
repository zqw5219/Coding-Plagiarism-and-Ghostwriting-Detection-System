#!/usr/bin/env python
# coding: utf-8

# In[195]:


import pandas as pd
import numpy as np
import sys
import difflib


# In[196]:


#read in dataframe
df=pd.read_csv('/Users/ziyu/Desktop/bits_TJ.csv').replace(np.nan, ' ')
df


# In[197]:


n=len(df)+1
nlist=range(1,n)
df['id']=nlist
df.to_csv('df', columns=['id', 'originalCode', 'typeIndicator', 'pureCode', 'pureComment', 'pureMacro'], index=0, header=1)
df


# In[198]:


#read in template
template=pd.read_csv('/Users/ziyu/Desktop/bits_TJ3.csv').replace(np.nan, ' ')
template
#df2 54 441-54=387


# In[199]:


#extract pureComment column from the template and remove unnecessary strings
template_comment=template["pureComment"].tolist()
template_comment=[i for i in template_comment if i != ' ']
template_comment=[i for i in template_comment if i != '/*']
template_comment=[i for i in template_comment if i != '*/']
template_comment=[i for i in template_comment if i != '*']
template_comment=[i for i in template_comment if i != '}']
template_comment=[i for i in template_comment if i != '...']
template_comment
#55-18=37 387+37=424


# In[200]:


#filter template from df
pureComment_filter=df["pureComment"].isin([i for i in template_comment])
pureComment_df=df[pureComment_filter]
pureComment_df


# In[201]:


#drop template from df
pureComment_df=df.drop(pureComment_df['pureComment'].index)
pureComment_df
#440-11=429


# In[202]:


pureComment_df=pd.DataFrame(pureComment_df, columns=['originalCode','typeIndicator','pureComment', 'id'])
pureComment_df


# In[203]:


template_code=template["pureCode"].tolist()
template_code=[i for i in template_code if i != ' ']
template_code=[i for i in template_code if i != '/*']
template_code=[i for i in template_code if i != '*/']
template_code=[i for i in template_code if i != '*']
template_code=[i for i in template_code if i != '}']
template_code=[i for i in template_code if i != '...']
template_code


# In[204]:


pureCode_filter=df["pureCode"].isin([i for i in template_code])
pureCode_df=df[pureCode_filter]
pureCode_df


# In[205]:


pureCode_df=df.drop(pureCode_df['pureCode'].index)
pureCode_df


# In[206]:


#pureCode_df=pd.DataFrame(pureCode_df, columns=['originalCode','typeIndicator','pureCode', 'id'])
#pureCode_df


# In[207]:


template_macro=template["pureMacro"].tolist()
template_macro=[i for i in template_macro if i != ' ']
template_macro=[i for i in template_macro if i != '/*']
template_macro=[i for i in template_macro if i != '*/']
template_macro=[i for i in template_macro if i != '*']
template_macro=[i for i in template_macro if i != '}']
template_macro=[i for i in template_macro if i != '...']
template_macro


# In[208]:


pureMacro_filter=df["pureMacro"].isin([i for i in template_macro])
pureMacro_df=df[pureMacro_filter]
pureMacro_df


# In[209]:


pureMacro_df=df.drop(pureMacro_df['pureMacro'].index)
pureMacro_df


# In[210]:


#pureMacro_df=pd.DataFrame(pureMacro_df, columns=['originalCode','typeIndicator','pureMacro', 'id'])
#pureMacro_df


# In[212]:


#df_template_removed=pd.merge(pureCode_df, pureComment_df)
#df_template_removed=pd.merge(df_template_removed, pureMacro_df)
#df_template_removed


# In[ ]:





# In[ ]:




