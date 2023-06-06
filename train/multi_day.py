import yfinance as yf
import datetime
import numpy as np


def fetch_all_futures(symbols, start_date, end_date, bond_note_symbols):
    futures = {}

    for symbol in symbols:
        future = yf.Ticker(symbol)
        data = future.history(start=start_date, end=end_date)
        data = data.dropna()
        rows = data.iterrows()

        for i, row in rows:
            date = datetime.datetime.strftime(i, "%Y-%m-%d")
            Open = row["Open"]
            High = row["High"]
            Low = row["Low"]
            Close = row["Close"]

            # we can fetch globex future historical data
            # average of high, low, close of globex
            PMA = np.mean([row["High"], row["Low"], row["Close"]])

            # average of high, low
            diff = np.mean([row["High"], row["Low"]])

            # pivot range
            PR = abs(PMA - diff)
            PivotHigh = PMA + PR
            PivotLow = PMA - PR
            R1 = 2 * PMA - Low
            S1 = 2 * PMA - High
            R2 = PMA + (High - Low)
            S2 = PMA - (High - Low)
            R3 = R1 + (High - Low)
            S3 = S1 - (High - Low)

            if symbol in bond_note_symbols:
                Open = convert_bond_note(Open)
                High = convert_bond_note(High)
                Low = convert_bond_note(Low)
                Close = convert_bond_note(Close)
                PivotHigh = convert_bond_note(PivotHigh)
                PMA = convert_bond_note(PMA)
                PivotLow = convert_bond_note(PivotLow)
                R1 = convert_bond_note(R1)
                S1 = convert_bond_note(S1)
                R2 = convert_bond_note(R2)
                S2 = convert_bond_note(S2)
                R3 = convert_bond_note(R3)
                S3 = convert_bond_note(S3)
            else:
                Open = round(Open, 2)
                High = round(High, 2)
                Low = round(Low, 2)
                Close = round(Close, 2)
                PivotHigh = round(PivotHigh, 2)
                PMA = round(PMA, 2)
                PivotLow = round(PivotLow, 2)
                R1 = round(R1, 2)
                S1 = round(S1, 2)
                R2 = round(R2, 2)
                S2 = round(S2, 2)
                R3 = round(R3, 2)
                S3 = round(S3, 2)

            if symbol not in futures:
                PR = round(PR, 3)
                futures[symbol] = {}
            futures[symbol][date] = {
                "Open": Open,
                "High": High,
                "Low": Low,
                "Close": Close,
                "PivotHigh": PivotHigh,
                "PMA": PMA,
                "PivotLow": PivotLow,
                "range": PR,
                "RES 1": R1,
                "SUP 1": S1,
                "RES 2": R2,
                "SUP 2": S2,
                "RES 3": R3,
                "SUP 3": S3
            }

    return futures


def convert_bond_note(num):
    decimal_part = num - int(num)
    post_fix = decimal_part * 320
    if post_fix < 100:
        post_fix = "0" + str(post_fix)
    else:
        post_fix = str(post_fix)

    data = str(int(num)) + "'" + post_fix
    return data.split(".")[0]


start_date = "2023-06-05"
end_date = datetime.date.today().strftime("%Y-%m-%d")
symbols = [
    'ES=F', 'NQ=F', 'RTY=F', 'YM=F', 'ZB=F', 'ZN=F', 'ZF=F', 'ZT=F', 'CL=F',
    'GC=F', 'SI=F', 'SPY', 'QQQ', 'IWM', 'USO', 'TLT', 'IEF', 'GLD', '^VIX',
    'BTC-USD', 'USDJPY=X', 'EURUSD=X', 'AAPL', 'NFLX', 'TSLA', 'JPM', '^GDAXI'
]

bond_note_symbols = ['ZB=F', 'ZN=F', 'ZF=F', 'ZT=F']

futures = fetch_all_futures(symbols, start_date, end_date, bond_note_symbols)


def print_dict(dictionary):
    print('{:^15} {:^15} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10}'.format(
        'Symbol', 'Date', 'Open', 'High', 'Low', 'Close', 'PivotHigh', 'Pivot',
        'PivotLow',
        'RES 1', 'SUP 1',
        'RES 2', 'SUP 2',
        'RES 3', 'SUP 3'))
    for key, value in dictionary.items():
        for vi, vv in value.items():
            print('{:^15} {:^15} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10}'.format(
                key,
                vi,
                vv['Open'],
                vv['High'],
                vv['Low'],
                vv['Close'],
                vv['PivotHigh'],
                vv['PMA'],
                vv['PivotLow'],
                vv['RES 1'],
                vv['SUP 1'],
                vv['RES 2'],
                vv['SUP 2'],
                vv['RES 3'],
                vv['SUP 3']
            ))


print_dict(futures)
