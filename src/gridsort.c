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

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	gridsort 1.1.4.2
*/

STRUCT(x_node)
{
	VAR *swap;
	size_t y_size;
	size_t y;
	VAR *y_base;
	STRUCT(y_node) **y_axis;
};

STRUCT(y_node)
{
	size_t z_size;
	VAR *z_axis1;
	VAR *z_axis2;
};

STRUCT(x_node) *FUNC(create_grid)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	STRUCT(x_node) *x_node = (STRUCT(x_node) *) malloc(sizeof(STRUCT(x_node)));
	STRUCT(y_node) *y_node;

	for (BSC_Z = BSC_X ; BSC_Z * BSC_Z * BSC_Y < nmemb ; BSC_Z *= 4);
/*
	x_node->swap = (VAR *) malloc(2 * nmemb * sizeof(VAR));

	if (x_node->swap == NULL)
	{
		printf("malloc error");
	}
	else
	{
		free(x_node->swap);
	}
*/
	x_node->swap = (VAR *) malloc(BSC_Z * sizeof(VAR));

	x_node->y_base = (VAR *) malloc(BSC_Z * sizeof(VAR));

	x_node->y_axis = (STRUCT(y_node) **) malloc(BSC_Z * sizeof(STRUCT(y_node) *));

	FUNC(quadsort_swap)(array, x_node->swap, BSC_Z * 2, cmp);

	for (int cnt = 0 ; cnt < 2 ; cnt++)
	{
		y_node = (STRUCT(y_node) *) malloc(sizeof(STRUCT(y_node)));

		y_node->z_axis1 = (VAR *) malloc(BSC_Z * sizeof(VAR));
		memcpy(y_node->z_axis1, array + cnt * BSC_Z, BSC_Z * sizeof(VAR));

		y_node->z_axis2 = (VAR *) malloc(BSC_Z * sizeof(VAR));

		y_node->z_size = 0;

		x_node->y_axis[cnt] = y_node;
		x_node->y_base[cnt] = y_node->z_axis1[0];
	}
	x_node->y_size = 2;
	x_node->y = 0;

	return x_node;
}

// y_node->z_axis1 should be sorted and of BSC_Z size.
// y_node->z_axis2 should be unsorted and of y_node->z_size size.

void FUNC(twin_merge_cpy)(STRUCT(x_node) *x_node, VAR *dest, STRUCT(y_node) *y_node, CMPFUNC *cmp)
{
	VAR *pta = y_node->z_axis1;
	VAR *ptb = y_node->z_axis2;
	size_t nmemb1 = BSC_Z;
	size_t nmemb2 = y_node->z_size;

	FUNC(quadsort_swap)(ptb, x_node->swap, nmemb2, cmp);

	while (1)
	{
		if (cmp(pta, ptb) <= 0)
		{
			*dest++ = *pta++;

			if (--nmemb1 == 0)
			{
				do *dest++ = *ptb++; while (--nmemb2);
				return;
			}
		}
		else
		{
			*dest++ = *ptb++;

			if (--nmemb2 == 0)
			{
				do *dest++ = *pta++; while (--nmemb1);
				return;
			}
		}
	}
}

// merge two sorted arrays across two buckets

void FUNC(twin_merge)(STRUCT(x_node) *x_node, STRUCT(y_node) *y_node1, STRUCT(y_node) *y_node2, CMPFUNC *cmp)
{
	VAR *pta, *ptb, *tpa, *tpb, *pts;
	unsigned int loop;

	FUNC(quadsort_swap)(y_node1->z_axis2, x_node->swap, y_node1->z_size, cmp);

	pta = y_node1->z_axis1;
	ptb = y_node1->z_axis2;
	tpa = pta + BSC_Z - 1;
	tpb = ptb + BSC_Z - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		pts = y_node1->z_axis2;
		y_node1->z_axis2 = y_node2->z_axis1;
		y_node2->z_axis1 = pts;

		return;
	}

	if (cmp(pta, tpb) > 0)
	{
		pts = y_node1->z_axis1;
		y_node1->z_axis1 = y_node1->z_axis2;
		y_node1->z_axis2 = y_node2->z_axis1;
		y_node2->z_axis1 = pts;

		return;
	}

	pts = y_node2->z_axis1;

	loop = BSC_Z;

	do
	{
		*pts++ = cmp(pta, ptb) <= 0 ? *pta++ : *ptb++;
	}
	while (--loop);

	pts = y_node2->z_axis2;

	if (cmp(tpa, tpb) <= 0)
	{
		do
		{
			*pts++ = cmp(pta, ptb) <= 0 ? *pta++ : *ptb++;
		}
		while (pta <= tpa);

		do
		{
			*pts++ = *ptb++;
		}
		while (ptb <= tpb);
	}
	else
	{
		do
		{
			*pts++ = cmp(pta, ptb) <= 0 ? *pta++ : *ptb++;
		}
		while (ptb <= tpb);

		do
		{
			*pts++ = *pta++;
		}
		while (pta <= tpa);
	}

	pts = y_node1->z_axis1;
	y_node1->z_axis1 = y_node2->z_axis1;
	y_node2->z_axis1 = y_node2->z_axis2;
	y_node2->z_axis2 = pts;
}

void FUNC(destroy_grid)(STRUCT(x_node) *x_node, VAR *array, CMPFUNC *cmp)
{
	STRUCT(y_node) *y_node;
	size_t y, z;

	for (y = z = 0 ; y < x_node->y_size ; y++)
	{
		y_node = x_node->y_axis[y];

		if (y_node->z_size)
		{
			FUNC(twin_merge_cpy)(x_node, &array[z], y_node, cmp);
		}
		else
		{
			memcpy(&array[z], y_node->z_axis1, BSC_Z * sizeof(VAR));
		}
		z += BSC_Z + y_node->z_size;

		free(y_node->z_axis1);
		free(y_node->z_axis2);

		free(y_node);
	}
	free(x_node->y_axis);
	free(x_node->y_base);
	free(x_node->swap);

	free(x_node);
}

size_t FUNC(adaptive_binary_search)(STRUCT(x_node) *x_node, VAR *array, VAR key, CMPFUNC *cmp)
{
	static unsigned int run;
	size_t top, mid;
	VAR *base = array;

	if (!run)
	{
		top = x_node->y_size;

		goto monobound;
	}

	if (x_node->y == x_node->y_size - 1)
	{
		if (cmp(base + x_node->y, &key) <= 0)
		{
			return x_node->y;
		}
		top = x_node->y;

		goto monobound;
	}

	if (x_node->y == 0)
	{
		base++;

		if (cmp(base, &key) > 0)
		{
			return 0;
		}
		top = x_node->y_size - 1;

		goto monobound;
	}

	base += x_node->y;

	if (cmp(base, &key) <= 0)
	{
		if (cmp(base + 1, &key) > 0)
		{
			goto end;
		}
		base++;
		top = x_node->y_size - x_node->y - 1;
		
	}
	else
	{
		base--;

		if (cmp(base, &key) <= 0)
		{
			goto end;
		}
		top = x_node->y - 1;
		base = array;
	}

	monobound:

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(base + mid, &key) <= 0)
		{
			base += mid;
		}
		top -= mid;
	}

	end:

	top = base - array;

	run = x_node->y == top;

	return x_node->y = top;
}

void FUNC(insert_y_node)(STRUCT(x_node) *x_node, size_t y)
{
	size_t end = ++x_node->y_size;

	if (x_node->y_size % BSC_Z == 0)
	{
		x_node->y_base = realloc(x_node->y_base, (x_node->y_size + BSC_Z) * sizeof(VAR));
		x_node->y_axis = realloc(x_node->y_axis, (x_node->y_size + BSC_Z) * sizeof(STRUCT(y_node) *));
	}

	while (y < --end)
	{
		x_node->y_axis[end] = x_node->y_axis[end - 1];
		x_node->y_base[end] = x_node->y_base[end - 1];
	}
	x_node->y_axis[y] = (STRUCT(y_node) *) malloc(sizeof(STRUCT(y_node)));

	x_node->y_axis[y]->z_axis1 = (VAR *) malloc(BSC_Z * sizeof(VAR));
	x_node->y_axis[y]->z_axis2 = (VAR *) malloc(BSC_Z * sizeof(VAR));
}

void FUNC(split_y_node)(STRUCT(x_node) *x_node, size_t y1, size_t y2, CMPFUNC *cmp)
{
	STRUCT(y_node) *y_node1, *y_node2;

	FUNC(insert_y_node)(x_node, y2);

	y_node1 = x_node->y_axis[y1];
	y_node2 = x_node->y_axis[y2];

	FUNC(twin_merge)(x_node, y_node1, y_node2, cmp);

	y_node1->z_size = y_node2->z_size = 0;

	x_node->y_base[y1] = y_node1->z_axis1[0];
	x_node->y_base[y2] = y_node2->z_axis1[0];
}

void FUNC(insert_z_node)(STRUCT(x_node) *x_node, VAR key, CMPFUNC *cmp)
{
	STRUCT(y_node) *y_node;
	size_t y;

	y = FUNC(adaptive_binary_search)(x_node, x_node->y_base, key, cmp);

	y_node = x_node->y_axis[y];

	y_node->z_axis2[y_node->z_size++] = key;

	if (y_node->z_size == BSC_Z)
	{
		FUNC(split_y_node)(x_node, y, y + 1, cmp);
	}
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

void FUNC(gridsort)(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	size_t cnt = nmemb;
	VAR *pta = (VAR *) array;

	STRUCT(x_node) *grid = FUNC(create_grid)(pta, cnt, cmp);

	pta += BSC_Z * 2;
	cnt -= BSC_Z * 2;

	while (cnt--)
	{
		FUNC(insert_z_node)(grid, *pta++, cmp);
	}

	FUNC(destroy_grid)(grid, (VAR *) array, cmp);
}
