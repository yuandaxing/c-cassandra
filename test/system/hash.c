#include "hash.h"

struct hashentry
{
	void *key;
	void *data;
	struct hashentry *next;
};

struct _hashtable
{
	unsigned int (*gethash)(void *);
	int (*compare)(void *, void *);
	int hashsize;
	int count;
	struct hashentry **hashlist;
	void keyFree(void *);
	void valueFree(void *);
};

#define hashindex(key, tab) ((tab->gethash)(key) % (tab->hashsize -1))

int lh_strhash(void *src, unsigned int *hash)
{
	int i, l;
	unsigned long ret = 0;
	unsigned short *s;
	char *str = (char *)src;
	if (str == NULL)
			return -1;
	l = (strlen(str) + 1) / 2;
	s = (unsigned short *)str;

	for (i = 0; i < l; i++) 
		ret ^= s[i]<<(i&0x0f);
	if(hash) *hash = ret;
	return 0;
}

int equal_str(void *k1, void *k2)
{
	return (0 == strcmp((char *)k1, (char *)k2));
}

inline struct hashentry *hashentry_new(void *key, void *data)
{
	struct hashentry *new = malloc(sizeof(struct hashentry));
	if(!new) return NULL;
	new->key = key;
	new->data = data;
	new->next = NULL;
	return new;
}

inline int hlist_append(struct hashentry **root, void *key, void *data)
{
	struct hashentry *l, *pos;
	l = hashentry_new(key, data);
	if(!l) return -1;
	if (*root == NULL) {
		*root = l;
	} else {
		for(pos = *root; pos->next != NULL; pos = pos->next);
				pos->next = l;
	}
	return  0;
}

int hlist_update(struct hashentry *root, void *key, void *data,
		        int (*compare)(void *, void *), void (*keyFree)(void *), void (*valueFree)(void *))
{
	struct hashentry *pos;
	for(pos = root; pos != NULL; pos = pos->next ) {
		if ( compare(key, pos->key) ) {
			valueFree(pos->data);     
			pos->data = data;
			keyFree(key);
			return 0;
		}
	}
	return -1;
}

inline struct hashentry *hashentry_free(struct hashentry *h)
{
	struct hashentry *next = h->next;
	free(h->key);
	free(h->data);
	free(h);
	h = NULL;
	return (next);
}

int hlist_remove(struct hashentry **root, void *key,
		                 int (*compare)(void *,void *))
{
	struct hashentry *pos ,*prev;

	if (NULL == *root) return -1;

	if (compare((*root)->key, key)) {
		*root = hashentry_free(*root);
		return 0;
	}

	prev = *root;
	for (pos = prev->next; NULL != pos; pos = pos->next) {
		if (compare(pos->key, key)) {
			prev->next = hashentry_free(pos);
			return 0;
		}
			prev = pos;
	}
		return -1;
}

hashtable *hash_create(unsigned int (*keyfunc)(void *),
		                       int (*comparefunc)(void *, void *),
							                          int size)
{
	int len = sizeof(struct hashentry *) * size; 
	int i;
	hashtable *tab = malloc( sizeof(hashtable) );
	memset(tab, 0, sizeof(hashtable));
	tab->hashlist = malloc(len);

	if (tab->hashlist == NULL) {
		free(tab);
		return NULL;
	}

	memset(tab->hashlist, 0, len );
	for (i = 0; i < size; i++)
		tab->hashlist[i] = NULL ;

	tab->compare = comparefunc;
	tab->gethash = keyfunc;
	tab->hashsize = size;
	tab->count    = 0;
	return tab;
}

void hash_free(hashtable *tab)
{
	int i;
	struct hashentry *pos;

	for (i = 0; i < tab->hashsize; i++)
		for (pos = tab->hashlist[i]; NULL != pos; pos = hashentry_free(pos));

	free(tab->hashlist);
	free(tab);
	tab =NULL;
}

void hash_insert(void *key, void *data, hashtable *tab)
{
	unsigned int index = hashindex(key, tab);
	struct hashentry *root = tab->hashlist[index];

	if ( hlist_update(root, key, data, tab->compare ) != 0 ) { //(1)
		hlist_append(&(tab->hashlist[index]), key, data );
		tab->count++;
	}
}
