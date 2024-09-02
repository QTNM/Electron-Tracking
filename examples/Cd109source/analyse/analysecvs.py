#!/usr/bin/env python
# coding: utf-8

# In[3]:


import numpy as np
import matplotlib.pyplot as plt
import csv
import pandas as pd


# In[15]:


#df = pd.read_csv('cd1092.csv')
#print(df.head())
#print(df.columns)


# In[17]:





# In[27]:


def ehist(filename, column):
    '''
    This function reads the csv file and selects the targeted column for electrons and makes it into a histogram
    filename - the name of the csv file
    column - name of the selected column
    '''
    df = pd.read_csv(filename)

    #ensure column names are stripped of leading/traling whitespace
    df.columns = df.columns.str.strip()
    
    # Filter the DataFrame for rows where 'PDG' column is 11
    filtered_df = df[df['PDG'] == 11]
    
    # Check if the filtered DataFrame is not empty
    if not filtered_df.empty:
        # Select the targeted column from the filtered DataFrame
        Tcolumn = filtered_df[column.strip()]

        plt.figure(figsize=(8, 6))
        plt.hist(Tcolumn, bins=200, edgecolor='black')
        plt.title(f'Distribution of {column.strip()} for electrons')
        plt.xlabel(column.strip())
        plt.ylabel('Frequency')
        plt.grid(True)
        plt.xlim(15,25)
        plt.ylim(0,10)
        plt.savefig(f'{column.strip()}_plot_electron.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")

# Example usage: Replace 'cd1092.csv' with your actual CSV file name and 'KE' with your desired column name
ehist('cd109.csv.gz', 'KE')


# In[ ]:





# In[25]:


def ghist(filename, column):
    '''
    This function reads the csv file and selects the targeted column for gamma rays and makes it into a histogram
    filename - the name of the csv file
    column - name of the selected column
    '''
    df = pd.read_csv(filename)

    #ensure column names are stripped of leading/traling whitespace
    df.columns = df.columns.str.strip()
    
    # Filter the DataFrame for rows where 'PDG' column is 11
    filtered_df = df[df['PDG'] == 22]
    
    # Check if the filtered DataFrame is not empty
    if not filtered_df.empty:
        # Select the targeted column from the filtered DataFrame
        Tcolumn = filtered_df[column.strip()]

        plt.figure(figsize=(8, 6))
        plt.hist(Tcolumn, bins=50, edgecolor='black')
        plt.title(f'Distribution of {column.strip()} for gamma')
        plt.xlabel(column.strip())
        plt.ylabel('Frequency')
        plt.grid(True)
        plt.savefig(f'{column.strip()}_plot_gamma.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")

# Example usage: Replace 'cd1092.csv' with your actual CSV file name and 'KE' with your desired column name
#ghist('cd1092.csv', 'Px')
#ghist('cd1092.csv', 'Py')
#ghist('cd1092.csv', 'Pz')\ghist('cd1092.csv', 'Px')

#ghist('cd109.csv.gz', 'Px')
#ghist('cd109.csv.gz', 'Py')
#ghist('cd109.csv.gz', 'Pz')

# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:





# In[ ]:




