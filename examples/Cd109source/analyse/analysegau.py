#!/usr/bin/env python
# coding: utf-8

# In[2]:


import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import gaussian_kde
from scipy.signal import find_peaks, peak_widths
import argparse


# In[4]:


def ehist_kde(filename, column):
    '''
    This function reads the csv file and plot the corresponding Kernal density estimation distributions
    '''
    df = pd.read_csv(filename)
    df.columns = df.columns.str.strip()
    filtered_df = df[df['PDG'] == 11]
    
    if not filtered_df.empty:
        Tcolumn = filtered_df[column.strip()]

        plt.figure(figsize=(8, 6))
        
        # Plot histogram
        plt.hist(Tcolumn, bins=300, density=True, edgecolor='black', alpha=0.6, label='Data histogram')

        # KDE estimation
        kde = gaussian_kde(Tcolumn)
        x = np.linspace(Tcolumn.min(), Tcolumn.max(), 1000)
        kde_values = kde(x)
        
        # Plot KDE
        plt.plot(x, kde_values, 'r-', linewidth=2, label='KDE')

        plt.title(f'Distribution of {column.strip()} for electron (r = 50 um, t = 0.5 um)')
        plt.xlabel(column.strip())
        plt.ylabel('Density')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'{column.strip()}_kde_r=50um_05.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")



# In[6]:


def ehist_kde_peaks(filename, column):
    '''
    This function also printed out the width of the peaks
    '''
    df = pd.read_csv(filename)
    df.columns = df.columns.str.strip()
    filtered_df = df[df['PDG'] == 11]
    
    if not filtered_df.empty:
        Tcolumn = filtered_df[column.strip()]

        plt.figure(figsize=(8, 6))
        
        # Plot histogram
        plt.hist(Tcolumn, bins=300, density=True, edgecolor='black', alpha=0.6, label='Data histogram')

        # KDE estimation
        kde = gaussian_kde(Tcolumn)
        x = np.linspace(Tcolumn.min(), Tcolumn.max(), 1000)
        kde_values = kde(x)
        
        # Plot KDE
        plt.plot(x, kde_values, 'r-', linewidth=2, label='KDE')

        # Find peaks in KDE
        peaks, properties = find_peaks(kde_values)
        peak_x = x[peaks]
        peak_y = kde_values[peaks]

        # Calculate peak widths
        widths, width_heights, left_ips, right_ips = peak_widths(kde_values, peaks, rel_height=0.5)
        width_x = x[left_ips.astype(int)]
        width_right_x = x[right_ips.astype(int)]

        # Plot peaks
        plt.plot(peak_x, peak_y, "bo", label='Peaks')

        # Annotate peaks and widths
        for px, py, wx, wrx, w in zip(peak_x, peak_y, width_x, width_right_x, widths):
            plt.annotate(f'({px:.2f}, {py:.2f})', (px, py), textcoords="offset points", xytext=(0,10), ha='center')
            plt.hlines(y=py * 0.5, xmin=wx, xmax=wrx, color='green')
            plt.annotate(f'Width: {w:.2f}', ((wrx - wx) , py * 0.5), textcoords="offset points", xytext=(0,10), ha='center')
        
        plt.title(f'Distribution of {column.strip()} for electron (r = 50 um, t = 0.5 um)')
        plt.xlabel(column.strip())
        plt.ylabel('Density')
        plt.legend()
        plt.grid(True)
        plt.savefig(f'{column.strip()}_kde_peaks_r=50um_05.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot KDE for a given column in a CSV file.")
    parser.add_argument('-f', '--file', type=str, required=True, help='The CSV file to read.')
    parser.add_argument('-c', '--column', type=str, required=True, help='The column to analyze.')
    args = parser.parse_args()
    
    ehist_kde(args.file, args.column)
    ehist_kde_peaks(args.file, args.column)

# In[ ]:




