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

void FUNC(split_y_node)(STRUCT(x_node) *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp);

STRUCT(x_node) *FUNC(create_grid)(VAR *array, size_t nmemb, CMPFUNC *cmp)
{
	STRUCT(x_node) *x_node = (STRUCT(x_node) *) malloc(sizeof(STRUCT(x_node)));
	STRUCT(y_node) *y_node = (STRUCT(y_node) *) malloc(sizeof(STRUCT(y_node)));

	for (BSC_Z = BSC_X ; BSC_Z * BSC_Z * BSC_Y < nmemb ; BSC_Z *= 2);

	x_node->swap = (VAR *) malloc(BSC_Z * sizeof(VAR));

	x_node->y_base = (VAR *) malloc(BSC_X * sizeof(VAR));

	x_node->y_axis = (STRUCT(y_node) **) malloc(BSC_X * sizeof(STRUCT(y_node) *));

	x_node->y_axis[0] = y_node;

	// avoid unneeded binary searches by creating two y nodes right away

	if (FUNC(quad_swap)(array, BSC_Z * 2, cmp) == 0)
	{
		FUNC(quad_merge)(array, x_node->swap, BSC_Z * 2, 16, cmp);
	}

	y_node->z_axis1 = (VAR *) malloc(BSC_Z * sizeof(VAR));
	memcpy(y_node->z_axis1, array, BSC_Z * sizeof(VAR));

	y_node->z_axis2 = (VAR *) malloc(BSC_Z * sizeof(VAR));
	memcpy(y_node->z_axis2, array + BSC_Z, BSC_Z * sizeof(VAR));

	x_node->y_size = 1;
	x_node->y = 0;

	x_node->y_base[0] = y_node->z_axis1[0];

	y_node->z_size = BSC_Z;

	FUNC(split_y_node)(x_node, 0, 1, cmp);

	return x_node;
}

// merge two sorted arrays

void FUNC(twin_merge)(VAR *swap, STRUCT(y_node) *y_node, CMPFUNC *cmp)
{
	register VAR *pta, *ptb, *tpa, *tpb, *pts, *tps;

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

void FUNC(twin_merge_cpy)(VAR *dest, STRUCT(y_node) *y_node, size_t nmemb1, size_t nmemb2, CMPFUNC *cmp)
{
	register VAR *pta, *ptb, *tpa, *tpb;

	pta = y_node->z_axis1;
	ptb = y_node->z_axis2;
	tpa = pta + nmemb1 - 1;
	tpb = ptb + nmemb2 - 1;

	if (cmp(tpa, ptb) <= 0)
	{
		memcpy(dest, pta, nmemb1 * sizeof(VAR));

		dest += nmemb1;

		memcpy(dest, ptb, nmemb2 * sizeof(VAR));

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

void FUNC(bulksort_cpy)(STRUCT(x_node) *x_node, VAR *dest, STRUCT(y_node) *y_node, CMPFUNC *cmp)
{
	if (y_node->z_size > 128)
	{
		if (FUNC(quad_swap)(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			FUNC(quad_merge)(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else if (y_node->z_size > 16)
	{
		if (FUNC(quad_swap)(y_node->z_axis2, y_node->z_size, cmp) == 0)
		{
			FUNC(tail_merge)(y_node->z_axis2, x_node->swap, y_node->z_size, 16, cmp);
		}
	}
	else
	{
		FUNC(tail_swap)(y_node->z_axis2, y_node->z_size, cmp);
	}

	if (dest)
	{
		FUNC(twin_merge_cpy)(dest, y_node, BSC_Z, y_node->z_size, cmp);
	}
	else if (y_node->z_axis1)
	{
		FUNC(twin_merge)(x_node->swap, y_node, cmp);
	}
}

size_t FUNC(adaptive_binary_search)(STRUCT(x_node) *x_node, VAR *array, VAR key, CMPFUNC *cmp)
{
	size_t top, mid;
	VAR *base = array;

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

void FUNC(destroy_grid)(STRUCT(x_node) *x_node, VAR *array, CMPFUNC *cmp)
{
	size_t z_len = 0;

	if (x_node->y_size)
	{
		STRUCT(y_node) *y_node;
		size_t y;

		for (y = 0 ; y < x_node->y_size ; y++)
		{
			y_node = x_node->y_axis[y];

			if (y_node->z_size)
			{
				FUNC(bulksort_cpy)(x_node, &array[z_len], y_node, cmp);
			}
			else
			{
				memcpy(&array[z_len], y_node->z_axis1, BSC_Z * sizeof(VAR));
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

void FUNC(insert_y_node)(STRUCT(x_node) *x_node, unsigned short y)
{
	unsigned short end = ++x_node->y_size;

	if (x_node->y_size % BSC_X == 0)
	{
		x_node->y_base = realloc(x_node->y_base, (x_node->y_size + BSC_X) * sizeof(VAR));
		x_node->y_axis = realloc(x_node->y_axis, (x_node->y_size + BSC_X) * sizeof(STRUCT(y_node) *));
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

void FUNC(split_y_node)(STRUCT(x_node) *x_node, unsigned short y1, unsigned short y2, CMPFUNC *cmp)
{
	STRUCT(y_node) *y_node1, *y_node2;
	VAR *swap;

	FUNC(insert_y_node)(x_node, y2);

	y_node1 = x_node->y_axis[y1];
	y_node2 = x_node->y_axis[y2];

	if (x_node->y_size > 2)
	{
		FUNC(bulksort_cpy)(x_node, NULL, y_node1, cmp);
	}

	y_node1->z_size = y_node2->z_size = 0;

	swap = y_node2->z_axis1; y_node2->z_axis1 = y_node1->z_axis2; y_node1->z_axis2 = swap;

	x_node->y_base[y1] = y_node1->z_axis1[0];
	x_node->y_base[y2] = y_node2->z_axis1[0];
}
