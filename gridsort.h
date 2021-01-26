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
	gridsort 1.1.1.2
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#ifndef QUADSORT_H
  #include "quadsort.h"
#endif

//#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

#define BSC_X 64
#define BSC_Y 2

size_t  BSC_Z;

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
	int *swap;
	size_t y_size;
	size_t y;
	int *y_base;
	struct y_node32 **y_axis;
};

struct y_node32
{
	size_t z_size;
	int *z_axis1;
	int *z_axis2;
};

void split_y_node32(struct x_node32 *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp);

struct x_node32 *create_grid32(int *array, size_t nmemb, CMPFUNC *cmp)
{
	struct x_node32 *x_node = (struct x_node32 *) malloc(sizeof(struct x_node32));
	struct y_node32 *y_node = (struct y_node32 *) malloc(sizeof(struct y_node32));

	for (BSC_Z = BSC_X ; BSC_Z * BSC_Z * BSC_Y < nmemb ; BSC_Z *= 2);

	x_node->swap = (int *) malloc(BSC_Z * sizeof(int));

	x_node->y_base = (int *) malloc(BSC_X * sizeof(int));
	x_node->y_axis = (struct y_node32 **) malloc(BSC_X * sizeof(struct y_node32 *));

	x_node->y_axis[0] = y_node;

	// avoid pointless binary searches by creating two y nodes right away

	if (quad_swap32(array, BSC_Z * 2, cmp) == 0)
	{
		quad_merge32(array, x_node->swap, BSC_Z * 2, 16, cmp);
	}

	y_node->z_axis1 = (int *) malloc(BSC_Z * sizeof(int));
	memcpy(y_node->z_axis1, array, BSC_Z * sizeof(int));

	y_node->z_axis2 = (int *) malloc(BSC_Z * sizeof(int));
	memcpy(y_node->z_axis2, array + BSC_Z, BSC_Z * sizeof(int));

	x_node->y_size = 1;
	x_node->y = 0;

	x_node->y_base[0] = y_node->z_axis1[0];

	y_node->z_size = BSC_Z;

	split_y_node32(x_node, 0, 1, cmp);

	return x_node;
}

// merge two sorted arrays

void twin_merge32(int *swap, struct y_node32 *y_node, CMPFUNC *cmp)
{
	register int *pta, *ptb, *tpa, *tpb, *pts, *tps;

	pta = y_node->z_axis1;
	ptb = y_node->z_axis2;
	tpa = pta + BSC_Z - 1;
	tpb = ptb + BSC_Z - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		return;
	}

	if (cmp(pta, tpb) > 0)
	{
		pta = y_node->z_axis1;
		y_node->z_axis1 = y_node->z_axis2;
		y_node->z_axis2 = pta;

		return;
	}

	while (cmp(pta, ptb) <= 0)
	{
		pta++;
	}

	tps = pta;
	pts = swap;

	while (tps <= tpa)
	{
		*pts++ = *tps++;
	}

	tps = pts - 1;
	pts = swap;

	*pta++ = *ptb++;

	while (pta <= tpa)
	{
		*pta++ = cmp(pts, ptb) > 0 ? *ptb++ : *pts++;
	}

	pta = y_node->z_axis2;

	if (cmp(tps, tpb) <= 0)
	{
		while (pts <= tps)
		{
			while (cmp(pts, ptb) > 0)
			{
				*pta++ = *ptb++;
			}
			*pta++ = *pts++;
		}
	}
	else
	{
		while (ptb <= tpb)
		{
			while (cmp(pts, ptb) <= 0)
			{
				*pta++ = *pts++;
			}
			*pta++ = *ptb++;
		}

		while (pts <= tps)
		{
			*pta++ = *pts++;
		}
	}
}


void twin_merge32_cpy(int *dest, struct y_node32 *y_node, size_t nmemb1, size_t nmemb2, CMPFUNC *cmp)
{
	register int *pta, *ptb, *tpa, *tpb;

	pta = y_node->z_axis1;
	ptb = y_node->z_axis2;
	tpa = pta + nmemb1 - 1;
	tpb = ptb + nmemb2 - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		memcpy(dest, pta, nmemb1 * sizeof(int));

		dest += nmemb1;

		memcpy(dest, ptb, nmemb2 * sizeof(int));

		return;
	}

	if (cmp(tpa, tpb) <= 0)
	{
		while (pta <= tpa)
		{
			while (cmp(pta, ptb) > 0)
			{
				*dest++ = *ptb++;
			}
			*dest++ = *pta++;
		}

		while (ptb <= tpb)
		{
			*dest++ = *ptb++;
		}
	}
	else
	{
		while (ptb <= tpb)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*dest++ = *pta++;
			}
			*dest++ = *ptb++;
		}

		while (pta <= tpa)
		{
			*dest++ = *pta++;
		}
	}
}

// y_node->z_axis1 should be sorted and of BSC_Z size.
// y_node->z_axis2 should be unsorted and of y_node->z_size size.

void bulksort32_cpy(struct x_node32 *x_node, int *dest, struct y_node32 *y_node, CMPFUNC *cmp)
{
	if (y_node->z_size > 128)
	{
		if (quad_swap32(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			quad_merge32(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else if (y_node->z_size > 16)
	{
		if (quad_swap32(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			tail_merge32(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else
	{
		tail_swap32(y_node->z_axis2, y_node->z_size, cmp);
	}

	if (dest)
	{
		twin_merge32_cpy(dest, y_node, BSC_Z, y_node->z_size, cmp);
	}
	else if (y_node->z_axis1)
	{
		twin_merge32(x_node->swap, y_node, cmp);
	}
}


size_t adaptive_binary_search32(struct x_node32 *x_node, int *array, int key, CMPFUNC *cmp)
{
	size_t top, mid;
	int *base = array;

	if (x_node->y == x_node->y_size - 1)
	{
		if (cmp(&array[x_node->y], &key) <= 0)
		{
			return x_node->y;
		}
		top = x_node->y;
	}
	else if (x_node->y == 0)
	{
		base++;

		if (cmp(base, &key) > 0)
		{
			return 0;
		}
		top = x_node->y_size - 1;
	}
	else
	{
		top = x_node->y_size;
	}

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(base + mid, &key) <= 0)
		{
			base += mid;
		}
		top -= mid;
	}
	return x_node->y = base - array;
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

			if (y_node->z_size)
			{
				bulksort32_cpy(x_node, &array[z_len], y_node, cmp);
			}
			else
			{
				memcpy(&array[z_len], y_node->z_axis1, BSC_Z * sizeof(int));
			}
			z_len += BSC_Z + y_node->z_size;

			free(y_node->z_axis1);
			free(y_node->z_axis2);

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
	size_t y;

	y = adaptive_binary_search32(x_node, x_node->y_base, key, cmp);

	y_node = x_node->y_axis[y];

	y_node->z_axis2[y_node->z_size++] = key;

	if (y_node->z_size == BSC_Z)
	{
		split_y_node32(x_node, y, y + 1, cmp);
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

	x_node->y_axis[y]->z_axis1 = (int *) malloc(BSC_Z * sizeof(int));
	x_node->y_axis[y]->z_axis2 = (int *) malloc(BSC_Z * sizeof(int));
}


void split_y_node32(struct x_node32 *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp)
{
	struct y_node32 *y_node1, *y_node2;
	int *swap;

	insert_y_node32(x_node, y2);

	y_node1 = x_node->y_axis[y1];
	y_node2 = x_node->y_axis[y2];

	if (x_node->y_size > 2)
	{
		bulksort32_cpy(x_node, NULL, y_node1, cmp);
	}

	y_node1->z_size = y_node2->z_size = 0;

	swap = y_node2->z_axis1; y_node2->z_axis1 = y_node1->z_axis2; y_node1->z_axis2 = swap;

	x_node->y_base[y1] = y_node1->z_axis1[0];
	x_node->y_base[y2] = y_node2->z_axis1[0];
}

// identical to 32 bit version except that int has been changed to long long

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
	long long *swap;
	size_t y_size;
	size_t y;
	long long *y_base;
	struct y_node64 **y_axis;
};

struct y_node64
{
	size_t z_size;
	long long *z_axis1;
	long long *z_axis2;
};

void split_y_node64(struct x_node64 *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp);

struct x_node64 *create_grid64(long long *array, size_t nmemb, CMPFUNC *cmp)
{
	struct x_node64 *x_node = (struct x_node64 *) malloc(sizeof(struct x_node64));
	struct y_node64 *y_node = (struct y_node64 *) malloc(sizeof(struct y_node64));

	for (BSC_Z = BSC_X ; BSC_Z * BSC_Z * BSC_Y < nmemb ; BSC_Z *= 2);

	x_node->swap = (long long *) malloc(BSC_Z * sizeof(long long));

	x_node->y_base = (long long *) malloc(BSC_X * sizeof(long long));
	x_node->y_axis = (struct y_node64 **) malloc(BSC_X * sizeof(struct y_node64 *));

	x_node->y_axis[0] = y_node;

	if (quad_swap64(array, BSC_Z * 2, cmp) == 0)
	{
		quad_merge64(array, x_node->swap, BSC_Z * 2, 16, cmp);
	}

	y_node->z_axis1 = (long long *) malloc(BSC_Z * sizeof(long long));
	memcpy(y_node->z_axis1, array, BSC_Z * sizeof(long long));

	y_node->z_axis2 = (long long *) malloc(BSC_Z * sizeof(long long));
	memcpy(y_node->z_axis2, array + BSC_Z, BSC_Z * sizeof(long long));

	x_node->y_size = 1;
	x_node->y = 0;

	x_node->y_base[0] = y_node->z_axis1[0];

	y_node->z_size = BSC_Z;

	split_y_node64(x_node, 0, 1, cmp);

	return x_node;
}

// merge two sorted arrays

void twin_merge64(long long *swap, struct y_node64 *y_node, CMPFUNC *cmp)
{
	register long long *pta, *ptb, *tpa, *tpb, *pts, *tps;

	pta = y_node->z_axis1;
	ptb = y_node->z_axis2;
	tpa = pta + BSC_Z - 1;
	tpb = ptb + BSC_Z - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		return;
	}

	if (cmp(pta, tpb) > 0)
	{
		pta = y_node->z_axis1;
		y_node->z_axis1 = y_node->z_axis2;
		y_node->z_axis2 = pta;

		return;
	}

	while (cmp(pta, ptb) <= 0)
	{
		pta++;
	}

	tps = pta;
	pts = swap;

	while (tps <= tpa)
	{
		*pts++ = *tps++;
	}

	tps = pts - 1;
	pts = swap;

	*pta++ = *ptb++;

	while (pta <= tpa)
	{
		*pta++ = cmp(pts, ptb) > 0 ? *ptb++ : *pts++;
	}

	pta = y_node->z_axis2;

	if (cmp(tps, tpb) <= 0)
	{
		while (pts <= tps)
		{
			while (cmp(pts, ptb) > 0)
			{
				*pta++ = *ptb++;
			}
			*pta++ = *pts++;
		}
	}
	else
	{
		while (ptb <= tpb)
		{
			while (cmp(pts, ptb) <= 0)
			{
				*pta++ = *pts++;
			}
			*pta++ = *ptb++;
		}

		while (pts <= tps)
		{
			*pta++ = *pts++;
		}
	}
}


void twin_merge64_cpy(long long *dest, struct y_node64 *y_node, size_t nmemb1, size_t nmemb2, CMPFUNC *cmp)
{
	register long long *pta, *ptb, *tpa, *tpb;

	pta = y_node->z_axis1;
	ptb = y_node->z_axis2;
	tpa = pta + nmemb1 - 1;
	tpb = ptb + nmemb2 - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		memcpy(dest, pta, nmemb1 * sizeof(long long));

		dest += nmemb1;

		memcpy(dest, ptb, nmemb2 * sizeof(long long));

		return;
	}

	if (cmp(tpa, tpb) <= 0)
	{
		while (pta <= tpa)
		{
			while (cmp(pta, ptb) > 0)
			{
				*dest++ = *ptb++;
			}
			*dest++ = *pta++;
		}

		while (ptb <= tpb)
		{
			*dest++ = *ptb++;
		}
	}
	else
	{
		while (ptb <= tpb)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*dest++ = *pta++;
			}
			*dest++ = *ptb++;
		}

		while (pta <= tpa)
		{
			*dest++ = *pta++;
		}
	}
}

// y_node->z_axis1 should be sorted and of BSC_Z size.
// y_node->z_axis2 should be unsorted and of y_node->z_size size.

void bulksort64_cpy(struct x_node64 *x_node, long long *dest, struct y_node64 *y_node, CMPFUNC *cmp)
{
	if (y_node->z_size > 128)
	{
		if (quad_swap64(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			quad_merge64(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else if (y_node->z_size > 16)
	{
		if (quad_swap64(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			tail_merge64(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else
	{
		tail_swap64(y_node->z_axis2, y_node->z_size, cmp);
	}

	if (dest)
	{
		twin_merge64_cpy(dest, y_node, BSC_Z, y_node->z_size, cmp);
	}
	else if (y_node->z_axis1)
	{
		twin_merge64(x_node->swap, y_node, cmp);
	}
}


size_t adaptive_binary_search64(struct x_node64 *x_node, long long *array, long long key, CMPFUNC *cmp)
{
	size_t top, mid;
	long long *base = array;

	if (x_node->y == x_node->y_size - 1)
	{
		if (cmp(&array[x_node->y], &key) <= 0)
		{
			return x_node->y;
		}
		top = x_node->y;
	}
	else if (x_node->y == 0)
	{
		base++;

		if (cmp(base, &key) > 0)
		{
			return 0;
		}
		top = x_node->y_size - 1;
	}
	else
	{
		top = x_node->y_size;
	}

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(base + mid, &key) <= 0)
		{
			base += mid;
		}
		top -= mid;
	}

	return x_node->y = base - array;
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

			if (y_node->z_size)
			{
				bulksort64_cpy(x_node, &array[z_len], y_node, cmp);
			}
			else
			{
				memcpy(&array[z_len], y_node->z_axis1, BSC_Z * sizeof(long long));
			}
			z_len += BSC_Z + y_node->z_size;

			free(y_node->z_axis1);
			free(y_node->z_axis2);

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
	size_t y;

	y = adaptive_binary_search64(x_node, x_node->y_base, key, cmp);

	y_node = x_node->y_axis[y];

	y_node->z_axis2[y_node->z_size++] = key;

	if (y_node->z_size == BSC_Z)
	{
		split_y_node64(x_node, y, y + 1, cmp);
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

	x_node->y_axis[y]->z_axis1 = (long long *) malloc(BSC_Z * sizeof(long long));
	x_node->y_axis[y]->z_axis2 = (long long *) malloc(BSC_Z * sizeof(long long));
}


void split_y_node64(struct x_node64 *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp)
{
	struct y_node64 *y_node1, *y_node2;
	long long *swap;

	insert_y_node64(x_node, y2);

	y_node1 = x_node->y_axis[y1];
	y_node2 = x_node->y_axis[y2];

	if (x_node->y_size > 2)
	{
		bulksort64_cpy(x_node, NULL, y_node1, cmp);
	}

	y_node1->z_size = y_node2->z_size = 0;

	swap = y_node2->z_axis1; y_node2->z_axis1 = y_node1->z_axis2; y_node1->z_axis2 = swap;

	x_node->y_base[y1] = y_node1->z_axis1[0];
	x_node->y_base[y2] = y_node2->z_axis1[0];
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

	if (nmemb < BSC_X * BSC_X)
	{
		return quadsort(array, nmemb, size, cmp);
	}

	if (size == sizeof(int))
	{
		int *pta = (int *) array;

		struct x_node32 *grid = create_grid32(pta, cnt, cmp);

		pta += BSC_Z * 2;
		cnt -= BSC_Z * 2;

		while (cnt--)
		{
			insert_z_node32(grid, *pta++, cmp);
		}

		destroy_grid32(grid, (int *) array, cmp);
	}
	else if (size == sizeof(long long))
	{
		long long *pta = (long long *) array;

		struct x_node64 *grid = create_grid64(pta, cnt, cmp);

		pta += BSC_Z * 2;
		cnt -= BSC_Z * 2;

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
