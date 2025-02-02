### 0.1

Whenever I decided to add this changelog

62996 bytes

```c
5981643 nodes 2646000 nps
```

### 0.2

"Bunch of changes":

* 8 output buckets NNUE
* Adjusted time management for Kaggle
* Relax 3-fold checks

65384 bytes

```c
6319719 nodes 2320000 nps
```

```c
Max observed memory: 4.4 MB
```

### 0.3

Use shorts for histories

65377 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
01a50fe66a15b2c86a5434d54ad9994f  Alexandria

```c
6319719 nodes 2279000 nps
```

```c
Max observed memory: 2.9 MB
```

```
Score of sse-shorthist vs sse-dev: 253 - 245 - 502  [0.504] 1000
...      sse-shorthist playing White: 231 - 27 - 243  [0.704] 501
...      sse-shorthist playing Black: 22 - 218 - 259  [0.304] 499
...      White vs Black: 449 - 49 - 502  [0.700] 1000
Elo difference: 2.8 +/- 15.2, LOS: 64.0 %, DrawRatio: 50.2 %
```

### 0.4

Smaller conthist

65481 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
4ea9fbdcea3ebec9bb074d088fdc9f8a  Alexandria

```c
5844361 nodes 2383000 nps
```

```c
Max observed memory: 2.4 MB
```

```
Score of sse-conthist2 vs sse-dev: 1366 - 1394 - 2740  [0.497] 5500
...      sse-conthist2 playing White: 1219 - 144 - 1387  [0.695] 2750
...      sse-conthist2 playing Black: 147 - 1250 - 1353  [0.299] 2750
...      White vs Black: 2469 - 291 - 2740  [0.698] 5500
Elo difference: -1.8 +/- 6.5, LOS: 29.7 %, DrawRatio: 49.8 %
```

### 0.5

Reductions table

65028 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
f22b045c49c81dfe415d87e1edb786ec  Alexandria

```c
Max observed memory: 2.4 MB
```

```c
5844361 nodes 2509000 nps
```

### 0.6

Exhaustive xz parameter search, no functional change

64545 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
f22b045c49c81dfe415d87e1edb786ec  Alexandria

### 0.7

Reintroduce magic bitboards

65347 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
32041dcc10431b65ce7bace35ba53b54  Alexandria

```c
5844361 nodes 3031000 nps
```

```c
Max observed memory: 4.7 MB
```

```c
Score of sse-magics vs sse-dev: 11445 - 7372 - 18413  [0.555] 37230
...      sse-magics playing White: 10099 - 579 - 7937  [0.756] 18615
...      sse-magics playing Black: 1346 - 6793 - 10476  [0.354] 18615
...      White vs Black: 16892 - 1925 - 18413  [0.701] 37230
Elo difference: 38.2 +/- 2.5, LOS: 100.0 %, DrawRatio: 49.5 %
```

### 0.8

Change MAX_PLY from 128 to 64

65304 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
70907d0c8fc74b047906065829ee2e50  Alexandria

```
5844361 nodes 3125000 nps
```

```c
Max observed memory: 4.7 MB
```

```c
Score of sse-dev vs sse-magics: 1527 - 1425 - 3048  [0.508] 6000
...      sse-dev playing White: 1383 - 123 - 1494  [0.710] 3000
...      sse-dev playing Black: 144 - 1302 - 1554  [0.307] 3000
...      White vs Black: 2685 - 267 - 3048  [0.702] 6000
Elo difference: 5.9 +/- 6.2, LOS: 97.0 %, DrawRatio: 50.8 %
```

### 0.9

Reduce size, no functional change

64881 bytes

ad007539cec52de9368bc2e2b30fb83f  nn.net
e905aa1e0a940b76711e9cc9937235b8  Alexandria
