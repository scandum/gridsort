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
	gridsort 1.1.1.1
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef QUADSORT_H
  #include "quadsort.h"
#endif

#ifndef QUADSORT_CPY_H
  #include "quadsort_cpy.h"
#endif

//#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

#define BSC_X 16

#define BSC_Z 1024

//typedef int CMPFUNC (const void *a, const void *b);

///////////////////////////////////////////////////////
// ┌────────────────────────────────────────────────┐//
// │   ██████┐ ██████┐    ██████┐ ██████┐████████┐  │//
// │   └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
// │    █████┌┘ █████┌┘   ██████┌┘  ██│     ██│     │//
// │    └───██┐██┌───┘    ██┌──██┐  ██│     ██│     │//
// │   ██████┌┘███████┐   ██████┌┘██████┐   ██│     │//
// │   └─────┘ └──────┘   └─────┘ └─────┘   └─┘     │//
// └────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////

struct x_node32
{
	size_t y_size;
	size_t y;
	int *y_base;
	struct y_node32 **y_axis;
};

struct y_node32
{
	unsigned short z_size;
	unsigned short z_sort;
	int z_axis[BSC_Z];
};

struct y_node32 *split_y_node32(struct x_node32 *x_node, unsigned short y, CMPFUNC *cmp);

struct x_node32 *create_grid32(int *array, size_t nmemb)
{
	struct x_node32 *x_node = (struct x_node32 *) calloc(1, sizeof(struct x_node32));

	x_node->y_base = malloc(BSC_X * sizeof(int));
	x_node->y_axis = malloc(BSC_X * sizeof(struct y_node32 *));

	x_node->y_axis[0] = (struct y_node32 *) malloc(sizeof(struct y_node32));

	x_node->y_size = x_node->y_axis[0]->z_size = x_node->y_axis[0]->z_sort = 1;

	x_node->y_axis[0]->z_axis[0] = x_node->y_base[0] = array[0];

	x_node->y = 0;

	return x_node;
}

void bulksort32_cpy(int *dest, int *array, unsigned short nmemb, unsigned short sort, CMPFUNC *cmp)
{
	int swap32[BSC_Z / 2];
	unsigned short diff;

	if (sort >= nmemb / 2)
	{
		diff = nmemb - sort;
	}
	else
	{
		diff = nmemb;
		sort = 0;
	}

	if (diff > 128)
	{
		if (quad_swap32(array + sort, diff, cmp) == 0)
		{
			quad_merge32(array + sort, swap32, diff, 16, cmp);
		}
	}
	else if (diff > 16)
	{
		if (quad_swap32(array + sort, diff, cmp) == 0)
		{
			tail_merge32(array + sort, swap32, diff, 16, cmp);
		}
	}
	else
	{
		tail_swap32(array + sort, diff, cmp);
	}

	if (dest)
	{
		if (sort)
		{
			tail_merge32_cpy(dest, array, nmemb, sort, cmp);
		}
		else
		{
			memcpy(dest, array, nmemb * sizeof(int));
		}
	}
	else
	{
		if (sort)
		{
			tail_merge32(array, swap32, nmemb, sort, cmp);
		}
	}
}

size_t adaptive_binary_search32(struct x_node32 *x_node, int *array, size_t array_size, int key, CMPFUNC *cmp)
{
	size_t bot, top, mid;

	bot = x_node->y;

	if (bot == x_node->y_size - 1)
	{
		if (cmp(&array[bot], &key) <= 0)
		{
			return bot;
		}
		top = bot;
	}
	else
	{
		if (bot == 0 && cmp(&array[0], &key) > 0)
		{
			return 0;
		}
		top = array_size;
	}
	bot = 0;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(&array[bot + mid], &key) <= 0)
		{
			bot += mid;
		}
		top -= mid;
	}
	return x_node->y = bot;
}

void destroy_grid32(struct x_node32 *x_node, int *array, CMPFUNC *cmp)
{
	size_t z_len = 0;

	if (x_node->y_size)
	{
		struct y_node32 *y_node;
		size_t y;

		for (y = 0 ; y < x_node->y_size ; y++)
		{
			y_node = x_node->y_axis[y];

			if (y_node->z_sort != y_node->z_size)
			{
				bulksort32_cpy(&array[z_len], y_node->z_axis, y_node->z_size, y_node->z_sort, cmp);
			}
			else
			{
				memcpy(&array[z_len], y_node->z_axis, y_node->z_size * sizeof(int));
			}
			z_len += y_node->z_size;

			free(y_node);
		}
	}
	free(x_node->y_axis);
	free(x_node->y_base);

	free(x_node);
}

void insert_z_node32(struct x_node32 *x_node, int key, CMPFUNC *cmp)
{
	struct y_node32 *y_node;
	unsigned short y;

	y = adaptive_binary_search32(x_node, x_node->y_base, x_node->y_size, key, cmp);

	y_node = x_node->y_axis[y];

	y_node->z_axis[y_node->z_size++] = key;

	if (y_node->z_size == BSC_Z)
	{
		split_y_node32(x_node, y, cmp);
	}
}

void insert_y_node32(struct x_node32 *x_node, unsigned short y)
{
	unsigned short end = ++x_node->y_size;

	if (x_node->y_size % BSC_X == 0)
	{
		x_node->y_base = realloc(x_node->y_base, (x_node->y_size + BSC_X) * sizeof(int));
		x_node->y_axis = realloc(x_node->y_axis, (x_node->y_size + BSC_X) * sizeof(struct y_node32 *));
	}

	while (y < --end)
	{
		x_node->y_axis[end] = x_node->y_axis[end - 1];
		x_node->y_base[end] = x_node->y_base[end - 1];
	}
	x_node->y_axis[y] = (struct y_node32 *) malloc(sizeof(struct y_node32));
}

struct y_node32 *split_y_node32(struct x_node32 *x_node, unsigned short y, CMPFUNC *cmp)
{
	struct y_node32 *y_node1, *y_node2;

	insert_y_node32(x_node, y + 1);

	y_node1 = x_node->y_axis[y];
	y_node2 = x_node->y_axis[y + 1];

	bulksort32_cpy(NULL, y_node1->z_axis, y_node1->z_size, y_node1->z_sort, cmp);

	y_node2->z_sort = y_node2->z_size = (y == 0) ? BSC_Z - 1 : BSC_Z / 2;

	y_node1->z_sort = y_node1->z_size = BSC_Z - y_node2->z_size;

	x_node->y_base[y + 0] = y_node1->z_axis[0];
	x_node->y_base[y + 1] = y_node1->z_axis[y_node1->z_size];

	memcpy(y_node2->z_axis, y_node1->z_axis + y_node1->z_size, y_node2->z_size * sizeof(int));

	return x_node->y_axis[y];
}


///////////////////////////////////////////////////////
// ┌────────────────────────────────────────────────┐//
// │    █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐  │//
// │   ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘  │//
// │   ██████┐ ███████│   ██████┌┘  ██│     ██│     │//
// │   ██┌──██┐└────██│   ██┌──██┐  ██│     ██│     │//
// │   └█████┌┘     ██│   ██████┌┘██████┐   ██│     │//
// │    └────┘      └─┘   └─────┘ └─────┘   └─┘     │//
// └────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////

struct x_node64
{
	size_t y_size;
	size_t y;
	long long *y_base;
	struct y_node64 **y_axis;
};

struct y_node64
{
	unsigned short z_size;
	unsigned short z_sort;
	long long z_axis[BSC_Z];
};

struct y_node64 *split_y_node64(struct x_node64 *x_node, unsigned short y, CMPFUNC *cmp);

struct x_node64 *create_grid64(long long *array, size_t nmemb)
{
	struct x_node64 *x_node = (struct x_node64 *) calloc(1, sizeof(struct x_node64));

	x_node->y_base = malloc(BSC_X * sizeof(long long));
	x_node->y_axis = malloc(BSC_X * sizeof(struct y_node64 *));

	x_node->y_axis[0] = (struct y_node64 *) malloc(sizeof(struct y_node64));

	x_node->y_size = x_node->y_axis[0]->z_size = x_node->y_axis[0]->z_sort = 1;

	x_node->y_axis[0]->z_axis[0] = x_node->y_base[0] = array[0];

	x_node->y = 0;

	return x_node;
}

void bulksort64_cpy(long long *dest, long long *array, unsigned short nmemb, unsigned short sort, CMPFUNC *cmp)
{
	long long swap64[BSC_Z / 2];
	unsigned short diff;

	if (sort >= nmemb / 2)
	{
		diff = nmemb - sort;
	}
	else
	{
		diff = nmemb;
		sort = 0;
	}

	if (diff > 128)
	{
		if (quad_swap64(array + sort, diff, cmp) == 0)
		{
			quad_merge64(array + sort, swap64, diff, 16, cmp);
		}
	}
	else if (diff > 16)
	{
		if (quad_swap64(array + sort, diff, cmp) == 0)
		{
			tail_merge64(array + sort, swap64, diff, 16, cmp);
		}
	}
	else
	{
		tail_swap64(array + sort, diff, cmp);
	}

	if (dest)
	{
		if (sort)
		{
			tail_merge64_cpy(dest, array, nmemb, sort, cmp);
		}
		else
		{
			memcpy(dest, array, nmemb * sizeof(long long));
		}
	}
	else
	{
		if (sort)
		{
			tail_merge64(array, swap64, nmemb, sort, cmp);
		}
	}
}

size_t adaptive_binary_search64(struct x_node64 *x_node, long long *array, size_t array_size, long long key, CMPFUNC *cmp)
{
	size_t bot, top, mid;

	bot = x_node->y;

	if (bot == x_node->y_size - 1)
	{
		if (cmp(&array[bot], &key) <= 0)
		{
			return bot;
		}
		top = bot;
	}
	else
	{
		if (bot == 0 && cmp(&array[0], &key) > 0)
		{
			return 0;
		}
		top = array_size;
	}
	bot = 0;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(&array[bot + mid], &key) <= 0)
		{
			bot += mid;
		}
		top -= mid;
	}
	return x_node->y = bot;
}

void destroy_grid64(struct x_node64 *x_node, long long *array, CMPFUNC *cmp)
{
	size_t z_len = 0;

	if (x_node->y_size)
	{
		struct y_node64 *y_node;
		size_t y;

		for (y = 0 ; y < x_node->y_size ; y++)
		{
			y_node = x_node->y_axis[y];

			if (y_node->z_sort != y_node->z_size)
			{
				bulksort64_cpy(&array[z_len], y_node->z_axis, y_node->z_size, y_node->z_sort, cmp);
			}
			else
			{
				memcpy(&array[z_len], y_node->z_axis, y_node->z_size * sizeof(long long));
			}
			z_len += y_node->z_size;

			free(y_node);
		}
	}
	free(x_node->y_axis);
	free(x_node->y_base);

	free(x_node);
}

void insert_z_node64(struct x_node64 *x_node, long long key, CMPFUNC *cmp)
{
	struct y_node64 *y_node;
	unsigned short y;

	y = adaptive_binary_search64(x_node, x_node->y_base, x_node->y_size, key, cmp);

	y_node = x_node->y_axis[y];

	y_node->z_axis[y_node->z_size++] = key;

	if (y_node->z_size == BSC_Z)
	{
		split_y_node64(x_node, y, cmp);
	}
}

void insert_y_node64(struct x_node64 *x_node, unsigned short y)
{
	unsigned short end = ++x_node->y_size;

	if (x_node->y_size % BSC_X == 0)
	{
		x_node->y_base = realloc(x_node->y_base, (x_node->y_size + BSC_X) * sizeof(long long));
		x_node->y_axis = realloc(x_node->y_axis, (x_node->y_size + BSC_X) * sizeof(struct y_node64 *));
	}

	while (y < --end)
	{
		x_node->y_axis[end] = x_node->y_axis[end - 1];
		x_node->y_base[end] = x_node->y_base[end - 1];
	}
	x_node->y_axis[y] = (struct y_node64 *) malloc(sizeof(struct y_node64));
}

struct y_node64 *split_y_node64(struct x_node64 *x_node, unsigned short y, CMPFUNC *cmp)
{
	struct y_node64 *y_node1, *y_node2;

	insert_y_node64(x_node, y + 1);

	y_node1 = x_node->y_axis[y];
	y_node2 = x_node->y_axis[y + 1];

	bulksort64_cpy(NULL, y_node1->z_axis, y_node1->z_size, y_node1->z_sort, cmp);

	y_node2->z_sort = y_node2->z_size = (y == 0) ? BSC_Z - 1 : BSC_Z / 2;

	y_node1->z_sort = y_node1->z_size = BSC_Z - y_node2->z_size;

	x_node->y_base[y + 0] = y_node1->z_axis[0];
	x_node->y_base[y + 1] = y_node1->z_axis[y_node1->z_size];

	memcpy(y_node2->z_axis, y_node1->z_axis + y_node1->z_size, y_node2->z_size * sizeof(long long));

	return x_node->y_axis[y];
}

/////////////////////////////////////////////////////////////////////////////
//┌───────────────────────────────────────────────────────────────────────┐//
//│    ██████┐ ██████┐ ██████┐██████┐ ███████┐ ██████┐ ██████┐ ████████┐  │//
//│   ██┌────┘ ██┌──██┐└─██┌─┘██┌──██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘  │//
//│   ██│  ███┐██████┌┘  ██│  ██│  ██│███████┐██│   ██│██████┌┘   ██│     │//
//│   ██│   ██│██┌──██┐  ██│  ██│  ██│└────██│██│   ██│██┌──██┐   ██│     │//
//│   └██████┌┘██│  ██│██████┐██████┌┘███████│└██████┌┘██│  ██│   ██│     │//
//│    └─────┘ └─┘  └─┘└─────┘└─────┘ └──────┘ └─────┘ └─┘  └─┘   └─┘     │//
//└───────────────────────────────────────────────────────────────────────┘//
/////////////////////////////////////////////////////////////////////////////

void gridsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	size_t cnt = nmemb;

	if (nmemb < 2)
	{
		return;
	}

	if (size == sizeof(int))
	{
		int *pta = (int *) array;

		struct x_node32 *grid = create_grid32(pta++, cnt--);

		while (cnt--)
		{
			insert_z_node32(grid, *pta++, cmp);
		}

		destroy_grid32(grid, (int *) array, cmp);
	}
	else if (size == sizeof(long long))
	{
		long long *pta = (long long *) array;

		struct x_node64 *grid = create_grid64(pta++, cnt--);

		while (cnt--)
		{
			insert_z_node64(grid, *pta++, cmp);
		}

		destroy_grid64(grid, (long long *) array, cmp);
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}
