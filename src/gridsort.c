// gridsort 1.2.1.3 - Igor van den Hoven ivdhoven@gmail.com

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

	for (BSC_Z = BSC_X ; BSC_Z * BSC_Z / 4 < nmemb ; BSC_Z *= 4);

	x_node->swap = (VAR *) malloc(BSC_Z * 2 * sizeof(VAR));

	x_node->y_base = (VAR *) malloc(BSC_Z * sizeof(VAR));

	x_node->y_axis = (STRUCT(y_node) **) malloc(BSC_Z * sizeof(STRUCT(y_node) *));

	FUNC(quadsort_swap)(array, x_node->swap, BSC_Z * 2, BSC_Z * 2, cmp);

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

// used by destroy_grid

// y_node->z_axis1 should be sorted and of BSC_Z size.
// y_node->z_axis2 should be unsorted and of y_node->z_size size.

void FUNC(twin_merge_cpy)(STRUCT(x_node) *x_node, VAR *dest, STRUCT(y_node) *y_node, CMPFUNC *cmp)
{
	VAR *ptl = y_node->z_axis1;
	VAR *ptr = y_node->z_axis2;
	size_t nmemb1 = BSC_Z;
	size_t nmemb2 = y_node->z_size;
	VAR *tpl = y_node->z_axis1 + nmemb1 - 1;
	VAR *tpr = y_node->z_axis2 + nmemb2 - 1;
	VAR *ptd = dest;
	VAR *tpd = dest + nmemb1 + nmemb2 - 1;
	size_t loop, x, y;

	FUNC(quadsort_swap)(ptr, x_node->swap, nmemb2, nmemb2, cmp);

	while (1)
	{
		if (tpl - ptl > 8)
		{
			ptl8_ptr: if (cmp(ptl + 7, ptr) <= 0)
			{
				memcpy(ptd, ptl, 8 * sizeof(VAR)); ptd += 8; ptl += 8;

				if (tpl - ptl > 8) {goto ptl8_ptr;} continue;
			}

			tpl8_tpr: if (cmp(tpl - 7, tpr) > 0)
			{
				tpd -= 7; tpl -= 7; memcpy(tpd--, tpl--, 8 * sizeof(VAR));

				if (tpl - ptl > 8) {goto tpl8_tpr;} continue;
			}
		}

		if (tpr - ptr > 8)
		{
			ptl_ptr8: if (cmp(ptl, ptr + 7) > 0)
			{
				memcpy(ptd, ptr, 8 * sizeof(VAR)); ptd += 8; ptr += 8;

				if (tpr - ptr > 8) {goto ptl_ptr8;} continue;
			}

			tpl_tpr8: if (cmp(tpl, tpr - 7) <= 0)
			{
				tpd -= 7; tpr -= 7; memcpy(tpd--, tpr--, 8 * sizeof(VAR));

				if (tpr - ptr > 8) {goto tpl_tpr8;} continue;
			}
		}

		if (tpd - ptd < 16)
		{
			break;
		}

		loop = 8; do
		{
			head_branchless_merge(ptd, x, ptl, ptr, cmp);
			tail_branchless_merge(tpd, y, tpl, tpr, cmp);
		}
		while (--loop);
	}

	while (tpl - ptl > 1 && tpr - ptr > 1)
	{
		if (cmp(ptl + 1, ptr) <= 0)
		{
			*ptd++ = *ptl++; *ptd++ = *ptl++;
		}
		else if (cmp(ptl, ptr + 1) > 0)
		{
			*ptd++ = *ptr++; *ptd++ = *ptr++;
		}
		else 
		{
			x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += 1; ptd[y] = *ptl; ptl += 1; ptd += 2;
			x = cmp(ptl, ptr) <= 0; y = !x; ptd[x] = *ptr; ptr += y; ptd[y] = *ptl; ptl += x; ptd++;
		}
	}

	while (ptl <= tpl && ptr <= tpr)
	{
		*ptd++ = cmp(ptl, ptr) <= 0 ? *ptl++ : *ptr++;
	}
	while (ptl <= tpl)
	{
		*ptd++ = *ptl++;
	}
	while (ptr <= tpr)
	{
		*ptd++ = *ptr++;
	}
}

void FUNC(parity_twin_merge)(VAR *ptl, VAR *ptr, VAR *ptd, VAR *tpd, size_t block, CMPFUNC *cmp)
{
	VAR *tpl, *tpr;
#if !defined __clang__
	unsigned char x, y;
#endif
	tpl = ptl + block - 1;
	tpr = ptr + block - 1;

	for (block-- ; block ; block--)
	{
		head_branchless_merge(ptd, x, ptl, ptr, cmp);
		tail_branchless_merge(tpd, y, tpl, tpr, cmp);
	}
	*ptd = cmp(ptl, ptr) <= 0 ? *ptl : *ptr;
	*tpd = cmp(tpl, tpr)  > 0 ? *tpl : *tpr;
}

// merge two sorted arrays across two buckets
// [AB][AB] --> [AA][  ] + [BB][  ]

void FUNC(twin_merge)(STRUCT(x_node) *x_node, STRUCT(y_node) *y_node1, STRUCT(y_node) *y_node2, CMPFUNC *cmp)
{
	VAR *pta, *ptb, *tpa, *tpb, *pts;

	FUNC(quadsort_swap)(y_node1->z_axis2, x_node->swap, BSC_Z, BSC_Z, cmp);

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

	FUNC(parity_twin_merge)(pta, ptb, y_node2->z_axis2, y_node2->z_axis1 + BSC_Z - 1, BSC_Z, cmp);

	pta = y_node1->z_axis1; y_node1->z_axis1 = y_node2->z_axis2; y_node2->z_axis2 = pta;
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
		x_node->y_base = (VAR *) realloc(x_node->y_base, (x_node->y_size + BSC_Z) * sizeof(VAR));
		x_node->y_axis = (STRUCT(y_node) **) realloc(x_node->y_axis, (x_node->y_size + BSC_Z) * sizeof(STRUCT(y_node) *));
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
