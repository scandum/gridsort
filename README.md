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

Memory
------
Gridsort allocates 2 * sqrt(n) blocks of memory of sqrt(n) size.

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
People wanting to port gridsort might want to have a look at [twinsort](https://github.com/scandum/twinsort), which is a simplified implementation of quadsort. Gridsort itself is a simplified implementation of cubesort. Another sort worth looking at is [fluxsort](https://github.com/scandum/wolfsort#fluxsort) which uses simpler top-down partitioning instead of gridsort's bottom-up partitioning. 

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
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The bar graph shows the best run out of 10 on 32 bit integers. Comparisons for gridsort and std::stable_sort are inlined.

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/images/gridsort_vs_stdsort.png)

<details><summary>data table</summary>

  
|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |  1000000 |  128 | 0.141087 | 0.142480 |         1 |      10 |     random order |
|  gridsort |  1000000 |  128 | 0.101785 | 0.103576 |         1 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |  1000000 |   64 | 0.076895 | 0.077535 |         1 |      10 |     random order |
|  gridsort |  1000000 |   64 | 0.051316 | 0.051824 |         1 |      10 |     random order |

|      Name |    Items | Type |     Best |  Average |     Loops | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|stablesort |  1000000 |   32 | 0.072891 | 0.073191 |         1 |      10 |     random order |
|  gridsort |  1000000 |   32 | 0.050891 | 0.051147 |         1 |      10 |     random order |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.010608 | 0.010848 |         1 |      10 |  ascending order |
|  gridsort |  1000000 |   32 | 0.003287 | 0.003543 |         1 |      10 |  ascending order |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.017488 | 0.017697 |         1 |      10 |    ascending saw |
|  gridsort |  1000000 |   32 | 0.012015 | 0.012160 |         1 |      10 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.041584 | 0.041741 |         1 |      10 |    generic order |
|  gridsort |  1000000 |   32 | 0.015809 | 0.016224 |         1 |      10 |    generic order |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.010641 | 0.010863 |         1 |      10 | descending order |
|  gridsort |  1000000 |   32 | 0.003577 | 0.003620 |         1 |      10 | descending order |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.014260 | 0.014466 |         1 |      10 |   descending saw |
|  gridsort |  1000000 |   32 | 0.009878 | 0.010047 |         1 |      10 |   descending saw |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.026343 | 0.026791 |         1 |      10 |      random tail |
|  gridsort |  1000000 |   32 | 0.015311 | 0.015351 |         1 |      10 |      random tail |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.043529 | 0.043730 |         1 |      10 |      random half |
|  gridsort |  1000000 |   32 | 0.028233 | 0.028463 |         1 |      10 |      random half |
|           |          |      |          |          |           |         |                  |
|stablesort |  1000000 |   32 | 0.013824 | 0.013961 |         1 |      10 |  ascending tiles |
|  gridsort |  1000000 |   32 | 0.012287 | 0.012459 |         1 |      10 |  ascending tiles |

</details>

The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c. The bar graph shows the best run out of 100 on 32 bit integers. Comparisons for gridsort and qsort are not inlined. The stdlib qsort() in the benchmark is a mergesort variant. 

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/images/gridsort_vs_qsort.png)

<details><summary>data table</summary>

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |  128 | 0.019117 | 0.019996 |   1536181 |     100 |     random order |
|  gridsort |   100000 |  128 | 0.012686 | 0.012730 |   1639247 |     100 |     random order |

|      Name |    Items | Type |     Best |  Average |  Compares | Samples |     Distribution |
| --------- | -------- | ---- | -------- | -------- | --------- | ------- | ---------------- |
|     qsort |   100000 |   32 | 0.008465 | 0.008580 |   1536634 |     100 |     random order |
|  gridsort |   100000 |   32 | 0.006045 | 0.006135 |   1643169 |     100 |     random order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002019 | 0.002113 |    815024 |     100 |  ascending order |
|  gridsort |   100000 |   32 | 0.000656 | 0.000669 |    202740 |     100 |  ascending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002818 | 0.002827 |    915019 |     100 |    ascending saw |
|  gridsort |   100000 |   32 | 0.001976 | 0.001994 |    582000 |     100 |    ascending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.006402 | 0.006545 |   1532339 |     100 |    generic order |
|  gridsort |   100000 |   32 | 0.002793 | 0.002948 |   1146648 |     100 |    generic order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002450 | 0.002566 |    853904 |     100 | descending order |
|  gridsort |   100000 |   32 | 0.000661 | 0.000668 |    200034 |     100 | descending order |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.002827 | 0.002941 |   1063907 |     100 |   descending saw |
|  gridsort |   100000 |   32 | 0.001666 | 0.001737 |    771299 |     100 |   descending saw |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.003734 | 0.003943 |   1012028 |     100 |      random tail |
|  gridsort |   100000 |   32 | 0.002057 | 0.002142 |    625065 |     100 |      random tail |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.005442 | 0.005595 |   1200835 |     100 |      random half |
|  gridsort |   100000 |   32 | 0.003511 | 0.003573 |    998798 |     100 |      random half |
|           |          |      |          |          |           |         |                  |
|     qsort |   100000 |   32 | 0.004080 | 0.004179 |   1209200 |     100 |  ascending tiles |
|  gridsort |   100000 |   32 | 0.003189 | 0.003343 |    868332 |     100 |  ascending tiles |

</details>
