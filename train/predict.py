import pandas as pd
import yfinance as yf
import concurrent.futures
import matplotlib.pyplot as plt
import math
import json
import numpy as np  # linear algebra
import tensorflow as tf
import tensorflow_addons as tfa
from sklearn.preprocessing import MinMaxScaler

"""
to do
1, save data into csv
2, read ticker range save into
{
    [ticker, date, next_extend_high, next_extend_low]
}
3, combine step 3 data into step 2
4, data is
 [
    {
        symbol: symbol,
        date: date,
        open: open,
        high: high,
        low: low,
        close: close
    }
 ]
 input: [open, high, low, close] output: [next_extend_high, next_extend_low]
"""
start = '2022-04-11'
end = '2023-02-02'

symbols = ['ES=F', 'NQ=F', 'RTY=F', 'CL=F', 'GC=F', 'ZN=F', 'ZB=F', 'YM=F', '^VIX', '^HSI', '^GDAXI', 'EURUSD=X', 'BTC-USD', 'JPY=X', 'SPY', 'QQQ', 'IWM', 'USO', 'TLT', 'IEF',
           'AAPL', 'TSLA', 'NFLX', 'WFC', 'GLD', 'JNJ', 'GOOGL', 'MSFT', 'WM', 'AMZN', 'META', 'TLT', 'NFLX', 'C', 'CRSP', 'IWM', 'JPM', 'RUN', 'XLF', 'SHY', 'QCOM', 'GS', 'FCEL', 'IBM']

symbols_d = {
    'ES=F': 'ES',
    'NQ=F': 'NQ',
    'RTY=F': 'RTY',
    'CL=F': 'CL',
    'GC=F': 'GC',
    'ZN=F': 'ZN',
    'ZB=F': 'ZB',
    'YM=F': 'YM',
    '^VIX': 'VIX',
    '^HSI': 'HSI',
    '^GDAXI': 'FDAX',
    'EURUSD=X': 'EURUSD',
    'BTC-USD': 'BTCUSD',
    'JPY=X': 'USDJPY',
    'SPY': 'SPY',
    'QQQ': 'QQQ',
    'IWM': 'IWM',
    'USO': 'USO',
    'TLT': 'TLT',
    'IEF': 'IEF',
    'AAPL': 'AAPL',
    'TSLA': 'TSLA',
    'NFLX': 'NFLX',
    'WFC': 'WFC',
    'GLD': 'GLD',
    'JNJ': 'JNJ',
    'GOOGL': 'GOOGL',
    'MSFT': 'MSFT',
    'WM': 'WM',
    'AMZN': 'AMZN',
    'META': 'META',
    'TLT': 'TLT',
    'NFLX': 'NFLX',
    'C': 'C',
    'CRSP': 'CRSP',
    'JPM': 'JPM',
    'RUN': 'RUN',
    'XLF': 'XLF',
    'SHY': 'SHY',
    'QCOM': 'QCOM',
    'GS': 'GS',
    'FCEL': 'FCEL',
    'IBM': 'IBM'
}


def read_data():
    filepath = './data.csv'
    result = []
    """
    {
        ('ES', '2022-09-08'): [low, high]
    }
    """
    result_d = {}
    # key = []
    with open(filepath, "r") as file:
        result = [x.strip().split(', ') for x in file.readlines()]
        for i in result:
            result_d[(i[1], i[0])] = [float(i[2]), float(i[3])]
            # if i[1] not in key:
            # key.append(i[1])
    # print(key)
    return result_d
# print(read_data())


def get_data(symbols, start, end):
    results = yf.download(symbols, start=start, end=end, group_by='tickers')
    return results


def fetch_data(start, end):
    results = get_data(symbols, start, end)

    result_d = {}
    for item in symbols:
        for index, row in results[item].iterrows():
            symbol = symbols_d[item]
            date = index.strftime('%Y-%m-%d')
            # print('symbol is %s, date is %s' % (symbol, date))
            result_d[(symbol, date)] = []

            # print OHLC
            for i, v in row.items():
                if (i == 'Open' or i == 'High' or i == 'Low' or i == 'Close') and not math.isnan(float(v)):
                    result_d[(symbol, date)].append(round(v, 5))
            # print('\n')
        # print('\n')
    return result_d


source_result = fetch_data(start, end)
output_result = read_data()
# print(source_result)


def optimize_data(source_result, output_result):
    res = []
    for k, v in source_result.items():
        # value is not empty and key is in read_data
        if v and k in output_result:
            v.extend(output_result[k])
            # show symbol and OHLC with (low, high)
            # value = (k, v)
            res.append(v)
    return res


res = optimize_data(source_result, output_result)
# print(res)
print('res [] length is %s ' % len(res))
