#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import matplotlib.pyplot as plt
import csv
import pandas as pd
import argparse


# In[62]:


#detected electron and gamma ray energies by the particle datasheet
e_value = [2.61, 18.5, 62.5196, 84.2278, 87.3161,87.9384]
g_value = np.array([2.98, 21.99, 22.163, 24.912, 24.943, 25.455, 88.0336])

def ehist(filename, column, eval=None, gval=None):
    '''
    This function reads the csv file and selects the targeted column for electrons and makes it into a histogram.
    
    filename - the name of the csv file
    column - name of the selected column
    lines - a list of x-coordinates where vertical lines should be added (default is None)
    '''
    df = pd.read_csv(filename)

    # Ensure column names are stripped of leading/trailing whitespace
    df.columns = df.columns.str.strip()
    
    # Filter the DataFrame for rows where 'PDG' column is 11
    filtered_df = df[df['PDG'] == 11]
    
    # Check if the filtered DataFrame is not empty
    if not filtered_df.empty:
        # Select the targeted column from the filtered DataFrame
        Tcolumn = filtered_df[column.strip()]

        plt.figure(figsize=(8, 6))
        plt.hist(Tcolumn, bins=1000, edgecolor='black')
        plt.yscale('log')  #make the y scale logrithem
        plt.title(f'Distribution of {column.strip()} for electrons (shell r_min = 10 mm)')
        plt.xlabel(column.strip())
        plt.ylabel('Frequency')
        plt.grid(True)

        # Flags to ensure each label is added only once
        red_label_added = False
        blue_label_added = False

        # Add vertical lines if specified
        if eval:
            for x_value in eval:
                if not red_label_added:
                    plt.axvline(x=x_value, color='red', linestyle='--', linewidth=1.5, label='Expected peaks of electrons')
                    red_label_added = True
                else:
                    plt.axvline(x=x_value, color='red', linestyle='--', linewidth=1.5)
        
        # Add vertical lines for gval
        if gval:
            for x_value in gval:
                if not blue_label_added:
                    plt.axvline(x=x_value, color='blue', linestyle='--', linewidth=1.5, label='Expected peaks of gamma rays')
                    blue_label_added = True
                else:
                    plt.axvline(x=x_value, color='blue', linestyle='--', linewidth=1.5)
        
        #plt.xlim(15, 25)
        #plt.ylim(0, 1e4)
        plt.legend()

        #plt.savefig(f'{column.strip()}_plot_electron_porintr_1.png')
        plt.savefig(f'shellr_10mm.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")

# Add vertical dashed lines at specified x-values
ehist('shellrmin_10mm.csv.gz', 'KE', eval=[2.61, 18.5, 62.5196, 84.2278, 87.3136, 87.9384], gval = [2.98, 21.99, 22.163, 24.912, 24.943, 25.455, 88.0336])


# In[66]:


def ghist(filename, column,eval = True, gval = True):
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
        plt.hist(Tcolumn, bins=100, edgecolor='black')
        plt.title(f'Distribution of {column.strip()} for gamma (r = 1 mm)')
        plt.xlabel(column.strip())
        plt.ylabel('Frequency')
        plt.grid(True)

     # Flags to ensure each label is added only once
        red_label_added = False
        blue_label_added = False

        # Add vertical lines if specified
        if eval:
            for x_value in eval:
                if not red_label_added:
                    plt.axvline(x=x_value, color='red', linestyle='--', linewidth=1.5, label='Expected peaks of electrons')
                    red_label_added = True
                else:
                    plt.axvline(x=x_value, color='red', linestyle='--', linewidth=1.5)
        
        # Add vertical lines for gval
        if gval:
            for x_value in gval:
                if not blue_label_added:
                    plt.axvline(x=x_value, color='blue', linestyle='--', linewidth=1.5, label='Expected peaks of gamma rays')
                    blue_label_added = True
                else:
                    plt.axvline(x=x_value, color='blue', linestyle='--', linewidth=1.5)
        
        #plt.xlim(15, 25)
        #plt.ylim(0, 10)
        plt.legend()
        plt.savefig(f'{column.strip()}_plot_gamma_r=1mm.png')
        plt.show()
    else:
        print(f"No data available for PDG code 11 in column {column.strip()}")

# Example usage: Replace 'cd1092.csv' with your actual CSV file name and 'KE' with your desired column name
#ghist('cd109.csv.gz', 'KE', eval=[2.61, 18.5, 62.5196, 84.2278, 87.3136, 87.9384], gval = [2.98, 21.99, 22.163, 24.912, 24.943, 25.455, 88.0336])
#ghist('cd1092.csv', 'Py')
#ghist('cd1092.csv', 'Pz')\ghist('cd1092.csv', 'Px')


# In[ ]:
def erhist(filename):
    '''
    This function reads the csv file and selects the targeted column for electrons and makes it into a histogram.
    
    filename - the name of the csv file
    '''
    # Read the CSV file into a DataFrame
    df = pd.read_csv(filename)

    # Ensure column names are stripped of leading/trailing whitespace
    df.columns = df.columns.str.strip()
    
    # Print the column names to verify they are correctly stripped
    print("Column names:", df.columns)

    # Filter the DataFrame for rows where 'PDG' column is 11
    filtered_df = df[df['PDG'] == 12]
    
    # Print the first few rows of the filtered DataFrame to check if filtering is correct
    #print("Filtered DataFrame:", filtered_df.head())

    # Check if the filtered DataFrame is not empty
    if not filtered_df.empty:
        # Select the targeted columns from the filtered DataFrame
        try:
            Tx = filtered_df["posx"]
            Ty = filtered_df["posy"]
            Tz = filtered_df["posz"]
        except KeyError as e:
            print(f"Error: {e}. Check if the column names are correct in the CSV file.")
            return
        
        # Calculate the radius
        Tr = np.sqrt(Tx**2 + Ty**2 + Tz**2)

        # Plot the histogram
        plt.figure(figsize=(8, 6))
        plt.hist(Tr, bins=500, edgecolor='black')
        plt.yscale('log')  # Set the y-axis to logarithmic scale
        plt.title(f'Correlation of number of events and radius')
        plt.xlabel('r / mm')
        plt.ylabel('Frequency')
        plt.grid(True)

        # Save and show the plot
        plt.savefig('events_radius_r-2.png')
        plt.show()
    else:
        print("No data available for PDG code 11.")

# Call the function with the filename
#erhist('cd109.csv.gz')




# In[ ]:




