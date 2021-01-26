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
	quadsort 1.1.3.3
*/

#ifndef QUADSORT_H
#define QUADSORT_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int CMPFUNC (const void *a, const void *b);

//#define cmp(a,b) (*(a) > *(b))

#define swap_two(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		swap = array[1]; array[1] = array[0]; array[0] = swap;  \
	}  \
}

#define swap_three(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		if (cmp(array, array + 2) <= 0)  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = swap;  \
		}  \
		else if (cmp(array + 1, array + 2) > 0)  \
		{  \
			swap = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = array[2]; array[2] = swap;  \
		}  \
	}  \
	else if (cmp(array + 1, array + 2) > 0)  \
	{  \
		if (cmp(array, array + 2) > 0)  \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;  \
		}  \
		else   \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = swap;  \
		}  \
	}  \
}  \

#define swap_four(array, swap)  \
{  \
	if (cmp(array, array + 1) > 0)  \
	{  \
		if (cmp(array, array + 2) <= 0)  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = swap;  \
		}  \
		else if (cmp(array + 1, array + 2) > 0)  \
		{  \
			swap = array[0]; array[0] = array[2]; array[2] = swap;  \
		}  \
		else  \
		{  \
			swap = array[0]; array[0] = array[1]; array[1] = array[2]; array[2] = swap;  \
		}  \
	}  \
	else if (cmp(array + 1, array + 2) > 0)  \
	{  \
		if (cmp(array, array + 2) > 0)  \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;  \
		}  \
		else   \
		{  \
			swap = array[2]; array[2] = array[1]; array[1] = swap;  \
		}  \
	}  \
	if (cmp(array + 1, array + 3) > 0)  \
	{  \
		if (cmp(array, array + 3) > 0)  \
		{  \
			swap = array[3]; array[3] = array[2]; array[2] = array[1]; array[1] = array[0]; array[0] = swap;  \
		}  \
		else  \
		{  \
			swap = array[3]; array[3] = array[2]; array[2] = array[1]; array[1] = swap;  \
		}  \
	}  \
	else if (cmp(array + 2, array + 3) > 0)  \
	{  \
		swap = array[3]; array[3] = array[2]; array[2] = swap;  \
	}  \
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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR int
#define FUNC(NAME) NAME##32

#include "quadsort.c"

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

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long long
#define FUNC(NAME) NAME##64

#include "quadsort.c"

///////////////////////////////////////////////////////////////////////////////
//┌─────────────────────────────────────────────────────────────────────────┐//
//│    ██████┐ ██┐   ██┐ █████┐ ██████┐ ███████┐ ██████┐ ██████┐ ████████┐  │//
//│   ██┌───██┐██│   ██│██┌──██┐██┌──██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘  │//
//│   ██│   ██│██│   ██│███████│██│  ██│███████┐██│   ██│██████┌┘   ██│     │//
//│   ██│▄▄ ██│██│   ██│██┌──██│██│  ██│└────██│██│   ██│██┌──██┐   ██│     │//
//│   └██████┌┘└██████┌┘██│  ██│██████┌┘███████│└██████┌┘██│  ██│   ██│     │//
//│    └──▀▀─┘  └─────┘ └─┘  └─┘└─────┘ └──────┘ └─────┘ └─┘  └─┘   └─┘     │//
//└─────────────────────────────────────────────────────────────────────────┘//
///////////////////////////////////////////////////////////////////////////////

void quadsort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}

	if (size == sizeof(int))
	{
		if (nmemb <= 16)
		{
			tail_swap32(array, (unsigned char) nmemb, cmp);
		}
		else if (nmemb < 128)
		{
			if (quad_swap32(array, nmemb, cmp) == 0)
			{
				int swap[64];

				tail_merge32(array, swap, nmemb, 16, cmp);
			}
		}
		else
		{
			if (quad_swap32(array, nmemb, cmp) == 0)
			{
				int *swap = malloc(nmemb * size / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "quadsort(%p,%zu,%zu,%p): malloc() failed: %s\n", array, nmemb, size, cmp, strerror(errno));

					return;
				}

				quad_merge32(array, swap, nmemb, 16, cmp);

				free(swap);
			}
		}
	}
	else if (size == sizeof(long long))
	{
		if (nmemb <= 16)
		{
			tail_swap64(array, (unsigned char) nmemb, cmp);
		}
		else if (nmemb < 128)
		{
			if (quad_swap64(array, nmemb, cmp) == 0)
			{
				long long swap[64];

				tail_merge64(array, swap, nmemb, 16, cmp);
			}
		}
		else
		{
			if (quad_swap64(array, nmemb, cmp) == 0)
			{
				long long *swap = malloc(nmemb * size / 2);

				if (swap == NULL)
				{
					fprintf(stderr, "quadsort(%p,%zu,%zu,%p): malloc() failed: %s\n", array, nmemb, size, cmp, strerror(errno));

					return;
				}
				quad_merge64(array, swap, nmemb, 16, cmp);

				free(swap);
			}
		}
	}
	else
	{
		assert(size == sizeof(int) || size == sizeof(long long));
	}
}

#endif
