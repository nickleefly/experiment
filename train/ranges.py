import yfinance as yf
from datetime import datetime, timedelta
import numpy as np
import pytz


def fetch_all_futures(symbols, bond_note_symbols, start_date, end_date):
    all_results = {}

    # Define the keys needed for each day's data
    formatted_values = [
        "Open", "High", "Low", "Close", "PivotHigh", "PMA", "PivotLow", "R1", "S1",
        "R2", "S2", "R3", "S3"
    ]

    for symbol in symbols:
        # Fetch the future data for the given symbol using yfinance
        future = yf.Ticker(symbol)

        data = future.history(start=start_date, end=end_date, interval='1m')

        # Drop any rows with missing data
        data = data.dropna()

        result = {}
        result['Open'] = data.iloc[0]['Open']
        result['High'] = data['High'].max()
        result['Low'] = data['Low'].min()
        result['Close'] = data.iloc[-1]['Close']
        date = end_date.strftime("%Y-%m-%d")

        # Extract the OHLC values from the row
        ohlc_values = [
            result["Open"], result["High"], result["Low"], result["Close"]
        ]

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
        elif (symbol == 'EURUSD=X'):
            ohlc_values = [round(val, 5) for val in ohlc_values]
            PMA, PivotHigh, PivotLow, R1, S1, R2, S2, R3, S3 = \
                [round(val, 5) for val in [PMA, PivotHigh,
                                           PivotLow, R1, S1, R2, S2, R3, S3]]
        else:
            ohlc_values = [round(val, 2) for val in ohlc_values]
            PMA, PivotHigh, PivotLow, R1, S1, R2, S2, R3, S3 = \
                [round(val, 2) for val in [PMA, PivotHigh,
                                           PivotLow, R1, S1, R2, S2, R3, S3]]

        # If this is the first day of data for this symbol, include the pivot range in the keys for the dictionary
        if symbol not in all_results:
            PR = round(PR, 3)
            all_results[symbol] = {}
        # Add the day's data to the dictionary under the appropriate symbol and date keys
        all_results[symbol][date] = {
            formatted_values[i]: ohlc_values[i]
            for i in range(len(ohlc_values))
        }

        all_results[symbol][date] = {
            "Open": ohlc_values[0],
            "High": ohlc_values[1],
            "Low": ohlc_values[2],
            "Close": ohlc_values[3],
            "PivotHigh": PivotHigh,
            "PMA": PMA,
            "PivotLow": PivotLow,
            "RES 1": R1,
            "SUP 1": S1,
            "RES 2": R2,
            "SUP 2": S2,
            "RES 3": R3,
            "SUP 3": S3
        }

    return all_results


def convert_bond_note(num):
    decimal_part = num - int(num)
    post_fix = decimal_part * 320
    if post_fix < 100:
        post_fix = "0" + str(post_fix)
    else:
        post_fix = str(post_fix)

    data = str(int(num)) + "'" + post_fix
    return data.split(".")[0]


def print_dict(dictionary):
    print(
        '{:^15} {:^15} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10}'
        .format('Symbol', 'Date', 'Open', 'High', 'Low', 'Close', 'PivotHigh',
                'Pivot', 'PivotLow', 'RES 1', 'SUP 1', 'RES 2', 'SUP 2', 'RES 3',
                'SUP 3'))
    for key, value in dictionary.items():
        for vi, vv in value.items():
            print(
                '{:^15} {:^15} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10} {:^10}'
                .format(key, vi, vv['Open'], vv['High'], vv['Low'], vv['Close'],
                        vv['PivotHigh'], vv['PMA'], vv['PivotLow'], vv['RES 1'],
                        vv['SUP 1'], vv['RES 2'], vv['SUP 2'], vv['RES 3'],
                        vv['SUP 3']))


if __name__ == '__main__':
    """
        * yesterday
            * if current hour is less than 17
                * if today is monday, start_time is last Thursday 18:00, end_time is Friday 17:00
                * else start_time is 18:00 one day before yesterday, end_time is yesterday 17:00
        * today
            * if current hour is more than 17, start_time is yesterday 18:00, end_time is today's 17:00
        """
    utc_minus_4 = pytz.timezone('US/Eastern')

    # get the current time in UTC-4
    now_utc_minus_4 = datetime.now(tz=utc_minus_4)
    print(now_utc_minus_4)

    # check if today is Monday
    if now_utc_minus_4.weekday() == 0:
        is_monday = True
    else:
        is_monday = False

    # check if today is Saturday
    if now_utc_minus_4.weekday() == 5:
        is_saturday = True
    else:
        is_saturday = False

    # check if today is Sunday
    if now_utc_minus_4.weekday() == 6:
        is_sunday = True
    else:
        is_sunday = False

    if is_saturday:
        end_time = now_utc_minus_4.replace(
            hour=17, minute=0, second=0, microsecond=0) - timedelta(days=1)
    elif is_sunday:
        end_time = now_utc_minus_4.replace(
            hour=17, minute=0, second=0, microsecond=0) - timedelta(days=2)
    else:
        if now_utc_minus_4.hour < 17:
            # if during market hours, use yesterday's time
            if is_monday:
                end_time = now_utc_minus_4.replace(
                    hour=17, minute=0, second=0, microsecond=0) - timedelta(days=3)
            else:
                end_time = now_utc_minus_4.replace(
                    hour=17, minute=0, second=0, microsecond=0) - timedelta(days=1)

        else:
            # if after market hours, use today's time
            end_time = now_utc_minus_4.replace(hour=17,
                                               minute=0,
                                               second=0,
                                               microsecond=0)
    start_time = end_time - timedelta(days=1)
    start_time = start_time.replace(hour=18, minute=0, second=0, microsecond=0)

    print(f'start_time is {start_time}, end_time is {end_time}')

    def get_data(start_date, end_date):
        symbols = [
            'ES=F', 'NQ=F', 'RTY=F', 'YM=F', 'ZB=F', 'ZN=F', 'ZF=F', 'ZT=F', 'CL=F',
            'GC=F', 'SI=F', 'SPY', 'QQQ', 'IWM', 'USO', 'TLT', 'IEF', 'GLD', '^VIX',
            'BTC-USD', 'USDJPY=X', 'EURUSD=X', 'AAPL', 'NFLX', 'TSLA', 'JPM',
            '^GDAXI'
        ]

        bond_note_symbols = ['ZB=F', 'ZN=F', 'ZF=F', 'ZT=F']

        futures = fetch_all_futures(symbols, bond_note_symbols, start_date,
                                    end_date)
        return futures

    futures = get_data(start_time, end_time)
    print_dict(futures)
