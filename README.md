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

[![cubesort visualization](https://github.com/scandum/gridsort/blob/main/cubesort.gif)](https://www.youtube.com/watch?v=DHC1qnV4mao)

In the visualization below one test is performed on a random distribution. This visualization more accurately shows the use of pointer operations to partition memory.

Cyan numbers are unsorted, green numbers are sorted, white numbers are sorted and ready to be
merged, yellow numbers are the index upon which a binary search is performed to find out where
to insert the next number, magenta numbers are ready to be merged back to the main array.

![gridsort visualization](https://github.com/scandum/gridsort/blob/main/gridsort.gif)

Benchmarks
----------

<details><summary>The following benchmark was on WSL gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04) using the [wolfsort](https://github.com/scandum/wolfsort) benchmark.
The source code was compiled using g++ -O3 -w -fpermissive bench.c. The std::sort() in the benchmark should be an in-place IntroSort.</summary>
|      Name |    Items | Type |     Best |  Average | Repetitions |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
| std::sort |  1000000 |  128 | 0.110756 | 0.111165 |           1 |     random order |
|  gridsort |  1000000 |  128 | 0.104944 | 0.105506 |           1 |     random order |

|      Name |    Items | Type |     Best |  Average | Repetitions |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
| std::sort |  1000000 |   64 | 0.065638 | 0.065886 |           1 |     random order |
|  gridsort |  1000000 |   64 | 0.054830 | 0.055245 |           1 |     random order |

|      Name |    Items | Type |     Best |  Average | Repetitions |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
| std::sort |  1000000 |   32 | 0.064990 | 0.065196 |           1 |     random order |
|  gridsort |  1000000 |   32 | 0.053937 | 0.054295 |           1 |     random order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.011424 | 0.011780 |           1 |  ascending order |
|  gridsort |  1000000 |   32 | 0.003396 | 0.003469 |           1 |  ascending order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.033669 | 0.033935 |           1 |    ascending saw |
|  gridsort |  1000000 |   32 | 0.013438 | 0.013535 |           1 |    ascending saw |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.030115 | 0.030396 |           1 |    generic order |
|  gridsort |  1000000 |   32 | 0.016028 | 0.016220 |           1 |    generic order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.008769 | 0.008950 |           1 | descending order |
|  gridsort |  1000000 |   32 | 0.003706 | 0.003814 |           1 | descending order |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.025817 | 0.026060 |           1 |   descending saw |
|  gridsort |  1000000 |   32 | 0.012219 | 0.012422 |           1 |   descending saw |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.044224 | 0.044493 |           1 |      random tail |
|  gridsort |  1000000 |   32 | 0.015914 | 0.016011 |           1 |      random tail |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.055512 | 0.055722 |           1 |      random half |
|  gridsort |  1000000 |   32 | 0.029508 | 0.029627 |           1 |      random half |
|           |          |      |          |          |             |                  |
| std::sort |  1000000 |   32 | 0.028018 | 0.028512 |           1 |         unstable |
|  gridsort |  1000000 |   32 | 0.012377 | 0.012522 |           1 |           stable |
</details>

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/gridsort_vs_stdsort.png)

<details><summary>The following benchmark was on WSL gcc version 7.4.0 (Ubuntu 7.4.0-1ubuntu1~18.04.1).
The source code was compiled using gcc -O3 bench.c. The stdlib qsort() in the benchmark is a mergesort variant.</summary>

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|     qsort |   100000 |  128 | 0.019153 | 0.020135 |     1536181 |     random order |
|  gridsort |   100000 |  128 | 0.013082 | 0.013162 |     1645873 |     random order |

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|     qsort |   100000 |   64 | 0.009234 | 0.009487 |     1536491 |     random order |
|  gridsort |   100000 |   64 | 0.006838 | 0.006893 |     1655053 |     random order |

|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |
| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |
|     qsort |   100000 |   32 | 0.008624 | 0.008857 |     1536634 |     random order |
|  gridsort |   100000 |   32 | 0.006446 | 0.006574 |     1649039 |     random order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.002276 | 0.002438 |      815024 |  ascending order |
|  gridsort |   100000 |   32 | 0.000697 | 0.000717 |      202485 |  ascending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.003065 | 0.003215 |      915019 |    ascending saw |
|  gridsort |   100000 |   32 | 0.002185 | 0.002204 |      639891 |    ascending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.006438 | 0.006661 |     1532339 |    generic order |
|  gridsort |   100000 |   32 | 0.002879 | 0.002920 |     1151399 |    generic order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.002308 | 0.002514 |      853904 | descending order |
|  gridsort |   100000 |   32 | 0.000713 | 0.000726 |      200036 | descending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.002736 | 0.002949 |     1063907 |   descending saw |
|  gridsort |   100000 |   32 | 0.001871 | 0.001916 |      841945 |   descending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.003904 | 0.004116 |     1012028 |      random tail |
|  gridsort |   100000 |   32 | 0.002155 | 0.002187 |      627734 |      random tail |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.005655 | 0.005864 |     1200835 |      random half |
|  gridsort |   100000 |   32 | 0.003736 | 0.003776 |     1001698 |      random half |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |   32 | 0.003005 | 0.003460 |     1209200 |           stable |
|  gridsort |   100000 |   32 | 0.001975 | 0.002004 |      867862 |           stable |
</details>

![gridsort vs stdsort](https://github.com/scandum/gridsort/blob/main/gridsort_vs_qsort.png)
