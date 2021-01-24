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
```cobol
                 ┌─────────────────┐┌──────────────────┐
                 │comparisons      ││swap memory       │
┌───────────────┐├─────┬─────┬─────┤├─────┬──────┬─────┤┌──────┐┌──────────┐┌──────────┐
│name           ││min  │avg  │max  ││min  │avg   │max  ││stable││partitions││comparison│
├───────────────┤├─────┼─────┼─────┤├─────┼──────┼─────┤├──────┤├──────────┤├──────────┤
│gridsort       ││n    │nᴸᴼᴳn│nᴸᴼᴳn││n    │n     │n    ││yes   ││yes       ││yes       │
├───────────────┤├─────┼─────┼─────┤├─────┼──────┼─────┤├──────┤├──────────┤├──────────┤
│mergesort      ││nᴸᴼᴳn│nᴸᴼᴳn│nᴸᴼᴳn││n    │n/ᴸᴼᴳn│n    ││yes   ││no        ││yes       │
├───────────────┤├─────┼─────┼─────┤├─────┼──────┼─────┤├──────┤├──────────┤├──────────┤
│quadsort       ││n    │nᴸᴼᴳn│nᴸᴼᴳn││1    │n/ᴸᴼᴳn│n    ││yes   ││no        ││yes       │
├───────────────┤├─────┼─────┼─────┤├─────┼──────┼─────┤├──────┤├──────────┤├──────────┤
│quicksort      ││n    │nᴸᴼᴳn│n²   ││1    │1     │1    ││no    ││yes       ││yes       │
└───────────────┘└─────┴─────┴─────┘└─────┴──────┴─────┘└──────┘└──────────┘└──────────┘
```

Gridsort makes n comparisons when the data is fully in order or in reverse order.

Visualization
-------------

In the visualization below one test is performed on a random distribution.

Cyan numbers are unsorted, green numbers are sorted, white numbers are sorted and ready to be
merged, yellow numbers are the index upon which a binary search is performed to find out where
to insert the next number, magenta numbers are ready to be merged back to the main array.

![gridsort visualization](https://github.com/scandum/gridsort/blob/main/gridsort.gif)

Benchmarks
----------
The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c.

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|     qsort |   100000 |  i32 | 0.008679 | 0.008971 |     1536634 |     random order |
|  gridsort |   100000 |  i32 | 0.006399 | 0.006681 |     1640493 |     random order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002283 | 0.002475 |      815024 |  ascending order |
|  gridsort |   100000 |  i32 | 0.000507 | 0.000520 |      200086 |  ascending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003076 | 0.003273 |      915019 |    ascending saw |
|  gridsort |   100000 |  i32 | 0.002468 | 0.002667 |      920331 |    ascending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.008673 | 0.008983 |     1536164 |    generic order |
|  gridsort |   100000 |  i32 | 0.006374 | 0.006599 |     1632479 |    generic order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002303 | 0.002504 |      853904 | descending order |
|  gridsort |   100000 |  i32 | 0.000506 | 0.000514 |      199329 | descending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002749 | 0.002951 |     1063907 |   descending saw |
|  gridsort |   100000 |  i32 | 0.002247 | 0.002459 |     1154496 |   descending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003918 | 0.004151 |     1012028 |      random tail |
|  gridsort |   100000 |  i32 | 0.002040 | 0.002072 |      626374 |      random tail |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.005686 | 0.005928 |     1200835 |      random half |
|  gridsort |   100000 |  i32 | 0.003732 | 0.003840 |     1004035 |      random half |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003006 | 0.003468 |     1209200 |           stable |
|  gridsort |   100000 |  i32 | 0.001790 | 0.002116 |      817453 |           stable |

