import yfinance as yf
from datetime import datetime, timedelta
import pytz

# create a timezone for UTC-4
utc_minus_4 = pytz.timezone('Etc/GMT+4')

# create a ticker object for the ES=F symbol
es = yf.Ticker("ES=F")

# get the current time in UTC-4
now_utc_minus_4 = datetime.now(tz=utc_minus_4)

if now_utc_minus_4.hour < 17:
    # if during market hours, get the price range so far
    end_time = now_utc_minus_4.replace(second=0, microsecond=0)
    start_time = end_time.replace(hour=18, minute=0, second=0, microsecond=0)
else:
    # if after market hours, get the price range until today's close
    end_time = now_utc_minus_4.replace(
        hour=17, minute=0, second=0, microsecond=0)
    start_time = end_time - timedelta(days=1)
    start_time = start_time.replace(hour=18, minute=0, second=0, microsecond=0)

# convert start and end times to UTC
start_time_utc = start_time.astimezone(pytz.utc)
end_time_utc = end_time.astimezone(pytz.utc)

# get the historical prices for the trading session in UTC
prices = es.history(start=start_time_utc, end=end_time_utc, interval='1m')

# compute the open, high, low, and close prices
open_price = prices.iloc[0]['Open']
high_price = prices['High'].max()
low_price = prices['Low'].min()
close_price = prices.iloc[-1]['Close']

# print the prices
print("ES=F Open: ", open_price)
print("ES=F High: ", high_price)
print("ES=F Low: ", low_price)
print("ES=F Close: ", close_price)
