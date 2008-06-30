/*
 *
 *	Copyright (C) 1981, Regents of the University of California
 *
 *	All rights reserved
 *
 */

/*
 * hash, hsearch
 */

int
hash (s, hshsiz)
register char *s;
int hshsiz;
{
	register long hash = 0;
	int retval;

	while (*s)
		hash += hash + *s++;
	if (hash < 0)
		hash = -hash;
	retval = hash % hshsiz;
	return (retval);
}

/*
 * hash search
 *	'key' is object to be found.
 *	'base' is the start of hash table.
 *	'nel' is the number of elements in the hash table.
 *	'width' is the size of a hash table entry.
 *	'compar' is passed key and a ptr into hash table,
 *		and must return 0 iff entry matches key.
 *	'open' is passed a ptr into the hash table,
 *		and must return 0 iff entry is empty.
 *	'slot' is set to point to a ptr into the hash table
 *		where the entry for key may be placed if not found,
 *		or is set to point to 0 if the table is full.
 *
 *	hash is called for first hash, linear rehash used.
 */

char *
hsearch (key, base, nel, width, compar, open, slot)
char *key;
register char *base;
int nel;
register unsigned width;
int (*compar)();
int (*open)();
char **slot;
{
	register int i = width * hash (key, nel);
	int nnel;

	for (nnel = nel; nnel > 0; nnel--)
		if ((*open)(base+i)) {
			*slot = base+i;
			return ((char *) 0);
		}
		else	if ((*compar)(key, base+i))
				i = (i+width) % (width*nel);
			else	return (base+i);
	return (*slot = (char *) 0);
}
