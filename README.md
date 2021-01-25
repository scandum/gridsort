Intro
-----
This document describes a partitioning stable comparison sort named 
sort. It is a simplified version of cubesort, intended to be helpful for people wanting to understand or port the code.

Binary Cube
-----------
Gridsort sorts data by storing data in a simplified [binary cube](https://github.com/scandum/binary_cube), a multidimentional sorted array. The binary cube offers excellent cache utilization.

Boundless Binary Search
-----------------------
The first step when sorting an element is a [boundless binary search](https://github.com/scandum/binary_search) to pin point the bucket where the element should be stored. A boundless binary search is up to two times faster than the legacy binary search used by most applications. The binary search is optimized to speed up searches at the start or the end of the index. Each element is added to the end of the bucket.

Quadsort
--------
Once a bucket overflows it is sorted using [quadsort](https://github.com/scandum/quadsort) and a new bucket is created. The sorted data is split between the two buckets so each bucket is half full.

Quadsort has exceptional performance when sorting arrays with fewer than 1000 elements. People wanting to port gridsort might want to use [tailsort](https://github.com/scandum/quadsort) instead which is a simplified implementation of quadsort.

Finish
------
Once all elements have been inserted into the binary cube every bucket receives a final sort and is copied back to the original array.

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

In the visualization below eight tests are performed. Random, Ascending, Ascending Saw, Generic,
Descending, Descending Saw, Random Tail, and Wave order.
                           
[![cubesort visualization](https://github.com/scandum/gridsort/blob/main/cubesort.gif)](https://www.youtube.com/watch?v=DHC1qnV4mao)

Benchmarks
----------
The following benchmark was on WSL gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) using the [wolfsort](https://github.com/scandum/wolfsort) benchmark.
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The std::sort() in the benchmark should be an in-place IntroSort.

|      Name |    Items | Type |     Best |  Average | Repetitions |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
| std::sort |  1000000 |  i32 | 0.065251 | 0.065579 |           1 |     random order |
|  gridsort |  1000000 |  i32 | 0.053845 | 0.054123 |           1 |     random order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.011072 | 0.011364 |           1 |  ascending order |
|  gridsort |  1000000 |  i32 | 0.003245 | 0.003337 |           1 |  ascending order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.033779 | 0.033952 |           1 |    ascending saw |
|  gridsort |  1000000 |  i32 | 0.018176 | 0.018301 |           1 |    ascending saw |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.030247 | 0.030533 |           1 |    generic order |
|  gridsort |  1000000 |  i32 | 0.015128 | 0.015355 |           1 |    generic order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.008549 | 0.008759 |           1 | descending order |
|  gridsort |  1000000 |  i32 | 0.003593 | 0.003695 |           1 | descending order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.026028 | 0.026215 |           1 |   descending saw |
|  gridsort |  1000000 |  i32 | 0.017865 | 0.018191 |           1 |   descending saw |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.044885 | 0.045100 |           1 |      random tail |
|  gridsort |  1000000 |  i32 | 0.015727 | 0.015829 |           1 |      random tail |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.056019 | 0.056235 |           1 |      random half |
|  gridsort |  1000000 |  i32 | 0.029440 | 0.029587 |           1 |      random half |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |  i32 | 0.028506 | 0.028893 |           1 |       wave order |
|  gridsort |  1000000 |  i32 | 0.015190 | 0.015785 |           1 |       wave order |

The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c. The stdlib qsort() in the benchmark is a mergesort variant.

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


