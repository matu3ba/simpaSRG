#!/usr/bin/env python3
from tabulate import tabulate
import pandas as pd

#cars = {'Brand': ['Honda Civic','Toyota Corolla','Ford Focus','Audi A4'],
#        'Price': [22000,25000,27000,35000]
#        }
#
#df = pd.DataFrame(cars, columns= ['Brand', 'Price'])

#df.to_csv ('export_dataframe.csv', index = False, header=True)

df = pd.read_csv ('export_dataframe.csv')

print (df)

