Intro
-----
This document describes a partitioning stable comparison-based sort named 
gridsort.

Binary Cube
-----------
Gridsort sorts data by storing data in a simplified [binary cube](https://github.com/scandum/binary_cube), a multidimentional sorted array. The binary cube offers excellent cache utilization. It's easiest to view a binary cube as a hash table, but instead of a hash function to find a bucket it uses a binary search on a lookup table.

Boundless Binary Search
-----------------------
The first step when sorting an element is a [boundless binary search](https://github.com/scandum/binary_search) to pin point the bucket where the element should be stored. A boundless binary search is up to two times faster than the legacy binary search used by most applications. Once a bucket is found the element is added to the end of the bucket.

Quadsort
--------
Once a bucket overflows it is sorted using [quadsort](https://github.com/scandum/quadsort) and a new bucket is created. The sorted data is split between the two buckets so each bucket is half full. The lowest element in each bucket is added to the lookup table.

Finish
------
Once all elements have been inserted into the binary cube every bucket receives a final sort and is copied back to the original array.

Data Types
----------
The C implementation of gridsort supports long doubles and 8, 16, 32, and 64 bit data types. By using pointers it's possible to sort any other data type.

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

Porting
-------
People wanting to port gridsort might want to have a look at [tailsort](https://github.com/scandum/tailsort), which is a simplified implementation of quadsort. Gridsort itself is a simplified implementation of cubesort.

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
|     qsort |   100000 |  i32 | 0.008652 | 0.008873 |     1536226 |     random order |
|  gridsort |   100000 |  i32 | 0.006340 | 0.006469 |     1649235 |     random order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002278 | 0.002463 |      815024 |  ascending order |
|  gridsort |   100000 |  i32 | 0.000542 | 0.000549 |      202097 |  ascending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003065 | 0.003181 |      915016 |    ascending saw |
|  gridsort |   100000 |  i32 | 0.002765 | 0.002791 |      997908 |    ascending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.006411 | 0.006618 |     1532490 |    generic order |
|  gridsort |   100000 |  i32 | 0.002723 | 0.002783 |     1145540 |    generic order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002304 | 0.002404 |      853904 | descending order |
|  gridsort |   100000 |  i32 | 0.000561 | 0.000573 |      200036 | descending order |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002735 | 0.002881 |     1063907 |   descending saw |
|  gridsort |   100000 |  i32 | 0.002621 | 0.002692 |     1267256 |   descending saw |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.003904 | 0.004122 |     1012031 |      random tail |
|  gridsort |   100000 |  i32 | 0.002060 | 0.002078 |      627248 |      random tail |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.005647 | 0.005843 |     1200664 |      random half |
|  gridsort |   100000 |  i32 | 0.003627 | 0.003655 |      999069 |      random half |
|           |          |      |          |          |             |                  |
|     qsort |   100000 |  i32 | 0.002996 | 0.003387 |     1209200 |           stable |
|  gridsort |   100000 |  i32 | 0.002067 | 0.002110 |      917156 |           stable |
