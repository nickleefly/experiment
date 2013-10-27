#include <stdlib.h>
#include "rst.h"


/* Prototypes for functions only visible within this file. */
void rst_free_dfs(rst_node_t *p);


/* rst_alloc() - Allocates space for a radix search trie and returns a
 * pointer to it.  The function get_value() returns the key value for a data
 * item.
 */
rst_t *rst_alloc(unsigned int (* get_value)(const void *), int max_b)
{
    rst_t *t;
    rst_node_t *r;
    

    t = malloc(sizeof(rst_t));
    r = t->root = malloc(sizeof(rst_node_t));
    r->child_links = 0;
    r->a[0] = r->a[1] = NULL;
    t->n = 0;
    t->max_b = max_b;
    t->get_value = get_value;
    t->stack = malloc(max_b * sizeof(rst_node_t *));
    t->path_info = malloc(max_b * sizeof(int));

    return t;
}


/* rst_free() - Frees space used by the radix search trie pointed to by t.
 */
void rst_free(rst_t *t) {
    rst_free_dfs(t->root);
    free(t);
}

void rst_free_dfs(rst_node_t *p) {
    if(p->child_links & 1) rst_free_dfs(p->a[0]);
    if(p->child_links & 2) rst_free_dfs(p->a[1]);
    free(p);
}


/* rst_insert() - Inserts an item into the radix search trie pointed to by
 * t, according the the value its key.  The key of an item in the radix
 * search trie must be unique among items in the tree.  If an item with
 * the same key already exists in the tree, a pointer to that item is returned.
 * Otherwise, NULL is returned, indicating insertion was successful.
 */
void *rst_insert(rst_t *t, void *item)
{
    unsigned int key;
    unsigned int j, mask, stop_mask;
    int i;
    rst_node_t *x, *p;
    void *item2;
    int key2;

    
    key = t->get_value(item);
    mask = 1 << (i = t->max_b - 1);
    
    /* Locate the insertion position. */
    p = t->root;    
    for(;;) {
	/* Traverse left or right branch, depending on the current key bit, j.
	 * If the branch does not exist, then the insertion position has
	 * been located.  Note that the (j+1)th bit in p->child_links
	 * indicates the kind of pointer for p->a[j].
	 */
	j = (key & mask) >> i;
	if(!(p->child_links & (j+1))) break;
	p = p->a[j];

	/* Move to the next bit. */
	mask >>= 1;
	i--;
    }

    if((item2 = p->a[j])) {
	/* Check that the same item does not already exist in the tree. */
	key2 = t->get_value(item2);
	if(key2 == key) return p->a[j];  /* Insert failed. */
	/* else */

	/* Create new nodes as necessary, in order to distinguish the key of
	 * the inserted item from the key of the existing item.
	 * The variable stop_mask is used for determining where the bits of the
	 * two mkeys differ.
	 */
	stop_mask = key ^ key2;  /* Exclusive OR */
	x = malloc(sizeof(rst_node_t));
	p->a[j] = x;
	p->child_links = p->child_links | (j+1);  /* Set bit j. */
	for(;;) {
	    p = x;
	    
	    /* Move to the next bit. */
	    mask >>= 1;
	    i--;
	    j = (key & mask) >> i;

	    /* If the current bit value is different in key and key2, then
	     * no more new nodes need to be created.
	     */
	    if(mask & stop_mask) break;

	    x = malloc(sizeof(rst_node_t));
	    p->a[j] = x;
	    p->a[!j] = NULL;
	    p->child_links = (j+1);  /* Only bit j is set. */
	}

	p->a[j] = item;
	p->a[!j] = item2;
        p->child_links = 0;
    }
    else {
	p->a[j] = item;
    }

    t->n++;

    return NULL;
}


/* rst_find() - Find an item in the radix search trie with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *rst_find(rst_t *t, void *key_item) {
    unsigned int key;
    unsigned int j, mask;
    int i;
    rst_node_t *p;

    
    mask = 1 << (i = t->max_b - 1);
    key = t->get_value(key_item);
    
    /* Search for the item with key `key'. */
    p = t->root;
    for(;;) {
	/* Traverse left or right branch, depending on the current bit. */
	j = (key & mask) >> i;
	if(!(p->child_links & (j+1))) break;
	p = p->a[j];

	/* Move to the next bit. */
	mask >>= 1;
	i--;
    }

    if(!p->a[j] || t->get_value(p->a[j]) != key) return NULL;
    
    return p->a[j];
}


/* rst_find() - Find an item in the radix search trie with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *rst_find_min(rst_t *t)
{
    unsigned int j;
    rst_node_t *p;

    p = t->root;
    for(;;) {
	j = p->a[0] ? 0 : 1;
	if(!(p->child_links & (j+1))) break;
	p = p->a[j];
    }

    return p->a[j];
}



/* rst_delete() - Delete the first item found in the radix search trie with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
void *rst_delete(rst_t *t, void *key_item) {
    rst_node_t *p;
    unsigned int mask, i, j;
    unsigned int key;
    rst_node_t **stack;
    int *path_info, tos;
    void *y, *return_item;

    
    mask = 1 << (i = t->max_b - 1);
    key = t->get_value(key_item);
    stack = t->stack;
    path_info = t->path_info;
    tos = 0;
    
    /* Search for the item with key `key'. */
    p = t->root;
    for(;;) {
	
	/* Traverse left or right branch, depending on the current bit. */
	j = (key & mask) >> i;
	stack[tos] = p;
	path_info[tos] = j;
	tos++;
	if(!(p->child_links & (j+1))) break;
	p = p->a[j];

	/* Move to the next bit. */
	mask >>= 1;
	i--;
    }

    /* The delete operation fails if the tree contains no items, or no mathcing
     * item was found.
     */
    if(!p->a[j] || t->get_value(p->a[j]) != key) return NULL;
    return_item = p->a[j];

    /* Currently p->a[j] points to the item which is to be removed.  After
     * removing the deleted item, the parent node must also be deleted if its
     * other child pointer is NULL.  This deletion can propagate up to the next
     * level.
     */
    tos--;
    for(;;) {
	if(tos == 0) {  /* Special case: deleteing a child of the root node. */
	    p->a[j] = NULL;
	    p->child_links = p->child_links & !(j+1);  /* Clear bit j. */
	    return return_item;
	}
	
        if(p->a[!j]) break;
	
	free(p);
        p = stack[--tos];
	j = path_info[tos];
    }
    
    /* For the current node, p, the child pointer p->a[!j] is not NULL.
     * If p->a[!j] points to a node, we set p->a[j] to NULL.  Otherwise if
     * p->a[!j] points to an item, we keep a pointer to the item, and
     * continue to delete parent nodes if thier other child pointer is NULL.
     */
    if(p->child_links & (!j+1)) {  /* p->a[!j] points to a node. */
        p->a[j] = NULL;
    }
    else {  /* p->a[!j] points to an item. */

        /* Delete p, and parent nodes for which the other child pointer is
	 * NULL.
	 */
        y = p->a[!j];
        do {
	    free(p);
            p = stack[--tos];
	    j = path_info[tos];
	    if(p->a[!j]) break;
        } while(tos != 0);

        /* For the current node, p, p->a[!j] is not NULL.  We assign item y to
	 * p->a[j].
	 */
	p->a[j] = y;
    }
    p->child_links = p->child_links & ~(j+1);  /* Clear bit j. */

    
    return return_item;
}



/* rst_delete_min() - Deletes the item with the smallest key from the radix
 * search trie pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *rst_delete_min(rst_t *t)
{
    rst_node_t *p;
    unsigned int j;
    rst_node_t **stack;
    int *path_info, tos;
    void *y, *return_item;


    stack = t->stack;
    path_info = t->path_info;
    tos = 0;


    /* Locate the minimum item. */
    p = t->root;
    for(;;) {
	j = p->a[0] ? 0 : 1;
	stack[tos] = p;
	path_info[tos] = j;
	tos++;
	if(!(p->child_links & (j+1))) break;
	p = p->a[j];
    }

    return_item = p->a[j];

    /* Currently p->a[j] points to the item which is to be removed.  After
     * removing the deleted item, the parent node must also be deleted if its
     * other child pointer is NULL.  This deletion can propagate up to the next
     * level.
     */
    tos--;
    for(;;) {
	if(tos == 0) {  /* Speical case:  removing a child of the root node. */
	    /* For the special case where the tree contains no items, p->a[j]
	     * is NULL, and this code results in no changes, and returns NULL.
	     */
	    p->a[j] = NULL;
	    p->child_links = p->child_links & ~(j+1);  /* Clear bit j. */
	    return return_item;
	}
	
        if(p->a[!j]) break;
	
	free(p);
        p = stack[--tos];
	j = path_info[tos];
    }
    
    /* For the current node, p, the child pointer p->a[!j] is not NULL.
     * If p->a[!j] points to a node, we set p->a[j] to NULL.  Otherwise if
     * p->a[!j] points to an item, we keep a pointer to the item, and
     * continue to delete parent nodes if thier other child pointer is NULL.
     */
    if(p->child_links & (!j+1)) {  /* p->a[!j] points to a node. */
        p->a[j] = NULL;
    }
    else {  /* p->a[!j] points to an item. */

        /* Delete p, and parent nodes for which the other child pointer is
	 * NULL.
	 */
        y = p->a[!j];
        do {
	    free(p);
            p = stack[--tos];
	    j = path_info[tos];
	    if(p->a[!j]) break;
        } while(tos != 0);

        /* For the current node, p, p->a[!j} is not NULL.  We assign item y to
	 * p->a[j].
	 */
	p->a[j] = y;
    }
    p->child_links = p->child_links & ~(j+1);  /* Clear bit j. */

    return return_item;
}


/*** Implement the universal dictionary structure type ***/

/*** Radix search tree wrapper functions. ***/

void *_rst_alloc(int (* compar)(const void *, const void *),
		 unsigned int (* getval)(const void *)) {
    return rst_alloc(getval, 8*sizeof(unsigned int));
}

void _rst_free(void *t) {
    rst_free((rst_t *)t);
}

void *_rst_insert(void *t, void *item) {
    return rst_insert((rst_t *)t, item);
}

void *_rst_delete(void *t, void *key_item) {
    return rst_delete((rst_t *)t, key_item);
}

void *_rst_delete_min(void *t) {
    return rst_delete_min((rst_t *)t);
}

void *_rst_find(void *t, void *key_item) {
    return rst_find((rst_t *)t, key_item);
}

void *_rst_find_min(void *t) {
    return rst_find_min((rst_t *)t);
}

/* Radix search tree info */
const dict_info_t RST_info = {
    _rst_alloc,
    _rst_free,
    _rst_insert,
    _rst_delete,
    _rst_delete_min,
    _rst_find,
    _rst_find_min
};
