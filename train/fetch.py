import yfinance as yf


def print_ohlc(ticker_list, start_date, end_date):
    whitelist = ["ES=F", "NQ=F"]
    for symbol in ticker_list:
        ticker = yf.Ticker(symbol)
        ticker_data = ticker.history(start=start_date, end=end_date)

        if symbol in whitelist:
            print(symbol, "OHLC:")
            print(ticker_data[['Open', 'High', 'Low', 'Close']].round(2))
            print("\n")
        else:
            print(symbol, "OHLC:")
            print(ticker_data[['Open', 'High', 'Low', 'Close']])
            print("\n")


# Example usage
ticker_list = ["ES=F", "NQ=F", "RTY=F", "DX-Y.NYB", "ZN=F",
               "ZB=F", "GC=F", "CL=F", "SPY", "QQQ", "TLT", "AAPL", "JPM"]
start_date = "2023-01-01"
end_date = "2023-05-31"
print_ohlc(ticker_list, start_date, end_date)
