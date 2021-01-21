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
	quadsort 1.1.3.1
*/

#ifndef QUADSORT_CPY_H
#define QUADSORT_CPY_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))

#define tail_swap_two_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		dest[0] = array[1]; dest[1] = array[0]; \
	} \
	else \
	{ \
		dest[0] = array[0]; dest[1] = array[1]; \
	} \
}

#define tail_swap_three_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		if (cmp(array, array + 2) <= 0) \
		{ \
			dest[0] = array[1]; dest[1] = array[0]; dest[2] = array[2]; \
		} \
		else if (cmp(array + 1, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[1]; dest[2] = array[0]; \
		} \
		else \
		{ \
			dest[0] = array[1]; dest[1] = array[2]; dest[2] = array[0]; \
		} \
	} \
	else if (cmp(array + 1, array + 2) > 0) \
	{ \
		if (cmp(array, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[0]; dest[2] = array[1]; \
		} \
		else  \
		{ \
			dest[0] = array[0]; dest[1] = array[2]; dest[2] = array[1]; \
		} \
	} \
	else \
	{ \
			dest[0] = array[0]; dest[1] = array[1]; dest[2] = array[2]; \
	} \
}

#define tail_swap_four_cpy(dest, array, swap) \
{ \
	if (cmp(array, array + 1) > 0) \
	{ \
		if (cmp(array, array + 2) <= 0) \
		{ \
			dest[0] = array[1]; dest[1] = array[0]; dest[2] = array[2]; \
		} \
		else if (cmp(array + 1, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[1]; dest[2] = array[0]; \
		} \
		else \
		{ \
			dest[0] = array[1]; dest[1] = array[2]; dest[2] = array[0]; \
		} \
	} \
	else if (cmp(array + 1, array + 2) > 0) \
	{ \
		if (cmp(array, array + 2) > 0) \
		{ \
			dest[0] = array[2]; dest[1] = array[0]; dest[2] = array[1]; \
		} \
		else  \
		{ \
			dest[0] = array[0]; dest[1] = array[2]; dest[2] = array[1]; \
		} \
	} \
	else \
	{ \
			dest[0] = array[0]; dest[1] = array[1]; dest[2] = array[2]; \
	} \
 \
	if (cmp(dest + 1, array + 3) > 0) \
	{ \
		if (cmp(dest, array + 3) > 0) \
		{ \
			dest[3] = dest[2]; dest[2] = dest[1]; dest[1] = dest[0]; dest[0] = array[3]; \
		} \
		else \
		{ \
			dest[3] = dest[2]; dest[2] = dest[1]; dest[1] = array[3]; \
		} \
	} \
	else if (cmp(dest + 2, array + 3) > 0) \
	{ \
		dest[3] = dest[2]; dest[2] = array[3]; \
	} \
	else \
	{ \
		dest[3] = array[3]; \
	} \
}

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

void tail_insert32_cpy(int *array, int key, unsigned char nmemb, CMPFUNC *cmp)
{
	int *pta = array + nmemb - 1;

	if (cmp(pta, &key) > 0)
	{
		if (nmemb > 4 && cmp(pta - 4, &key) > 0)
		{
			array[nmemb--] = *pta--;
			array[nmemb--] = *pta--;
			array[nmemb--] = *pta--;
			array[nmemb--] = *pta--;
		}
	
		if (nmemb > 2 && cmp(pta - 2, &key) > 0)
		{
			array[nmemb--] = *pta--;
			array[nmemb--] = *pta--;
		}
	
		if (nmemb > 1 && cmp(pta - 1, &key) > 0)
		{
			array[nmemb--] = *pta--;
		}
	
		if (nmemb > 0 && cmp(pta, &key) > 0)
		{
			array[nmemb--] = *pta--;
		}

	}
	array[nmemb] = key;
}


void tail_swap32_cpy(int *dest, int *array, unsigned char nmemb, CMPFUNC *cmp)
{
	int pts[8];
	register const int *pta = array;
	register int *ptd = dest;
	register unsigned char i, mid, cnt;

	switch (nmemb)
	{
		case 0:
			return;
		case 1:
			*dest = *array;
			return;
		case 2:
			tail_swap_two_cpy(ptd, pta, pts);
			return;
		case 3:
			tail_swap_three_cpy(ptd, pta, pts);
			return;
		case 4:
			tail_swap_four_cpy(ptd, pta, pts);
			return;
	}

	tail_swap_four_cpy(ptd, pta, pts);

	if (nmemb < 9)
	{
		for (cnt = 4 ; cnt < nmemb ; cnt++)
		{
			tail_insert32_cpy(dest, pta[cnt], cnt, cmp);
		}
		return;
	}

	pta += 4;
	ptd += 4;

	tail_swap_four_cpy(ptd, pta, pts);

	pta += 4;
	ptd += 4;

	switch (nmemb)
	{
		case 9:
			ptd[0] = pta[0];
			break;

		case 10:
			tail_swap_two_cpy(ptd, pta, pts);
			break;

		case 11:
			tail_swap_three_cpy(ptd, pta, pts);
			break;

		default:
			tail_swap_four_cpy(ptd, pta, pts);

			for (cnt = 4 ; cnt + 8 < nmemb ; cnt++)
			{
				tail_insert32_cpy(ptd, pta[cnt], cnt, cmp);
			}
			break;
	}
	ptd -= 8;

	// step 3

	if (cmp(&ptd[3], &ptd[4]) <= 0)
	{
		if (cmp(&ptd[7], &ptd[8]) <= 0)
		{
			return;
		}
		for (i = 0 ; i < 8 ; i++)
		{
			pts[i] = ptd[i];
		}
	}
	else if (cmp(&ptd[0], &ptd[7]) > 0)
	{
		pts[0] = ptd[4];
		pts[1] = ptd[5];
		pts[2] = ptd[6];
		pts[3] = ptd[7];

		pts[4] = ptd[0];
		pts[5] = ptd[1];
		pts[6] = ptd[2];
		pts[7] = ptd[3];
	}
	else
	{
		cnt = 0;
		i = 0;
		mid = 4;

		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];

		while (i < 4 && mid < 8)
		{
			if (cmp(&ptd[i], &ptd[mid]) > 0)
			{
				pts[cnt++] = ptd[mid++];
			}
			else
			{
				pts[cnt++] = ptd[i++];
			}
		}
		while (i < 4)
		{
			pts[cnt++] = ptd[i++];
		}
		while (mid < 8)
		{
			pts[cnt++] = ptd[mid++];
		}
	}

	cnt = 0;
	i = 0;
	mid = 8;

	ptd[cnt++] = cmp(&pts[i], &ptd[mid]) > 0 ? ptd[mid++] : pts[i++];

	while (i < 8 && mid < nmemb)
	{
		if (cmp(&pts[i], &ptd[mid]) > 0)
		{
			ptd[cnt++] = ptd[mid++];
		}
		else
		{
			ptd[cnt++] = pts[i++];
		}
	}
	while (i < 8)
	{
		ptd[cnt++] = pts[i++];
	}
}

void tail_merge32_cpy(int *dest, int *array, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	register int *pta, *ptb, *ptd, *ptm;

	ptd = dest;

	pta = array;
	ptb = array + block;
	ptm = array + nmemb;

	if (cmp(ptb - 1, ptb) <= 0)
	{
		while (pta + 8 < ptm)
		{
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
		}
		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
		return;
	}

	if (cmp(ptb - 1, ptm - 1) <= 0)
	{
		ptm = array + block;

		while (pta < ptm)
		{
			while (cmp(pta, ptb) > 0)
			{
				*ptd++ = *ptb++;
			}
			*ptd++ = *pta++;
		}
		ptm = array + nmemb;

		while (ptb < ptm)
		{
			*ptd++ = *ptb++;
		}
	}
	else
	{
		while (ptb < ptm)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*ptd++ = *pta++;
			}
			*ptd++ = *ptb++;
		}
		ptm = array + block;

		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
	}
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


void tail_insert64_cpy(long long *array, long long key, unsigned char nmemb, CMPFUNC *cmp)
{
	long long *pta = array + nmemb - 1;

	if (nmemb > 4 && cmp(pta - 4, &key) > 0)
	{
		array[nmemb--] = *pta--;
		array[nmemb--] = *pta--;
		array[nmemb--] = *pta--;
		array[nmemb--] = *pta--;
	}

	if (nmemb > 2 && cmp(pta - 2, &key) > 0)
	{
		array[nmemb--] = *pta--;
		array[nmemb--] = *pta--;
	}

	if (nmemb > 1 && cmp(pta - 1, &key) > 0)
	{
		array[nmemb--] = *pta--;
	}

	if (nmemb > 0 && cmp(pta, &key) > 0)
	{
		array[nmemb--] = *pta--;
	}
	array[nmemb] = key;
}

void tail_swap64_cpy(long long *dest, long long *array, unsigned char nmemb, CMPFUNC *cmp)
{
	long long pts[8];
	register const long long *pta = array;
	register long long *ptd = dest;
	register unsigned char i, mid, cnt;

	switch (nmemb)
	{
		case 0:
			return;
		case 1:
			*dest = *array;
			return;
		case 2:
			tail_swap_two_cpy(ptd, pta, pts);
			return;
		case 3:
			tail_swap_three_cpy(ptd, pta, pts);
			return;
		case 4:
			tail_swap_four_cpy(ptd, pta, pts);
			return;
	}

	tail_swap_four_cpy(ptd, pta, pts);

	if (nmemb < 9)
	{
		for (cnt = 4 ; cnt < nmemb ; cnt++)
		{
			tail_insert64_cpy(dest, pta[cnt], cnt, cmp);
		}
		return;
	}

	pta += 4;
	ptd += 4;

	tail_swap_four_cpy(ptd, pta, pts);

	pta += 4;
	ptd += 4;

	switch (nmemb)
	{
		case 9:
			ptd[0] = pta[0];
			break;

		case 10:
			tail_swap_two_cpy(ptd, pta, pts);
			break;

		case 11:
			tail_swap_three_cpy(ptd, pta, pts);
			break;

		default:
			tail_swap_four_cpy(ptd, pta, pts);

			for (cnt = 4 ; cnt + 8 < nmemb ; cnt++)
			{
				tail_insert64_cpy(ptd, pta[cnt], cnt, cmp);
			}
			break;
	}
	ptd -= 8;

	// step 3

	if (cmp(&ptd[3], &ptd[4]) <= 0)
	{
		if (cmp(&ptd[7], &ptd[8]) <= 0)
		{
			return;
		}
		for (i = 0 ; i < 8 ; i++)
		{
			pts[i] = ptd[i];
		}
	}
	else if (cmp(&ptd[0], &ptd[7]) > 0)
	{
		pts[0] = ptd[4];
		pts[1] = ptd[5];
		pts[2] = ptd[6];
		pts[3] = ptd[7];

		pts[4] = ptd[0];
		pts[5] = ptd[1];
		pts[6] = ptd[2];
		pts[7] = ptd[3];
	}
	else
	{
		cnt = 0;
		i = 0;
		mid = 4;

		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];
		pts[cnt++] = cmp(&ptd[i], &ptd[mid]) > 0 ? ptd[mid++] : ptd[i++];

		while (i < 4 && mid < 8)
		{
			if (cmp(&ptd[i], &ptd[mid]) > 0)
			{
				pts[cnt++] = ptd[mid++];
			}
			else
			{
				pts[cnt++] = ptd[i++];
			}
		}
		while (i < 4)
		{
			pts[cnt++] = ptd[i++];
		}
		while (mid < 8)
		{
			pts[cnt++] = ptd[mid++];
		}
	}

	cnt = 0;
	i = 0;
	mid = 8;

	ptd[cnt++] = cmp(&pts[i], &ptd[mid]) > 0 ? ptd[mid++] : pts[i++];

	while (i < 8 && mid < nmemb)
	{
		if (cmp(&pts[i], &ptd[mid]) > 0)
		{
			ptd[cnt++] = ptd[mid++];
		}
		else
		{
			ptd[cnt++] = pts[i++];
		}
	}
	while (i < 8)
	{
		ptd[cnt++] = pts[i++];
	}
}


void tail_merge64_cpy(long long *dest, long long *array, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	register long long *pta, *ptb, *ptd, *ptm;

	ptd = dest;

	pta = array;
	ptb = array + block;
	ptm = array + nmemb;

	if (cmp(ptb - 1, ptb) <= 0)
	{
		while (pta + 8 < ptm)
		{
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
			*ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++; *ptd++ = *pta++;
		}
		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
		return;
	}

	if (cmp(ptb - 1, ptm - 1) <= 0)
	{
		ptm = array + block;

		while (pta < ptm)
		{
			while (cmp(pta, ptb) > 0)
			{
				*ptd++ = *ptb++;
			}
			*ptd++ = *pta++;
		}
		ptm = array + nmemb;

		while (ptb < ptm)
		{
			*ptd++ = *ptb++;
		}
	}
	else
	{
		while (ptb < ptm)
		{
			while (cmp(pta, ptb) <= 0)
			{
				*ptd++ = *pta++;
			}
			*ptd++ = *ptb++;
		}
		ptm = array + block;

		while (pta < ptm)
		{
			*ptd++ = *pta++;
		}
	}
}

#endif
