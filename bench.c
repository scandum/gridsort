/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	The person recognizes Mars as a free planet and that no Earth-based
	government has authority or sovereignty over Martian activities.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	To compile use:
	
	g++ -O3 -fpermissive bench.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include "quadsort.h"
#include "gridsort.h"

//#define cmp(a,b) (*(a) > *(b))

//typedef int CMPFUNC (const void *a, const void *b);

typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *ignore);


// Must prevent inlining so the benchmark is fair.
// Remove __attribute__ ((noinline)) and counter++ for full throttle.

long long counter;

__attribute__ ((noinline)) int cmp_int(const void * a, const void * b)
{
	counter++;

	return *(int *) a - *(int *) b;
}

__attribute__ ((noinline)) int cmp_stable(const void * a, const void * b)
{
	counter++;

	return *(int *) a / 1000 - *(int *) b / 1000;
}

__attribute__ ((noinline)) int cmp_long(const void * a, const void * b)
{
	counter++;

/*	if (*(long long *) a < *(long long *) b)
	{
		return -1;
	}
*/
	return *(long long *) a > *(long long *) b;
}

int cmp_str(const void * a, const void * b)
{
	return strcmp(*(const char **) a, *(const char **) b);
}

int cmp_float(const void * a, const void * b)
{
	return *(float *) a - *(float *) b;
}


long long utime()
{
	struct timeval now_time;

	gettimeofday(&now_time, NULL);

	return now_time.tv_sec * 1000000LL + now_time.tv_usec;
}

void seed_rand(unsigned long long seed)
{
	srand(seed);
}

void test_sort(void *array, void *unsorted, void *valid, int minimum, int maximum, int samples, int repetitions, SRTFUNC *srt, const char *name, const char *desc, size_t size, CMPFUNC *cmpf)
{
	long long start, end, total, best, average;
	size_t rep, sam, max;
	long long *ptla = (long long *) array;
	int *pta = (int *) array, *ptv = (int *) valid, cnt;

	best = average = 0;

	for (sam = 0 ; sam < samples ; sam++)
	{
		total = 0;

		max = minimum;

		start = utime();

		for (rep = 0 ; rep < repetitions ; rep++)
		{
			memcpy(array, unsorted, max * size);

			counter = 0;

			if (max == 10 && sam == 0 && rep == 0)
				printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n",
					pta[0], pta[1], pta[2], pta[3], pta[4],
					pta[5], pta[6], pta[7], pta[8], pta[9]);

			switch (*name)
			{
				case 'q':
					if (name[1] == 'u')
					{
						quadsort(array, max, size, cmpf);
					}
					else
					{
						qsort(array, max, size, cmpf);
					}
					break;

				case 'g':
					gridsort(array, max, size, cmpf);
					break;

			}
			if (minimum < maximum)
			{
				max++;
			
				if (max > maximum)
				{
					max = minimum;
				}
			}
/*			if (max == 10 && sam == 0 && rep == 0)
			{
				printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6], pta[7], pta[8], pta[9]);
			}
*/
		}
		end = utime();

		total = end - start;

		if (!best || total < best)
		{
			best = total;
		}
		average += total;
	}

	if (repetitions == 0)
	{
		return;
	}

	average /= samples;

	if (cmpf == cmp_stable)
	{
		for (cnt = 1 ; cnt < maximum ; cnt++)
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				if (maximum == 1000)
				{
					printf("%17s: sorted %7d i%ds in %f seconds. KO: %5lld (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else if (maximum == 1000000)
				{
					printf("%17s: sorted %7d i%ds in %f seconds. MO: %10lld (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else
				{
					printf("%17s: sorted %7d i%ds in %f seconds. (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, desc);
				}
				return;
			}
		}
	}

	if (name[1] == 's')
	{
		if (!strcmp(desc, "random order") || !strcmp(desc, "random 1-4"))
		{
			if (counter)
			{
				printf("%s\n", "|      Name |    Items | Type |     Best |  Average | Comparisons |     Distribution |");
				printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |");
			}
			else
			{
				printf("%s\n", "|      Name |    Items | Type |     Best |  Average | Repetitions |     Distribution |");
				printf("%s\n", "| --------- | -------- | ---- | -------- | -------- | ----------- | ---------------- |");
			}
		}
		else
		{
			printf("%s\n", "|           |          |      |          |          |             |                  |");
		}
	}

	if (counter)
	{
		printf("|%10s | %8d |  i%d | %f | %f | %11lld | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, counter, desc);
	}
	else
	{
		printf("|%10s | %8d |  i%d | %f | %f | %11d | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, repetitions, desc);
	}

	if (minimum != maximum)
	{
		return;
	}

	for (cnt = 1 ; cnt < maximum ; cnt++)
	{
		if (size == sizeof(int))
		{
			if (pta[cnt - 1] > pta[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%d vs %d\n", name, cnt, pta[cnt - 1], pta[cnt]);
				break;
			}
			if (pta[cnt - 1] == pta[cnt])
			{
//				printf("%17s: Found a repeat value at index %d. (%d)\n", name, cnt, pta[cnt]);
			}
		}
		else if (size == sizeof(long long))
		{
			if (ptla[cnt - 1] > ptla[cnt])
			{
				printf("%17s: not properly sorted at index %d. (%lld vs %lld\n", name, cnt, ptla[cnt - 1], ptla[cnt]);
				break;
			}
		}
	}

	for (cnt = 1 ; cnt < maximum ; cnt++)
	{
		if (pta[cnt] != ptv[cnt])
		{
			printf("         validate: array[%d] != valid[%d]. (%d vs %d\n", cnt, cnt, pta[cnt], ptv[cnt]);
			break;
		}
	}
}

void validate()
{
	int seed = time(NULL);
	int cnt, val, max = 2000000;

	int *a_array, *r_array, *v_array;

	seed_rand(seed);

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}


	for (cnt = 1 ; cnt < 100 ; cnt++)
	{
		memcpy(a_array, r_array, max * sizeof(int));
		memcpy(v_array, r_array, max * sizeof(int));

		quadsort(a_array, cnt, sizeof(int), cmp_int);
		qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// ascending saw

	for (cnt = 0 ; cnt < 1000 ; cnt++)
	{
		r_array[cnt] = rand();
	}

        quadsort(r_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
        quadsort(r_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate ascending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

        // descending saw

        for (cnt = 0 ; cnt < 1000 ; cnt++)
        {
                r_array[cnt] = (max - cnt - 1) % 100000;
        }

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate descending saw: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	// random tail

	for (cnt = 0 ; cnt < max * 3 / 4 ; cnt++)
	{
		r_array[cnt] = cnt;
	}

	for (cnt = max * 3 / 4 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

        for (cnt = 1 ; cnt < 1000 ; cnt += 7)
	{
		memcpy(a_array, r_array, max * sizeof(int));
	        memcpy(v_array, r_array, max * sizeof(int));

	        quadsort(a_array, cnt, sizeof(int), cmp_int);
	        qsort(v_array, cnt, sizeof(int), cmp_int);

		for (val = 0 ; val < cnt ; val++)
		{
			if (val && v_array[val - 1] > v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not properly sorted at index %d.\n", seed, cnt, val);
				return;
			}

			if (a_array[val] != v_array[val])
			{
				printf("\e[1;31mvalidate rand tail: seed %d: size: %d Not verified at index %d.\n", seed, cnt, val);
				return;
			}
		}
	}

	free(a_array);
	free(r_array);
	free(v_array);
}


int main(int argc, char **argv)
{
	int max = 100000;
	int samples = 10;
	int repetitions = 1;
	int seed = 0;
	int cnt, rnd;
	int *a_array, *r_array, *v_array;
	long long *la_array, *lr_array, *lv_array;

//	char *sorts[] = { "stablesort", "quadsort", "timsort", "pdqsort", "wolfsort", "flowsort", "gridsort", "vergesort" };
	char *sorts[] = { "qsort", "quadsort", "gridsort" };

	if (argc >= 1 && argv[1] && *argv[1])
	{
		max = atoi(argv[1]);
	}

	if (argc >= 2 && argv[2] && *argv[2])
	{
		samples = atoi(argv[2]);
	}

	if (argc >= 3 && argv[3] && *argv[3])
	{
		repetitions = atoi(argv[3]);
	}

	if (argc >= 4 && argv[4] && *argv[4])
	{
		seed = atoi(argv[4]);
	}

	validate();

	rnd = seed ? seed : time(NULL);

	// 64 bit

	la_array = (long long *) malloc(max * sizeof(long long));
	lr_array = (long long *) malloc(max * sizeof(long long));
	lv_array = (long long *) malloc(max * sizeof(long long));

	if (la_array == NULL || lr_array == NULL || lv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	printf("Benchmark: array size: %d, samples: %d, repetitions: %d, seed: %d\n\n", max, samples, repetitions, rnd);

	// random 

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		lr_array[cnt] = rand();
		lr_array[cnt] += (unsigned long long) rand() << 32ULL;
	}

	memcpy(lv_array, lr_array, max * sizeof(long long));
	quadsort(lv_array, max, sizeof(long long), cmp_long);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(la_array, lr_array, lv_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(long long), cmp_long);
	}

	printf("\n");

	// 32 bit

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));


	// random

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random order", sizeof(int), cmp_int);
	}

	// ascending

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = cnt;
	}

        memcpy(v_array, r_array, max * sizeof(int));
        memcpy(r_array, v_array, max * sizeof(int));

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "ascending order", sizeof(int), cmp_int);
	}

	// ascending saw

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array + max / 4 * 0, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 1, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 2, max / 4, sizeof(int), cmp_int);
        quadsort(v_array + max / 4 * 3, max / 4, sizeof(int), cmp_int);
        memcpy(r_array, v_array, max * sizeof(int));

	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "ascending saw", sizeof(int), cmp_int);
	}

	// uniform

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "generic order", sizeof(int), cmp_int);
	}

        // descending

        for (cnt = 0 ; cnt < max ; cnt++)
        {
                r_array[cnt] = (max - cnt);
        }

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "descending order", sizeof(int), cmp_int);
	}

        // descending saw

        for (cnt = 0 ; cnt < max ; cnt++)
        {
                r_array[cnt] = (max - cnt - 1) % 10000;
        }

        memcpy(v_array, r_array, max * sizeof(int));
        quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "descending saw", sizeof(int), cmp_int);
	}

	// random tail

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max * 3 / 4, sizeof(int), cmp_int);
	memcpy(r_array, v_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random tail", sizeof(int), cmp_int);
	}

	seed_rand(rnd);

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand();
	}

        memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max / 2, sizeof(int), cmp_int);

	memcpy(r_array, v_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "random half", sizeof(int), cmp_int);
	}

	// wave

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		if (cnt % 2 == 0)
		{
			r_array[cnt] = 16777216 + cnt;
		}
		else
		{
			r_array[cnt] = 33554432 + cnt;
		}
	}

	memcpy(v_array, r_array, max * sizeof(int));
	quadsort(v_array, max, sizeof(int), cmp_int);

	for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
	{
		test_sort(a_array, r_array, v_array, max, max, samples, repetitions, qsort, sorts[cnt], "stable", sizeof(int), cmp_int);
	}

	if (repetitions > 0)
	{
		goto end;
	}

	// random range

	if (max >= 4)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 4 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 8 * sizeof(int));
		quadsort(v_array, 8, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 1, 4, 1000, 8192, quadsort,        "stablesort",          "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 1000, 8192, quadsort,        "quadsort",          "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 1000, 8192, qsort,           "timsort",             "random 1-4", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1, 4, 1000, 8192, qsort,           "wolfsort",             "random 1-4", sizeof(int), cmp_int);
	}

	if (max >= 8)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 8 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 8 * sizeof(int));
		quadsort(v_array, 8, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 5, 8, 100, 8192, quadsort,        "stablesort",          "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 100, 8192, quadsort,        "quadsort",          "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 100, 8192, qsort,           "timsort",             "random 5-8", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 5, 8, 100, 8192, quadsort,        "wolfsort",          "random 5-8", sizeof(int), cmp_int);

	}

	if (max >= 16)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 16 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 16 * sizeof(int));
		quadsort(v_array, 16, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 9, 15, 100, 8192, quadsort,        "stablesort",          "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 100, 8192, quadsort,        "quadsort",          "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 100, 8192, qsort,           "timsort",             "random 9-15", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 9, 15, 100, 8192, qsort,           "wolfsort",             "random 9-15", sizeof(int), cmp_int);
	}

	if (max >= 64)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 64 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 64 * sizeof(int));
		quadsort(v_array, 64, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 16, 63, 100, 4096, quadsort,        "stablesort",        "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 100, 4096, quadsort,        "quadsort",        "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 100, 4096, qsort,           "timsort",           "random 16-63", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 16, 63, 100, 4096, qsort,           "wolfsort",           "random 16-63", sizeof(int), cmp_int);

	}

	if (max >= 128)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 128 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 128 * sizeof(int));
		quadsort(v_array, 128, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 64, 127, 100, 2048, quadsort,        "stablesort",        "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 100, 2048, quadsort,        "quadsort",        "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 100, 2048, qsort,           "timsort",           "random 64-127", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 64, 127, 100, 2048, qsort,           "wolfsort",           "random 64-127", sizeof(int), cmp_int);
	}

	if (max >= 256)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 256 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 256 * sizeof(int));
		quadsort(v_array, 256, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 127, 255, 100, 1024, quadsort,        "stablesort",        "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 100, 1024, quadsort,        "quadsort",        "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 100, 1024, qsort,           "timsort",           "random 128-255", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 127, 255, 100, 1024, qsort,           "wolfsort",           "random 128-255", sizeof(int), cmp_int);
	}

	if (max >= 512)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 512 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 512 * sizeof(int));
		quadsort(v_array, 512, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 256, 511, 100, 512, quadsort,        "stablesort",        "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 100, 512, quadsort,        "quadsort",        "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 100, 512, qsort,           "timsort",           "random 256-511", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 256, 511, 100, 512, qsort,           "wolfsort",           "random 256-511", sizeof(int), cmp_int);
	}

	if (max >= 1024)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 1024 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 1024 * sizeof(int));
		quadsort(v_array, 1024, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 512, 1023, 100, 256, quadsort,        "stablesort",        "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 100, 256, quadsort,        "quadsort",        "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 100, 256, qsort,           "timsort",           "random 512-1023", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 512, 1023, 100, 256, qsort,           "wolfsort",           "random 512-1023", sizeof(int), cmp_int);
	}

	if (max >= 2048)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 2048 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 2048 * sizeof(int));
		quadsort(v_array, 2048, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 1024, 2047, 100, 128, quadsort,        "stablesort",          "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 100, 128, quadsort,        "quadsort",          "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 100, 128, qsort,           "timsort",             "random 1024-2047", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 1024, 2047, 100, 128, qsort,           "wolfsort",             "random 1024-2047", sizeof(int), cmp_int);
	}

	if (max >= 4096)
	{
		srand(rnd);

		for (cnt = 0 ; cnt < 4096 ; cnt++)
		{
			r_array[cnt] = rand();
		}

		memcpy(v_array, r_array, 4096 * sizeof(int));
		quadsort(v_array, 4096, sizeof(int), cmp_int);

		test_sort(a_array, r_array, v_array, 2048, 4095, 100, 64, quadsort,        "stablesort",          "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 100, 64, quadsort,        "quadsort",          "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 100, 64, qsort,           "timsort",             "random 2048-4095", sizeof(int), cmp_int);
		test_sort(a_array, r_array, v_array, 2048, 4095, 100, 64, qsort,           "wolfsort",             "random 2048-4095", sizeof(int), cmp_int);
	}

	end:

	free(la_array);
	free(lr_array);
	free(lv_array);

	free(a_array);
	free(r_array);
	free(v_array);

	return 0;
}
