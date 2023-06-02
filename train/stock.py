from yahoofinancials import YahooFinancials
import pprint

import ssl
try:
    _create_unverified_https_context = ssl._create_unverified_context
except AttributeError:
    # Legacy Python that doesn't verify HTTPS certificates by default
    pass
else:
    # Handle target environment that doesn't support HTTPS verification
    ssl._create_default_https_context = _create_unverified_https_context

pp = pprint.PrettyPrinter(indent=2)

total = ['DIS', 'OXY', 'INTC', 'JPM', 'BRK-B', 'CVS', 'psx', 'dal', 'usb', 't',
         'rcl', 'wfc', 'kr', 'bac', 'ccl', 'su', 'csl', 'fdx', 'wfc', 'pxd',
         'vlo', 'ual', 'xom', 'pfe', 'hsbc', 'fang', 'twtr', 'ohi', 'uber',
         'tpr', 'aeo', 'ubs', 'ge']
assets = ['DIS']

yahoo_financials = YahooFinancials(assets)

annual_income = yahoo_financials.get_financial_stmts('annual', 'income')
annual_balance = yahoo_financials.get_financial_stmts('annual', 'balance')
financials_data = yahoo_financials.get_stock_data('keystats', 'financialData')
# get_financial_data()

res = []
for item in annual_balance['balanceSheetHistory']:
    annual_balance_data = annual_balance['balanceSheetHistory'][item]
    annual_income_data = annual_income['incomeStatementHistory'][item]
    financials_data_values = financials_data[item]

    key = annual_balance_data[0].keys()[0]

    annual_balance_values = annual_balance_data[0].values()[0]
    annual_income_values = annual_income_data[0].values()[0]
    res_key = item + '/' + key
    res.append({
        res_key: [
            {
                'totalCurrentAssets': annual_balance_values.get('totalCurrentAssets'),
                'totalCurrentLiabilities': annual_balance_values.get('totalCurrentLiabilities'),
                'totalAssets': annual_balance_values.get('totalAssets'),
                'totalLiab': annual_balance_values.get('totalLiab'),
                'totalStockholderEquity': annual_balance_values.get('totalStockholderEquity'),
                'retainedEarnings': annual_balance_values.get('retainedEarnings'),
                'totalRevenue': annual_income_values.get('totalRevenue'),
                'netIncome': annual_income_values.get('netIncome'),
            },
            {
                'currentRatio': financials_data_values.get('currentRatio').get('raw'),
                'returnOnEquity': financials_data_values.get('returnOnEquity').get('raw'),
                'currentPrice': financials_data_values.get('currentPrice').get('raw'),
                'targetHighPrice': financials_data_values.get('targetHighPrice').get('raw'),
                'targetLowPrice': financials_data_values.get('targetLowPrice').get('raw'),
                'debtToEquity': financials_data_values.get('debtToEquity').get('raw'),
                'revenuePerShare': financials_data_values.get('revenuePerShare').get('raw'),
                'returnOnAssets': financials_data_values.get('returnOnAssets').get('raw'),
                'earningsGrowth': financials_data_values.get('earningsGrowth').get('raw'),
                'targetMedianPrice': financials_data_values.get('targetMedianPrice').get('raw'),
            }
        ]
    })
pp.pprint(res)
