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
	
	gcc -O3 bench.c
	
	or
	
	g++ -O3 -w -fpermissive bench.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "quadsort.h"
#include "gridsort.h"

//#define cmp(a,b) (*(a) > *(b))

//typedef int CMPFUNC (const void *a, const void *b);

typedef void SRTFUNC(void *array, size_t nmemb, size_t size, CMPFUNC *ignore);


// Must prevent inlining so the benchmark is fair against qsort.

// Remove __attribute__ ((noinline)) and counter++ for full throttle.

size_t counter;

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

/*	if ((*(long long *) a > *(long long *) b) - (*(long long *) a < *(long long *) b)
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

	if (*name == '*')
	{
		if (!strcmp(desc, "random order") || !strcmp(desc, "random 1-4") || !strcmp(desc, "random 8192"))
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
		return;
	}

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

			if (maximum == 10 && minimum == 10 && sam == 0 && rep == 0)
			{
				printf("\e[1;31m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6], pta[7], pta[8], pta[9]);
			}

			switch (*name)
			{
				case 'g':
					gridsort(array, max, size, cmpf);
					break;

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
			}

			if (minimum < maximum)
			{
				max++;
			
				if (max > maximum)
				{
					max = minimum;
				}
			}

			if (maximum == 10 && minimum == 10 && sam == 0 && rep == 0)
			{
				printf("\e[1;32m%10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\e[0m\n", pta[0], pta[1], pta[2], pta[3], pta[4], pta[5], pta[6], pta[7], pta[8], pta[9]);
			}
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
					printf("%17s: sorted %7d i%ds in %f seconds. KO: %5lu (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else if (maximum == 1000000)
				{
					printf("%17s: sorted %7d i%ds in %f seconds. MO: %10lu (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, counter, desc);
				}
				else
				{
					printf("%17s: sorted %7d i%ds in %f seconds. (un%s)\n", name, maximum, (int) size * 8, best / 1000000.0, desc);
				}
				return;
			}
		}
	}

	if (counter)
	{
		printf("|%10s | %8d |  i%d | %f | %f | %11lu | %16s |\n", name, maximum, (int) size * 8, best / 1000000.0, average / 1000000.0, counter, desc);
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
	int cnt, rnd, lst;
	int *a_array, *r_array, *v_array;
	long long *la_array, *lr_array, *lv_array;

	char dist[40], *sorts[] = { "*", "qsort", "quadsort", "gridsort" };

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

	a_array = (int *) malloc(max * sizeof(int));
	r_array = (int *) malloc(max * sizeof(int));
	v_array = (int *) malloc(max * sizeof(int));

	la_array = (long long *) malloc(max * sizeof(long long));
	lr_array = (long long *) malloc(max * sizeof(long long));
	lv_array = (long long *) malloc(max * sizeof(long long));

	if (la_array == NULL || lr_array == NULL || lv_array == NULL)
	{
		printf("main(%d,%d,%d): malloc: %s\n", max, samples, repetitions, strerror(errno));

		return 0;
	}

	printf("Benchmark: array size: %d, samples: %d, repetitions: %d, seed: %d\n\n", max, samples, repetitions, rnd);

	if (samples == 0 && repetitions == 0)
	{
		goto small_range_test;
	}

	// 64 bit

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

	// generic

	for (cnt = 0 ; cnt < max ; cnt++)
	{
		r_array[cnt] = rand() % 100;
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

	small_range_test:

	if (max >= 8192)
	{
		goto large_range_test;
	}

	for (lst = 1, samples = 32768, repetitions = 4 ; repetitions <= 4096 ; repetitions *= 2, samples /= 2)
	{
		if (max >= repetitions)
		{
			sprintf(dist, "random %d-%d", lst, repetitions);

			srand(rnd);

			for (cnt = 0 ; cnt < repetitions ; cnt++)
			{
				r_array[cnt] = rand();
			}

			memcpy(v_array, r_array, repetitions * sizeof(int));
			quadsort(v_array, repetitions, sizeof(int), cmp_int);

			for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
			{
				test_sort(a_array, r_array, v_array, lst, repetitions, 100, samples, qsort,           sorts[cnt],             dist, sizeof(int), cmp_int);
			}
			lst = repetitions + 1;
		}
	}

	goto end;

	large_range_test:

	for (samples = 94, repetitions = 8192 ; repetitions < 1000000000 ; repetitions *= 2, samples = (samples + 1) / 3 * 2)
	{
		if (max >= repetitions)
		{
			srand(rnd);

			for (cnt = 0 ; cnt < repetitions ; cnt++)
			{
				r_array[cnt] = rand();
			}

			memcpy(v_array, r_array, repetitions * sizeof(int));
			quadsort(v_array, repetitions, sizeof(int), cmp_int);

			sprintf(dist, "random %d", repetitions);

			for (cnt = 0 ; cnt < sizeof(sorts) / sizeof(char *) ; cnt++)
			{
				test_sort(a_array, r_array, v_array, repetitions, repetitions, samples, 1, qsort,           sorts[cnt],             dist, sizeof(int), cmp_int);
			}
		}
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
