import pandas as pd
import json
import re
import csv

def main():
    df = pd.read_csv('syscall_conversion.csv', skiprows=0, keep_default_na=False)
    df = df.replace('-', -1).astype({b: 'int' for b in df.columns if b != 'Syscall'})
    sort_df = df.sort_values('x86_64')
    sort_df = sort_df.replace(-1, '-')
    sort_df.to_csv('sorted_syscall_conversion.csv')
    print(sort_df)

if __name__ == '__main__':
    main()
