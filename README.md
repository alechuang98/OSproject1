# OSproject1

### control.h usage

`make(TSK *tsk)` 

創建一個process，還不會跑，`tsk -> pid`會被設成該process的pid。



`run(TSK *tsk, int run_time)`

讓`tsd -> pid`的process跑`run_time`單位時間，`tsk -> rem`會被減去`run_time`

__note__: `run_time > tsk -> rem` 並沒有被定義，不要出現這種情況



`clear(TSK *tsk)`

收屍



### Prototype

`<scheduling algo>(const TSK *tsk)`

要排序的話自己開葛array排，tsk[]內容不要改，方便debug。

