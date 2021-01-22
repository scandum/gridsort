Intro
-----
This document describes a stable partitioning comparison sort named gridsort. It is a simplified version of cubesort, intended to be helpful for people wanting to understand or port the code.

Binary Grid
-----------
Gridsort sorts data by storing data in a [binary grid](https://github.com/scandum/binary_cube), a multidimentional sorted array.

Binary Search
-------------
In order to sort an element a [monobound binary search](https://github.com/scandum/binary_search) is performed to pin point the bucket where the element should be stored.

Overflow
--------
Once a bucket overflows it is sorted by [quadsort](https://github.com/scandum/quadsort) and the content is split between two buckets. People wanting to port gridsort might want to use [tailsort](https://github.com/scandum/quadsort) instead of quadsort.

Finish
------
Once all elements have been inserted into the grid, all unsorted buckets are sorted. Since the buckets are already in order the sort is finished.

Big O
-----
|     Name | Best | Average |   Worst | Stable | Partitioning | Memory |
| -------- | ---- | ------- | ------- | ------ | -------------| ------ |
| gridsort |    n | n log n | n log n |    yes |          yes |      n |

Gridsort makes n comparisons when the data is already sorted or reverse sorted.

Benchmarks
----------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|     qsort |   100000 |  i32 | 0.008679 | 0.008990 |     1536357 |     random order |
|  gridsort |   100000 |  i32 | 0.006886 | 0.007006 |     1694286 |     random order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002283 | 0.002514 |      815024 |  ascending order |
|  gridsort |   100000 |  i32 | 0.000578 | 0.000587 |      201117 |  ascending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003078 | 0.003195 |      915018 |    ascending saw |
|  gridsort |   100000 |  i32 | 0.002769 | 0.002850 |      930990 |    ascending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.008667 | 0.008858 |     1536301 |    generic order |
|  gridsort |   100000 |  i32 | 0.007049 | 0.007125 |     1697117 |    generic order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002305 | 0.002498 |      853904 | descending order |
|  gridsort |   100000 |  i32 | 0.000567 | 0.000575 |      199998 | descending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002750 | 0.002975 |     1063907 |   descending saw |
|  gridsort |   100000 |  i32 | 0.002609 | 0.002781 |     1169464 |   descending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003915 | 0.004070 |     1012163 |      random tail |
|  gridsort |   100000 |  i32 | 0.002189 | 0.002223 |      626399 |      random tail |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.005674 | 0.005910 |     1200821 |      random half |
|  gridsort |   100000 |  i32 | 0.003854 | 0.003926 |     1005873 |      random half |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003028 | 0.003140 |     1209200 |           stable |
|  gridsort |   100000 |  i32 | 0.002122 | 0.002281 |      823079 |           stable |

