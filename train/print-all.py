import yfinance as yf
import datetime
import numpy as np
import argparse


def fetch_all_futures(symbols, bond_note_symbols, start_date, end_date):
    futures = {}

    # Define the keys needed for each day's data
    formatted_values = [
        "Open", "High", "Low", "Close", "PivotHigh", "PMA", "PivotLow", "R1", "S1",
        "R2", "S2", "R3", "S3"
    ]

    for symbol in symbols:
        # Fetch the future data for the given symbol using yfinance
        future = yf.Ticker(symbol)
        data = future.history(start=start_date, end=end_date)

        # Drop any rows with missing data
        data = data.dropna()

        # Loop over each row of the data (each day's worth of data)
        for i, row in data.iterrows():
            # Use the date from the row to create a key for this day's data in the futures dictionary
            date = datetime.datetime.strftime(i, "%Y-%m-%d")

            # Extract the OHLC values from the row
            ohlc_values = [row["Open"], row["High"], row["Low"], row["Close"]]

            # Compute the pivot, resistance, and support levels as before
            PMA = np.mean(ohlc_values[1:])
            diff = np.mean(ohlc_values[1:3])
            PR = abs(PMA - diff)
            PivotHigh = PMA + PR
            PivotLow = PMA - PR
            R1 = 2 * PMA - ohlc_values[2]
            S1 = 2 * PMA - ohlc_values[1]
            R2 = PMA + (ohlc_values[1] - ohlc_values[2])
            S2 = PMA - (ohlc_values[1] - ohlc_values[2])
            R3 = R1 + (ohlc_values[1] - ohlc_values[2])
            S3 = S1 - (ohlc_values[1] - ohlc_values[2])

            # If the symbol is a bond/note symbol, format the values to the appropriate format
            if symbol in bond_note_symbols:
                ohlc_values = [convert_bond_note(val) for val in ohlc_values]
                PMA, PivotHigh, PivotLow, R1, S1, R2, S2, R3, S3 = \
                    [convert_bond_note(val) for val in [
                        PMA, PivotHigh, PivotLow, R1, S1, R2, S2, R3, S3]]
            # Otherwise, round the values to two decimal places
            else:
                ohlc_values = [round(val, 2) for val in ohlc_values]
                PMA, PivotHigh, PivotLow, R1, S1, R2, S2, R3, S3 = \
                    [round(val, 2) for val in [PMA, PivotHigh,
                                               PivotLow, R1, S1, R2, S2, R3, S3]]

            # If this is the first day of data for this symbol, include the pivot range in the keys for the dictionary
            if symbol not in futures:
                PR = round(PR, 3)
                futures[symbol] = {}
            # Add the day's data to the dictionary under the appropriate symbol and date keys
            futures[symbol][date] = {
                formatted_values[i]: ohlc_values[i]
                for i in range(len(ohlc_values))
            }
            futures[symbol][date] = {
                "Open": ohlc_values[0],
                "High": ohlc_values[1],
                "Low": ohlc_values[2],
                "Close": ohlc_values[3],
                "PivotHigh": PivotHigh,
                "PMA": PMA,
                "PivotLow": PivotLow,
                "range": round(PR, 2),
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


today = datetime.date.today().strftime("%Y-%m-%d")
start_date = "2023-06-02"
end_date = today
symbols = [
    'ES=F', 'NQ=F', 'RTY=F', 'YM=F', 'ZB=F', 'ZN=F', 'ZF=F', 'ZT=F', 'CL=F',
    'GC=F', 'SI=F', 'SPY', 'QQQ', 'IWM', 'USO', 'TLT', 'IEF', 'GLD', '^VIX',
    'BTC-USD', 'USDJPY=X', 'EURUSD=X', 'AAPL', 'NFLX', 'TSLA', 'JPM', '^GDAXI'
]

bond_note_symbols = ['ZB=F', 'ZN=F', 'ZF=F', 'ZT=F']

parser = argparse.ArgumentParser()
parser.add_argument('--start_date', type=str)
parser.add_argument('--end_date', type=str)
args = parser.parse_args()
if args.start_date and args.end_date:
    futures = fetch_all_futures(symbols,
                                bond_note_symbols,
                                start_date=args.start_date,
                                end_date=args.end_date)
else:
    futures = fetch_all_futures(symbols, bond_note_symbols, str(start_date),
                                str(end_date))


def print_dict(dictionary):
    for key, value in dictionary.items():
        for vi, vv in value.items():
            # print(key, vi, vv)
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


print('{:^15} {:^15} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10}'.format(
    'Symbol', 'Date', 'Open', 'High', 'Low', 'Close', 'PivotHigh', 'Pivot',
    'PivotLow', 'RES 1', 'SUP 1', 'RES 2', 'SUP 2', 'RES 3', 'SUP 3'))
print_dict(futures)
