import yfinance as yf
c = yfinance.download(["SPY"], start="1993-01-01", end="2023-06-06",progress=False)["Close"]

r = c/c.shift()-1

s = r.rolling(60,min_periods=20).std().shift()

f = -(r/r).where(r.index.month==9,0)
f = f - f.mean()

(r*f/s).cumsum().plot()
