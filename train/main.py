import yfinance as yf
import datetime
import numpy as np


def fetch_all_futures(symbols, start_date, end_date, bond_note_symbols):
    futures = {}

    for symbol in symbols:
        future = yf.Ticker(symbol)
        data = future.history(start=start_date, end=end_date)
        last_data = data.iloc[-1]

        Open = last_data["Open"]
        High = last_data["High"]
        Low = last_data["Low"]
        Close = last_data["Close"]

        # pivot average
        PMA = np.mean(
            [last_data["High"], last_data["Low"], last_data["Close"]])
        diff = np.mean([last_data["High"], last_data["Low"]])
        # pivot diff
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

        futures[symbol] = {
            "Open": Open,
            "High": High,
            "Low": Low,
            "Close": Close,
            "PivotHigh": PivotHigh,
            "Pivot": PMA,
            "PivotLow": PivotLow,
            "Resistance 1": R1,
            "Support 1": S1,
            "Resistance 2": R2,
            "Support 2": S2,
            "Resistance 3": R3,
            "Support 3": S3
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


start_date = "2023-06-02"
end_date = datetime.date.today().strftime("%Y-%m-%d")
symbols = [
    'ES=F', 'NQ=F', 'RTY=F', 'YM=F', 'ZB=F', 'ZN=F', 'ZF=F', 'ZT=F', 'CL=F',
    'GC=F', 'SI=F', 'SPY', 'QQQ', 'IWM', 'USO', 'TLT', 'IEF', 'GLD', '^VIX',
    'BTC-USD', 'USDJPY=X', 'EURUSD=X', 'AAPL', 'NFLX', 'TSLA', 'JPM', '^GDAXI'
]

bond_note_symbols = ['ZB=F', 'ZN=F', 'ZF=F', 'ZT=F']

futures = fetch_all_futures(symbols, start_date, end_date, bond_note_symbols)
print(futures)
