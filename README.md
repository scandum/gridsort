Intro
-----
This document describes a partitioning stable adaptive comparison-based sort named gridsort.

Binary Cube
-----------
Gridsort sorts data by storing data in a simplified [binary cube](https://github.com/scandum/binary_cube), a multidimentional sorted array. The binary cube offers excellent cache utilization. It's easiest to view a binary cube as a hash table, but instead of a hash function to find a bucket it uses a binary search on a lookup table.

Boundless Binary Search
-----------------------
The first step when sorting an element is a [boundless binary search](https://github.com/scandum/binary_search) to pin point the bucket where the element should be stored. A boundless binary search is up to two times faster than the legacy binary search used by most applications. Once a bucket is found the element is added to the end of the bucket.

Gridsort switches to an adaptive binary search when it detects data that is already sorted.

Quadsort
--------
Once a bucket overflows it is sorted using [quadsort](https://github.com/scandum/quadsort) and a new bucket is created. The sorted data is split between the two buckets so each bucket is half full. The lowest element in each bucket is added to the lookup table.

Finish
------
Once all elements have been inserted into the binary cube every bucket receives a final sort and is copied back to the original array.

Data Types
----------
The C implementation of gridsort supports long doubles and 8, 16, 32, and 64 bit data types. By using pointers it's possible to sort any other data type.

Interface
---------
Gridsort uses the same interface as qsort, which is described in [man qsort](https://man7.org/linux/man-pages/man3/qsort.3p.html).

Big O
-----
```cobol
                 ┌───────────────────────┐┌───────────────────────┐
                 │comparisons            ││swap memory            │
┌───────────────┐├───────┬───────┬───────┤├───────┬───────┬───────┤┌──────┐┌─────────┐┌─────────┐
│name           ││min    │avg    │max    ││min    │avg    │max    ││stable││partition││adaptive │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│gridsort       ││n      │n log n│n log n││n      │n      │n      ││yes   ││yes      ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│mergesort      ││n log n│n log n│n log n││n      │n      │n      ││yes   ││no       ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quadsort       ││n      │n log n│n log n││1      │n      │n      ││yes   ││no       ││yes      │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│quicksort      ││n      │n log n│n²     ││1      │1      │1      ││no    ││yes      ││no       │
├───────────────┤├───────┼───────┼───────┤├───────┼───────┼───────┤├──────┤├─────────┤├─────────┤
│introsort      ││n log n│n log n│n log n││1      │1      │1      ││no    ││yes      ││no       │
└───────────────┘└───────┴───────┴───────┘└───────┴───────┴───────┘└──────┘└─────────┘└─────────┘
```

Gridsort makes n comparisons when the data is fully in order or in reverse order.

Porting
-------
People wanting to port gridsort might want to have a look at [twinsort](https://github.com/scandum/twinsort), which is a simplified implementation of quadsort. Gridsort itself is a simplified implementation of cubesort.

Visualization
-------------
In the visualization below eight tests are performed. Random, Ascending, Ascending Saw, Generic,
Descending, Descending Saw, Random Tail, and Wave order.

[![cubesort visualization](https://github.com/scandum/gridsort/blob/main/images/cubesort.gif)](https://www.youtube.com/watch?v=DHC1qnV4mao)

In the visualization below one test is performed on a random distribution. This visualization more accurately shows the use of pointer operations to partition memory.

Cyan numbers are unsorted, green numbers are sorted, white numbers are sorted and ready to be
merged, yellow numbers are the index upon which a binary search is performed to find out where
to insert the next number, magenta numbers are ready to be merged back to the main array.

![gridsort visualization](https://github.com/scandum/gridsort/blob/main/images/gridsort.gif)

Benchmarks
----------

The following benchmark was on WSL gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) using the [wolfsort](https://github.com/scandum/wolfsort) benchmark.
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The bar graph shows the best run out of 100 on 32 bit integers. Comparisons for gridsort and std::sort are inlined. The std::sort() in the benchmark should be an in-place IntroSort. 

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/images/gridsort_vs_stdsort.png)

<details><summary>data table</summary>

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |  1000000 |  128 | 0.110579 | 0.110943 |         1 |     100 |     random order |
|  gridsort |  1000000 |  128 | 0.105021 | 0.105474 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |  1000000 |   64 | 0.065856 | 0.066048 |         1 |     100 |     random order |
|  gridsort |  1000000 |   64 | 0.054824 | 0.055161 |         1 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
| std::sort |  1000000 |   32 | 0.065065 | 0.065391 |         1 |     100 |     random order |
|  gridsort |  1000000 |   32 | 0.053922 | 0.054189 |         1 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.011443 | 0.011800 |         1 |     100 |  ascending order |
|  gridsort |  1000000 |   32 | 0.003463 | 0.003526 |         1 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.033698 | 0.033884 |         1 |     100 |    ascending saw |
|  gridsort |  1000000 |   32 | 0.013591 | 0.013691 |         1 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.030675 | 0.030970 |         1 |     100 |    generic order |
|  gridsort |  1000000 |   32 | 0.015841 | 0.016119 |         1 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.008789 | 0.009153 |         1 |     100 | descending order |
|  gridsort |  1000000 |   32 | 0.003662 | 0.003750 |         1 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.026253 | 0.026449 |         1 |     100 |   descending saw |
|  gridsort |  1000000 |   32 | 0.012359 | 0.012535 |         1 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.044365 | 0.044593 |         1 |     100 |      random tail |
|  gridsort |  1000000 |   32 | 0.015910 | 0.016008 |         1 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.055819 | 0.056025 |         1 |     100 |      random half |
|  gridsort |  1000000 |   32 | 0.029525 | 0.029700 |         1 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
| std::sort |  1000000 |   32 | 0.027889 | 0.028305 |         1 |     100 |  ascending tiles |
|  gridsort |  1000000 |   32 | 0.012352 | 0.012592 |         1 |     100 |  ascending tiles |
</details>

The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c. The bar graph shows the best run out of 100 on 32 bit integers. Comparisons for gridsort and qsort are not inlined. The stdlib qsort() in the benchmark is a mergesort variant. 

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/images/gridsort_vs_qsort.png)

<details><summary>data table</summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.019332 | 0.020187 |   1536181 |     100 |     random order |
|  gridsort |   100000 |  128 | 0.013077 | 0.013145 |   1645784 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   64 | 0.009379 | 0.009614 |   1536491 |     100 |     random order |
|  gridsort |   100000 |   64 | 0.007207 | 0.007287 |   1654963 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008563 | 0.008838 |   1536634 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.006496 | 0.006611 |   1648950 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002268 | 0.002402 |    815024 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000685 | 0.000695 |    202485 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003044 | 0.003232 |    915019 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.002210 | 0.002243 |    639757 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006369 | 0.006623 |   1532339 |     100 |    generic order |
|  gridsort |   100000 |   32 | 0.002942 | 0.003046 |   1151338 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002314 | 0.002538 |    853904 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000661 | 0.000677 |    200036 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002754 | 0.002995 |   1063907 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.001940 | 0.002044 |    841084 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003875 | 0.004092 |   1012028 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.002176 | 0.002206 |    627704 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005599 | 0.005844 |   1200835 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.003740 | 0.003799 |   1001582 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003878 | 0.004274 |   1209200 |     100 |  ascending tiles |
|  gridsort |   100000 |   32 | 0.003160 | 0.003267 |    867858 |     100 |  ascending tiles |
</details>
