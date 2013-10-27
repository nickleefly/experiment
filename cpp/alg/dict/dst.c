#include <stdlib.h>
#include "dst.h"


/* Prototypes for functions only visible within this file. */
void dst_free_dfs(dst_node_t *p);


/* dst_alloc() - Allocates space for a digital search tree and returns a
 * pointer to it.  The function get_value() returns the key value for a data
 * item.
 */
dst_t *dst_alloc(unsigned int (* get_value)(const void *), int max_b)
{
    dst_t *t;
    

    t = malloc(sizeof(dst_t));
    t->root = NULL;
    t->n = 0;
    t->max_b = max_b;
    t->get_value = get_value;

    return t;
}


/* dst_free() - Frees space used by the digital search tree pointed to by t.
 */
void dst_free(dst_t *t) {
    dst_free_dfs(t->root);
    free(t);
}

void dst_free_dfs(dst_node_t *p) {
    if(!p) return;
    dst_free_dfs(p->a[0]);
    dst_free_dfs(p->a[1]);
    free(p);
}


/* dst_insert() - Inserts an item into the digital search tree pointed to by
 * t, according the the value its key.  The key of an item in the digital
 * search tree tree must be unique among items in the tree.  If an item with
 * the same key already exists in the tree, a pointer to that item is returned.
 * Otherwise, NULL is returned, indicating insertion was successful.
 */
void *dst_insert(dst_t *t, void *item)
{
    unsigned int key;
    unsigned int j, mask;
    int i;
    dst_node_t *x, *p, *next_p, **child_link;

    
    key = t->get_value(item);

    if((next_p = t->root)) {;

        mask = 1 << (i = t->max_b - 1);
    
        /* Locate the insertion position. */
    
        for(;;) {
	    p = next_p;

	    if(p->key == key) {
		return p->item;  /* Item found. */
	    }
	    /* else */
	    
            /* Traverse left or right branch, depending on the current bit. */
	    j = (key & mask) >> i;
	    next_p = p->a[j];

            if(!next_p) {  /* Empty position to insert at. */
		child_link = &p->a[j];
		break;
	    }

	    /* Move to the next bit. */
	    mask >>= 1;
	    i--;
	}
    }
    else {
	child_link = &t->root;
    }

    /* Create a new node. */
    x = malloc(sizeof(dst_node_t));
    x->a[0] = x->a[1] = NULL;
    x->key = key;
    x->item = item;

    /* Join the new node onto the tree. */
    *child_link = x;

    t->n++;

    return NULL;
}


/* dst_find() - Find an item in the digital search tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *dst_find(dst_t *t, void *key_item) {
    unsigned int key;
    unsigned int j, mask;
    int i;
    dst_node_t *p, *next_p;
    
    if((next_p = t->root)) {;

        mask = 1 << (i = t->max_b - 1);
        key = t->get_value(key_item);
    
        /* Search for the node with key `key'. */
    
        for(;;) {
	    p = next_p;

	    if(p->key == key) {
		return p->item;  /* Item found. */
	    }
	    /* else */
	    
            /* Traverse left or right branch, depending on the current bit. */
	    j = (key & mask) >> i;
	    next_p = p->a[j];
            if(!next_p) {
		break;
	    }

	    /* Move to the next bit. */
	    mask >>= 1;
	    i--;
	}
    }

    return NULL;
}


/* dst_find() - Find an item in the digital search tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *dst_find_min(dst_t *t)
{
    unsigned int min_key;
    dst_node_t *p, *next_p, *min_node;

    if((next_p = t->root)) {
	min_key = 0xFFFFFFFF;  /* Note the sign bit. */
	for(;;) {
	    p = next_p;
	    if(p->key <= min_key) {
	        min_node = p;
		min_key = p->key;
	    }
	    next_p = p->a[0] ? p->a[0] : p->a[1];
	    if(!next_p) return min_node->item;
        }
    }

    return NULL;
}


/* This digital search tree implementation contains code for two possible
 * rearrangement mechanisms to use after deleting a node.
 */
/* dst_delete() - Delete the first item found in the digital search tree with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
#if DST_FAST_DELETE
void *dst_delete(dst_t *t, void *key_item) {
    dst_node_t *p, *next_p, *q, *next_q;
    dst_node_t **child_link, **del_child_link;
    dst_node_t *return_item;
    unsigned int mask, i, j;
    unsigned int key;

    
    if((next_p = t->root)) {
	del_child_link = &t->root;

        mask = 1 << (i = t->max_b - 1);
        key = t->get_value(key_item);
    
        /* Locate the delete position. */
    
        for(;;) {
	    p = next_p;

	    if(p->key == key) {
		break;  /* Item found. */
	    }
	    /* else */
	    
            /* Traverse left or right branch, depending on the current bit. */
	    j = (key & mask) >> i;
	    next_p = p->a[j];
            if(!next_p) {
		return NULL;
	    }
	    del_child_link = &p->a[j];

	    /* Move to the next bit. */
	    mask >>= 1;
	    i--;
	}
    }
    else {
	return NULL;
    }


    /* The node to be removed from the tree is pointed to by p.  The node, q,
     * q replace p is located at the leaf level of the tree.
     */
    q = p;
    for(;;) {
	/* Traverse left or right branch, depending on the current bit for a
	 * random enough value.
	 */
	j = (key & mask) >> i;
	if(!q->a[j]) j = !j;
	next_q = q->a[j];
	if(!next_q) {
	    break;
	}
	child_link = &q->a[j];

        /* Move to the next bit. */
	mask >>= 1;
	i--;
	
	q = next_q;
    }

    /* Node q is moved to replace node p. */
    if(p != q) {
        *child_link = NULL;
        *del_child_link = q;
        q->a[0] = p->a[0];
        q->a[1] = p->a[1];
    }
    else {
        *del_child_link = NULL;
    }
    
    return_item = p->item;
    free(p);

    return return_item;
}
#else
void *dst_delete(dst_t *t, void *key_item) {
    dst_node_t *p, *next_p, *q, *next_q, *q2, *next_q2;
    dst_node_t **child_link;
    dst_node_t *return_item;
    unsigned int mask, i, j, next_j;
    unsigned int key;

    
    if((next_p = t->root)) {
	child_link = &t->root;

        mask = 1 << (i = t->max_b - 1);
        key = t->get_value(key_item);
    
        /* Locate the delete position. */
    
        for(;;) {
	    p = next_p;

	    if(p->key == key) {
		break;  /* Item found. */
	    }
	    /* else */
	    
            /* Traverse left or right branch, depending on the current bit. */
	    j = (key & mask) >> i;
	    next_p = p->a[j];
            if(!next_p) {
		return NULL;
	    }
	    child_link = &p->a[j];

	    /* Move to the next bit. */
	    mask >>= 1;
	    i--;
	}
    }
    else {
	return NULL;
    }


    /* The node to be removed from the tree is pointed to by p.  After p is
     * removed, one of p's children is moved up to replace it.  This `sift-up'
     * process will continue untill a leaf node is reached.
     */

    /* To randomly choose the child, q, to replace p, we use the value of the
     * least significant bit of the key of p.  If there is only one child, then
     * q will point to that child.
     */
    j = p->key & 1;
    if(!p->a[j]) {
	j = !j;
    }
    q = p->a[j];
    q2 = p->a[!j];    
    *child_link = q;
	
    while(q) {
	/* Before assigning left and right children to q, obtain the next
	 * pointers since they will be overwritten.
	 */
	next_j = q->key & 1; /* Compare to next_j = 0; */
	if(!q->a[next_j]) {
	    next_j = !next_j;
	}
	next_q = q->a[next_j];
	next_q2 = q->a[!next_j];

	/* Assign left and right pointers. */
	q->a[!j] = q2;
	q->a[j] = next_q;
	    
        /* Obtain the next values for j, q, and q2. */
	j = next_j;
	q = next_q;
	q2 = next_q2;
    }


    return_item = p->item;
    free(p);

    return return_item;
}
#endif



/* dst_delete_min() - Deletes the item with the smallest key from the digital
 * search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *dst_delete_min(dst_t *t)
{
    dst_node_t *p, *next_p, **child_link, *return_item;
    dst_node_t *min_node, **min_child_link;
    unsigned int j, min_key;

    if(!(next_p = t->root)) {
	return NULL;
    }

    min_key = 0xFFFFFFFF;
    child_link = &t->root;
    for(;;) {
	p = next_p;
	if(p->key <= min_key) {
            min_node = p;
	    min_child_link = child_link;
	    min_key = p->key;
	}
	j = p->a[0] ? 0 : 1;
	next_p = p->a[j];
	if(!next_p) break;
	child_link = &p->a[j];
    }

    /* Node p is moved to replace min_node.  This may actaully be better than
     * replacing min_node with a random descendant, since over a series of
     * delete-min operations we can expect the minimum to be found earlier on
     * in the search.  This would result in fewer updates to the min_node,
     * min_child_link, and min_key variables during the search.
     */
    *child_link = NULL;
    if(min_node != p) {
        *min_child_link = p;
        p->a[0] = min_node->a[0];
        p->a[1] = min_node->a[1];
    }

    return_item = min_node->item;
    free(min_node);

    return return_item;
}


/*** Implement the universal dictionary structure type ***/

/*** Digital search tree wrapper functions. ***/

void *_dst_alloc(int (* compar)(const void *, const void *),
		 unsigned int (* getval)(const void *)) {
    return dst_alloc(getval, 8*sizeof(unsigned int));
}

void _dst_free(void *t) {
    dst_free((dst_t *)t);
}

void *_dst_insert(void *t, void *item) {
    return dst_insert((dst_t *)t, item);
}

void *_dst_delete(void *t, void *key_item) {
    return dst_delete((dst_t *)t, key_item);
}

void *_dst_delete_min(void *t) {
    return dst_delete_min((dst_t *)t);
}

void *_dst_find(void *t, void *key_item) {
    return dst_find((dst_t *)t, key_item);
}

void *_dst_find_min(void *t) {
    return dst_find_min((dst_t *)t);
}

/* Digital search tree info */
const dict_info_t DST_info = {
    _dst_alloc,
    _dst_free,
    _dst_insert,
    _dst_delete,
    _dst_delete_min,
    _dst_find,
    _dst_find_min
};
