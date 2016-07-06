1)Compile

```python
gcc -o tcharts TerminalCharts.c -lncurses
```
2)Download ohlc data from respective website as csv and then Ctrl+A and Ctrl+V to textfile.

[NSE Historical Index Data](https://www.nseindia.com/products/content/equities/indices/historical_index_data.htm)
[NSE Historical Contract-wise](https://www.nseindia.com/products/content/derivatives/equities/historical_fo.htm )

2)Run respective for Japaneese Candlestick, bar pattern.

```python
./tcharts -s bar NIFTY
./tcharts -s japcandle NIFTY
```

Opt)Create live market data

```python
gcc -o randt randt.c -lpthread
```

Opt)Run live market data (under progress)

```python
./live
```
