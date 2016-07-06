# 1)Compile
```python
gcc -o tcharts TerminalCharts.c -lncurses
```
# 2)Download ohlc data from respective website as csv and then Ctrl+A and Ctrl+V to textfile.

[NSE Historical Index Data](https://www.nseindia.com/products/content/equities/indices/historical_index_data.htm)
[NSE Historical Contract-wise](https://www.nseindia.com/products/content/derivatives/equities/historical_fo.htm )

# 3)Run respective for Japaneese Candlestick / Bar pattern (replace "NIFTY" with file saved earlier).
```python
./offline -s candle NIFTY
./offline -s bar NIFTY
```

# 4)Create live market data (use "rand" file created now in above code).
```python
gcc -o randt randt.c -lpthread
```

# 5)Run live market data (under progress)
```python
./live.sh
```
