/*** File tree23_ext.c - Extended 2-3 Tree Implementation. ***/
/* This 2-3 tree implementation supports O(1) worst case time for delete_min.
 */
/*
 *   Shane Saunders
 */
#include <malloc.h>
#include <stdio.h>
#include "tree23_ext.h"


/* tree23_alloc() - Allocates space for a 2-3 tree and returns a pointer to
 * it.  The function compar compares they keys of two items, and returns a
 * negative, zero, or positive integer depending on whether the first item is
 * less than, equal to, or greater than the second.
 */
tree23_t *tree23_alloc(int (* compar)(const void *, const void *))
{
    tree23_t *t;
    tree23_node_t *r;
    
    t = malloc(sizeof(tree23_t));
    t->n = 0;
    t->height = 1;
    t->compar = compar;
    r = t->min_node = t->root = malloc(sizeof(tree23_node_t));
    r->key_node1 = r->key_node2 = NULL;
    r->parent = NULL;
    r->left.item = r->middle.item = r->right.item = NULL;

    return t;
}


/* tree23_free() - Frees space used by the 2-3 tree pointed to by t. */
void tree23_free(tree23_t *t)
{
    int tos;
    tree23_node_t *p, **stack;

    /* In order to free all nodes in the tree a depth first search is performed
     * This is implemented using a stack.
     */

    stack = malloc((2 * t->height - 1) * sizeof(tree23_node_t *));
    stack[0] = t->root;
    tos = 1;

    while(tos) {
        p = stack[--tos];
        if(p->key_node1) {
	    stack[tos++] = p->left.node;
	    stack[tos++] = p->middle.node;
	    if(p->right.node) stack[tos++] = p->right.node;
	}
	free(p);
    }

    free(stack);

    free(t);
}


/* tree23_insert() - Inserts an item into the 2-3 tree pointed to by t,
 * according the the value its key.  The key of an item in the 2-3 tree must
 * be unique among items in the tree.  If an item with the same key already
 * exists in the tree, a pointer to that item is returned.  Otherwise, NULL is
 * returned, indicating insertion was successful.
 */
void *tree23_insert(tree23_t *t, void *item)
{
    int (* compar)(const void *, const void *);
    int cmp_result;
    void *temp_item, *x_min, *return_item;
    tree23_node_t *new_node, *x, *p, *temp_node, *parent;
    
    p = t->root;
    compar = t->compar;

    /* Special case: only zero or one items in the tree already. */
    if(t->n <= 1) {
	if(t->n == 0) {  /* 0 items --> 1 item */   
	    p->left.item = item;
	}
	else {  /* 1 item --> 2 items */
	    cmp_result = compar(item, p->left.item);

	    /* Check that an item with the same key does not already exist. */
	    if(cmp_result == 0) return p->left.item;
	    /* else */
	    
            /* Determine insertion position. */
	    if(cmp_result > 0) {  /* Insert as middle child. */	
	        p->middle.item = item;
	    }
	    else {  /* Insert as left child. */
                p->middle.item = p->left.item;
		p->left.item = item;
	    }
	}

	t->n++;
	return NULL;  /* Insertion successful. */
    }

    /* Search the tree to locate the insertion position. */
    while(p->key_node1) {
	if(p->key_node2 && compar(item, p->key_node2->left.item) >= 0) {
	    p = p->right.node;
	}
	else if(compar(item, p->key_node1->left.item) >= 0) {
	    p = p->middle.node;
	}
	else {
	    p = p->left.node;
	}
    }
    
    /* Insert at the leaf items of node p. */
    if(p->right.item && (cmp_result = compar(item, p->right.item)) >= 0) {
        /* Insert beside right branch. */

	/* Check if the same key already exists. */
	if(cmp_result == 0) {
	    return_item = p->right.item;  /* Insertion failed. */
	    return return_item;
	}
	/* else */
	
        /* Create a new node.  Node p's right child becomes the left child
	 * of the new node.  The inserted item is the middle child of the
	 * new node.
	 */
	new_node = malloc(sizeof(tree23_node_t));
	new_node->key_node1 = new_node->key_node2 = NULL;
	new_node->left.item = p->right.item;
	new_node->middle.item = item;
	new_node->right.item = NULL;
	p->right.item = NULL;

	/* Insertion for new_node will continue higher up in the tree. */
    }
    else if((cmp_result = compar(item, p->middle.item)) >= 0) {
        /* Insert beside the middle branch. */

	/* Check if the same key already exists. */
	if(cmp_result == 0) {
	    return_item = p->middle.item;  /* Insertion failed. */
	    return return_item;
	}
	/* else */
	
        /* Insertion depends on the number of children node p currently has. */
	if(p->right.item) {  /* Node p has three children. */
	    
            /* Create a new node.  The inserted item is the left child of
	     * the new node.  Node p's right child becomes the middle
	     * child of the new node.
	     */
	    new_node = malloc(sizeof(tree23_node_t));
	    new_node->key_node1 = new_node->key_node2 = NULL;
	    new_node->left.item = item;
	    new_node->middle.item = p->right.item;
	    new_node->right.item = NULL;
	    p->right.item = NULL;

	    /* Insertion for new_node will continue higher up in the tree. */
	}
	else {  /* Node p has two children. */
	    
	    /* The item is inserted as the right child of node p. */
	    p->right.item = item;

	    /* No need to insert higher up. */
	    return_item = NULL;  /* Insertion successful. */
	    t->n++;
	    return return_item;
	}
    }
    else {
        /* Insert beside the left branch. */

	/* Account for the special case, where the item being inserted is
	 * smaller than any other item in the tree.
	 */
	if((cmp_result = compar(item, p->left.item)) <= 0) {
	    
	    /* Check if the same key already exists in the tree. */
	    if(cmp_result == 0) {
		return_item = p->left.item;  /* Insertion failed. */
		return return_item;
	    }
	    /* else */

	    /* The item being inserted is smaller than any other item in the
	     * tree.  Treat p's left child as the item begin inserted.  This
	     * done by replacing p's left child with the item being inserted.
	     */
	    temp_item = item;
	    item = p->left.item;
	    p->left.item = temp_item;
	}

        /* Insertion depends on the number of children node p currently has. */
	if(p->right.item) {  /* Node p has three children. */
	    
            /* Create a new node.  Node p's middle child becomes the left
	     * child of the new node.  Node p's right child becomes the middle
	     * child of the new node.  The item being inserted becomes the
	     * middle child of node p.
	     */
	    new_node = malloc(sizeof(tree23_node_t));
	    new_node->key_node1 = new_node->key_node2 = NULL;
	    new_node->left.item = p->middle.item;
	    new_node->middle.item = p->right.item;
	    new_node->right.item = NULL;
	    p->middle.item = item;
	    p->right.item = NULL;

	    /* Insertion for new_node will continue higher up in the tree. */
	}
	else {  /* Node p has two children. */
	    
            /* The middle child of node p becomes node p's right child, and
	     * the item being inserted becomes the middle child.
	     */
	    p->right.item = p->middle.item;
	    p->middle.item = item;

	    /* No need to insert higher up. */
	    return_item = NULL;  /* Insertion successful. */
	    t->n++;
	    return return_item;
	}
    }

    return_item = NULL;  /* Insertion successful. */
    t->n++;

    /* x points to the node being inserted into the tree.  x_min keeps track of
     * the parent node for the minimum item in the subtree rooted at node x.
     */
    x_min = x = new_node;

    /* Insertion of new nodes can keep propagating up one level in the tree.
     * This stops when an insertion does not result in a new node at the next
     * level up, or the root level is reached.
     */
    while((parent = p->parent)) {

	/* Determine the insertion position of x under p. */
	if(parent->right.node == p) {  /* Insert beside the right branch. */
	
	    /* Create a new node.  parent's right child becomes the left child
	     * of the new node.  Node x is the middle child of the new node.
	     */
	    new_node = malloc(sizeof(tree23_node_t));
	    new_node->left.node = parent->right.node;
	    parent->right.node->parent = new_node;
	    new_node->middle.node = x;  x->parent = new_node;
	    new_node->key_node1 = x_min;
	    new_node->right.node = NULL;	
	    new_node->key_node2 = NULL;
	    x_min = parent->key_node2;
	    parent->right.node = NULL;
	    parent->key_node2 = NULL;

	    /* Insertion for new_node will continue higher up in the tree. */
	}
	else if(parent->middle.node == p) {
            /* Insert beside the middle branch. */

	    /* Insertion depends on the number of children node p currently
	     * has.
	     */
	    if(parent->key_node2) {  /* parent has three children. */
		
		/* Create a new node.  Node x is the left child of the new
		 * node.  parent's right child becomes the middle child of the
		 * new node.
		 */
		new_node = malloc(sizeof(tree23_node_t));
		new_node->left.node = x;  x->parent = new_node;
                    /* x_min does not change. */
		new_node->middle.node = parent->right.node;
		parent->right.node->parent = new_node;
		new_node->key_node1 = parent->key_node2;
		new_node->right.node = NULL;
		new_node->key_node2 = NULL;
		parent->right.node = NULL;
		parent->key_node2 = NULL;
		
	        /* Insertion for new_node will continue higher up in the tree.
		 */
	    }
	    else {  /* parent has two children. */
		
		/* Node x is inserted as the right child of parent. */
		parent->right.node = x;  x->parent = parent;
		parent->key_node2 = x_min;
		
	        /* No need to insert higher up. */
	        return return_item;
	    }
	}
	else {  /* Insert beside the left branch. */

            /* Insertion depends on the number of children parent currently
	     * has.
	     */
	    if(parent->key_node2) {  /* parent has three children. */
		
		/* Create a new node.  parent's middle child becomes the left
		 * child of the new node.  parent's right child becomes the
		 * middle child of the new node.  Node x becomes the middle
		 * child of parent.
		 */
		new_node = malloc(sizeof(tree23_node_t));
		new_node->left.node = parent->middle.node;
		parent->middle.node->parent = new_node;
		new_node->middle.node = parent->right.node;
		parent->right.node->parent = new_node;
		new_node->key_node1 = parent->key_node2;
		new_node->right.node = NULL;
		new_node->key_node2 = NULL;
		parent->middle.node = x;  x->parent = parent;
		temp_node = parent->key_node1;
		parent->key_node1 = x_min;
		x_min = temp_node;
		parent->right.node = NULL;
		parent->key_node2 = NULL;
		
	        /* Insertion for new_node will continue higher up in the tree.
		 */
	    }
	    else {  /* parent has two children. */
		
		/* The middle child of parent becomes parent's right child, and
		 * node x becomes the middle child.
		 */
		parent->right.node = parent->middle.node;
		parent->key_node2 = parent->key_node1;
		parent->middle.node = x;  x->parent = parent;
		parent->key_node1 = x_min;
		
	        /* No need to insert higher up. */
	        return return_item;
	    }
	}

	x = new_node;
        p = parent;
    }
    
    /* This point is only reached if the root node was split.  A new root node
     * will be created, with the child nodes pointed to by p (old root node)
     * and x (inserted node).
     */
    new_node = malloc(sizeof(tree23_node_t));
    new_node->left.node = p;  p->parent = new_node;
    new_node->middle.node = x;  x->parent = new_node;
    new_node->key_node1 = x_min;
    new_node->right.node = NULL;
    new_node->key_node2 = NULL;
    new_node->parent = NULL;
    t->root = new_node;
    t->height++;


    return return_item;
}



/* tree23_find() - Find an item in the 2-3 tree with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *tree23_find(tree23_t *t, void *key_item)
{
    int (* compar)(const void *, const void *);
    int cmp_result;
    tree23_node_t *p;
    
    p = t->root;
    compar = t->compar;
    
    /* First check for the special cases where the tree contains contains only
     * zero or one nodes.
     */
    if(t->n <= 1) {
        if(t->n && (compar(key_item, p->left.item) == 0)) return p->left.item;
      	/* else */
	
	return NULL;
    }

    /* Search the tree to locate the item with key key_item. */
    while(p->key_node1) {
	if(p->key_node2 && compar(key_item, p->key_node2->left.item) >= 0) {
	    p = p->right.node;
	}
	else if(compar(key_item, p->key_node1->left.node) >= 0) {
	    p = p->middle.node;
	}
	else {
	    p = p->left.node;
	}
    }
    
    /* Find a leaf item of node p. */
    if(p->right.item && (cmp_result = compar(key_item, p->right.item)) >= 0) {
	/* Item may be right child. */

       	if(cmp_result) {
	    /* Find failed - matching item does not exist in the tree. */
	    return NULL;
	}
	/* else */
	return p->right.item;  /* Item found. */
    }
    else if((cmp_result = compar(key_item, p->middle.item)) >= 0) {
	/* Item may be middle child. */

	if(cmp_result) {
	    /* Find failed - Matching item does not exist in the tree. */
	    return NULL;
	}
	/* else */
	return p->middle.item;  /* Item found. */
    }
    else {
	/* Item may be left child. */

	if(compar(key_item, p->left.item)) {
	    /* Find failed - matching item does not exist in the tree. */
	    return NULL;
	}
	/* else */
        return p->left.item;  /* item found. */
    }

}



/* tree23_find_min() - Returns a pointer to the minimum item in the 2-3 tree
 * pointed to by t.  If there are no items in the tree a NULL pointer is
 * returned.
 */
void *tree23_find_min(tree23_t *t)
{
    return t->min_node->left.item;
}



/* tree23_delete() - Delete the item in the 2-3 tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the deleted item,
 * and NULL if no item was found.
 */
void *tree23_delete(tree23_t *t, void *key_item)
{
    int (* compar)(const void *, const void *);
    int cmp_result;
    void *return_item, *merge_item;
    tree23_node_t *p, *q, *parent, *merge_node, *lchild;

    
    p = t->root;
    compar = t->compar;
    
    /* Special cases: 0, 1, or 2 items in the tree. */
    if(t->n <= 2) {
	if(t->n <= 1) {
 	    if(t->n == 0) {
                return NULL;   /* Tree empty.  Delete failed. */
            }
    	    /* else: one item in the tree... */

	    /* Check if the item is the left child. */
	    if(compar(key_item, p->left.item) == 0) {
	         return_item = p->left.item;  /* Item found. */
	         p->left.item = NULL;
		 t->n--;
		 return return_item;
	    }
	    /* else */
	    
	    return NULL;  /* Item not found. */
	}
        /* else: two items in the tree... */

	/* Check if the item may be the middle child. */
        if((cmp_result = compar(key_item, p->middle.item)) >= 0) {

	    /* check if the item is the middle child. */
	    if(cmp_result == 0) {
		return_item = p->middle.item;  /* Item found. */
		p->middle.item = NULL;
		t->n--;
		return return_item;
            }
	    /* else */
	    
	    return NULL;  /* Item not found. */
	}

        /* Check if the item is the left child. */
	if(compar(key_item, p->left.item) == 0) {
             return_item = p->left.item;  /* Item found. */
             p->left.item = p->middle.item;
	     p->middle.item = NULL;
	     t->n--;
	     return return_item;
	}
	/* else */

        return NULL;  /* Item not found. */
    }
    
    /* Search the tree to locate the node pointing to the leaf item to be
     * deleted from the 2-3 tree.
     */
    while(p->key_node1) {
	if(p->key_node2 && compar(key_item, p->key_node2->left.item) >= 0) {
	    p = p->right.node;
	}
	else if(compar(key_item, p->key_node1->left.item) >= 0) {
	    p = p->middle.node;
	}
	else {
	    p = p->left.node;
	}
    }

    
    /* Delete the appropriate leaf item of node p. */
    if(p->right.item && (cmp_result = compar(key_item, p->right.item)) >= 0) {
	/* Item may be right child. */

	/* Check whether the item to be deleted was found. */
       	if(cmp_result) {
	    /* Item not found. */
	    return_item = NULL;
	}
	else {
	    /* Item found. */
	    return_item = p->right.item;
	    t->n--;
	    p->right.item = NULL;
	}

	/* No need for merge since node p still has two items. */
	return return_item;
    }
    else if((cmp_result = compar(key_item, p->middle.item)) >= 0) {
	/* Item may be middle child. */

	/* Check whether the item to be deleted was found. */
	if(cmp_result) {
	    /* Item not found. */
	    return_item = NULL;
	    return return_item;
	}
	/* else */
	
	return_item = p->middle.item;  /* Item found. */
	t->n--;

  	/* If node p has three children, two are left after the delete, and no
	 * further rearrangement is needed.
	 */
	if(p->right.item) {
	    p->middle.item = p->right.item;
	    p->right.item = NULL;
     	    return return_item;
	}
        /* else */
	
	/* Node p has only its left child remaining.  The remaining child will
	 * be merged with a child from a sibling of node p.
	 */
	merge_item = p->left.item;
    }
    else {
	/* Item may be left child. */

	if(compar(key_item, p->left.item)) {
	    /* Delete failed - matching item does not exist in the tree. */
	    return_item = NULL;
	    return return_item;
	}

        return_item = p->left.item;  /* item found. */
	t->n--;

  	/* If node p has three children, two are left after the delete, and no
	 * further rearrangement is needed.
	 */
	if(p->right.item) {
            p->left.item = p->middle.item;
	    p->middle.item = p->right.item;
	    p->right.item = NULL;
	    return return_item;
	}
	/* else */

	/* Node p has only its middle child remaining.  The remaining child
	 * will be merged with a child from a sibling of node p.
	 */
	merge_item = p->middle.item;
    }


    /* If the function has not exited by this point, then the remaining child
     * item of node p is to be merged with a child from a sibling of node p.
     * Node p is not the root node, since this falls under the special case
     * delete (handled at the start of this function).
     */
    
    parent = p->parent;  /* Node p's parent. */

    /* The following code performs the leaf level merging of merge_item.  Note
     * that unless node p was a left child, it always has a sibling to its
     * left.
     */
    if(parent->right.node == p) {
	/* p was the right child. */
        q = parent->middle.node;  /* Sibling to the left of node p. */

	/* Merging depends on how many children node q currently has. */
	if(q->right.item) {  /* Node q has three children. */
	    
	    /* Keep node p by assigning it the right child of q as the sibling
	     * of merge_item.
	     */
	    p->left.item = q->right.item;
	    p->middle.item = merge_item;
	    q->right.item = NULL;

	    return return_item;  /* Node p is not deleted. */
	}
	else {  /* Node q has two children. */
	    
            /* Make merge_item a child of node q, and delete p. */
	    q->right.item = merge_item;
	    parent->key_node2 = parent->right.node = NULL;
	    free(p);

	    return return_item;  /* The parent still has two children. */
	}
    }
    else if(parent->middle.node == p) {
	/* p was the middle child. */
        q = parent->left.node;  /* Sibling to the left of node p. */

        /* Merging depends on how many children node q currently has. */
        if(q->right.item) {  /* Node q has three children. */
	    
	    /* Keep node p by assigning it the right child of q as the sibling
	     * of merge_item.
	     */
	    p->left.item = q->right.item;
	    p->middle.item = merge_item;
	    q->right.item = NULL;
	    
	    return return_item;  /* Node p is not deleted. */
	}
	else {  /* Node q has two children. */
	    
            /* Make merge_item a child of node q, and delete p. */
	    q->right.item = merge_item;
	    free(p);

  	    /* If the parent of p and q had three children, then two will be
	     * left after the merge, and merging will not be needed at the next
	     * level up.
	     */
	    if(parent->key_node2) {
		parent->key_node1 = parent->middle.node = parent->key_node2;
		parent->key_node2 = parent->right.node = NULL;
		return return_item;
	    }
	    /* else */

	    /* The parent only has child q remaining.  The remaining child
	     * will be merged with a child from a sibling of the parent.
	     */
	    merge_node = q;
	    p = parent;
	}
    }
    else {
	/* p was the left child. */
	q = parent->middle.node;  /* Sibling to the right of node p. */

        /* Merging depends on how many children node q currently has. */
        if(q->right.item) {  /* Node q has three children. */
	    
	    /* Keep node p by assigning it the left child of q as the sibling
	     * of merge_item.
	     */
	    p->left.item = merge_item;
	    p->middle.item = q->left.item;
	    q->left.item = q->middle.item;
	    q->middle.item = q->right.item;
	    q->right.item = NULL;
	    
	    return return_item;
	}
	else {  /* Node q has two children. */
	    
            /* We could make merge_item a child of node q, then delete node p.
	     * However, since node p is pointed to by a key_node pointer, we
	     * instead give merge_item and node q's children to p, then delete
	     * node q.  This ensures that the key_node pointer from an
	     * ancestors of p remains correct.
	     */
	    p->right.item = q->middle.item;
	    p->middle.item = q->left.item;
	    p->left.item = merge_item;
	    free(q);
	    
  	    /* If the parent of p and q had three children, then two will be
	     * left after the merge, and merging will not be needed at the next
	     * level up.
	     */
	    if(parent->key_node2) {
		parent->key_node1 = parent->middle.node = parent->key_node2;
		parent->key_node2 = parent->right.node = NULL;
		return return_item;
	    }
	    /* else */

	    /* The parent only has child q remaining.  The remaining child
	     * will be merged with a child from a sibling of the parent.
	     */
	    merge_node = p;
	    p = parent;
	}
    }
    
    /* Merging of nodes can keep propagating up one level in the tree.  This
     * stops when the result of a merge does not require a merge at the next
     * level up, or the root level is reached.
     */
    while((parent = p->parent)) {
	/* The following code merges node p's single child, merge_node, with
	 * a children from node p's sibling, q.
	 */

        /* Merging depends on which child p is. */
	if(parent->right.node == p) {
	    /* p was the right child. */
	    q = parent->middle.node;  /* Sibling to the left of node p. */

            /* Merging depends on how many children node q currently has. */
	    if(q->key_node2) {  /* Node q has three children. */
		
		/* Keep node p by assigning it the right child of q as the
		 * sibling of merge_node.
		 */
		lchild = q->right.node;
		p->left.node = lchild;  lchild->parent = p;
		p->middle.node = merge_node;  merge_node->parent = p;
		p->key_node1 = parent->key_node2;  /* merge_min */
		parent->key_node2 = q->key_node2;
		q->right.node = NULL;
                q->key_node2 = NULL;
		
		return return_item;  /* Node p is not deleted. */
	    }
	    else {  /* Node q has two children. */
		
		/* Make merge_node a child of node q, and delete p. */
		q->right.node = merge_node;  merge_node->parent = q;
		q->key_node2 = parent->key_node2;  /* merge_min */
		parent->right.node = NULL;
		parent->key_node2 = NULL;
		free(p);

		return return_item;  /* The parent still has two children. */
	    }
	}
	else if(parent->middle.node == p) {
	    /* p was the middle child. */
	    q = parent->left.node;  /* Sibling to the left of node p. */
	    
            /* Merging depends on how many children node q currently has. */
	    if(q->key_node2) {  /* Node q has three children. */
		
		/* Keep node p by assigning it the right child of q as the
		 * sibling of merge_node.
		 */
		lchild = q->right.node;
		p->left.node = lchild;  lchild->parent = p;
		p->middle.node = merge_node;  merge_node->parent = p;
		p->key_node1 = parent->key_node1;  /* merge_min */
		parent->key_node1 = q->key_node2;
		q->right.node = NULL;
	        q->key_node2 = NULL;
		
		return return_item;  /* p is not deleted. */
	    }
	    else {  /* Node q has two children. */
		
		/* Make merge_node a child of node q, and delete p. */
		q->right.node = merge_node;  merge_node->parent = q;
		q->key_node2 = parent->key_node1;  /* merge_min */
		free(p);
		
  	        /* If the parent of p and q had three children, then two will
	         * be left after the merge, and merging will not be needed at
	         * the next level up.
	         */
		if(parent->key_node2) {
		    parent->middle.node = parent->right.node;
		    parent->key_node1 = parent->key_node2;
		    parent->right.node = NULL;
		    parent->key_node2 = NULL;
		    return return_item;
		}
		/* else */

		/* The parent only has child q remaining.  The remaining child
	         * will be merged with a child from a sibling of the parent.
	         */
		merge_node = q;
		p = parent;
	    }
	}
	else {
	    /* p was the left child. */
	    q = parent->middle.node;  /* Sibling to the right of node p. */

            /* Merging depends on how many children node q currently has. */
	    if(q->key_node2) {  /* Node q has three children. */
		
		/* Keep node p by assigning it the left child of q as the
		 * sibling of merge_node.
	         */
		p->left.node = merge_node;  merge_node->parent = p;
		p->middle.node = q->left.node;  q->left.node->parent = p;
		p->key_node1 = parent->key_node1;
		    /* Equals minimum node in tree(q->left.node). */
		q->left.node = q->middle.node;
		parent->key_node1 = q->key_node1;
		q->middle.node = q->right.node;
		q->key_node1 = q->key_node2;
		q->right.node = NULL;
	        q->key_node2 = NULL;
		return return_item;
	    }
	    else {  /* Node q has two children. */
		
		/* Make merge_node a child of node p, and move q's children to
		 * p, deleting node q.  (Keeping the merge process consisitient
		 * with leaf level merges.)
		 */
		p->right.node = q->middle.node;  q->middle.node->parent = p;
		p->key_node2 = q->key_node1;
		p->middle.node = q->left.node;  q->left.node->parent = p;
		p->key_node1 = parent->key_node1;
		    /* Equals minimum node in tree(q->left.node). */
		p->left.node = merge_node;  merge_node->parent = p;
		free(q);
	    
  	        /* If the parent of p and q had three children, then two will
	         * be left after the merge, and merging will not be needed at
		 * the next level up.
	         */
		if(parent->key_node2) {
		    parent->middle.node = parent->right.node;
		    parent->key_node1 = parent->key_node2;
		    parent->right.node = NULL;
		    parent->key_node2 = NULL;
		    return return_item;
		}
		/* else */

		/* The parent only has child q remaining.  The remaining child
	         * will be merged with a child from a sibling of the parent.
	         */
		merge_node = p;
		p = parent;
	    }
	}
    }


    /* Remove the old root node, p, making node merge_node the new root node.
     */
    free(p);
    t->root = merge_node;  merge_node->parent = NULL;
    t->height--;
    

    return return_item;
}



/* tree23_delete_min() - Deletes the item with the smallest key from the
 * binary search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *tree23_delete_min(tree23_t *t)
{
    void *return_item, *merge_item;
    tree23_node_t *p, *q, *r, *parent, *merge_node;

    p = t->root;
    
    /* Special cases: 0, 1, or 2 items in the tree. */
    if(t->n <= 2) {
	if(t->n <= 1) {
 	    if(t->n == 0) {
                return NULL;   /* Tree empty.  Delete failed. */
            }
    	    /* else: one item in the tree... */
	    
	    return_item = p->left.item;  /* Item found. */
	    p->left.item = NULL;
	}
	else {  /* Two items in the tree. */

            return_item = p->left.item;  /* Item found. */
            p->left.item = p->middle.item;
	    p->middle.item = NULL;
	}
	t->n--;
	return return_item;
    }
    /* else */

    /* Obtain a pointer to the parent of the minimum data item in the tree.
     */
    p = t->min_node;
    return_item = p->left.item;  /* Item found. */
    t->n--;

    /* If node p has three children, two are left after the delete, and no
     * further rearrangement is needed.
     */
    if(p->right.item) {
	p->left.item = p->middle.item;
	p->middle.item = p->right.item;
	p->right.item = NULL;
	return return_item;
    }
    /* else */

    /* Node p has only its middle child remaining.  The remaining child
     * will be merged with a child from a sibling of node p.
     */
    merge_item = p->middle.item;

    /* If the function has not exited by this point, then the remaining child
     * item of node p is to be merged with a child from a sibling of node p.
     * Node p is not the root node, since this falls under the special case
     * delete (handled at the start of this function).
     */
    
    parent = p->parent;  /* Node p's parent. */

    /* p was the left child. */
    q = parent->middle.node;  /* Sibling to the right of node p. */
    
    /* Merging depends on how many children node q currently has. */
    if(q->right.item) {  /* Node q has three children. */
	/* Keep node p by assigning it the left child of q as the sibling
	 * of merge_item.
	 */
	p->left.item = merge_item;
	p->middle.item = q->left.item;
	q->left.item = q->middle.item;
	q->middle.item = q->right.item;
	q->right.item = NULL;
	    
	return return_item;
    }
    else {  /* node q has two children... */
    
	/* We could make merge_item a child of node q, then delete node p.
         * However, since node p is pointed to by the trees min_node pointer,
	 * we instead give merge_item and node q's children to p, then delete
	 * node q.  This way, as with insertion, the trees min_node pointer
	 * never changes.
	 */
	p->right.item = q->middle.item;
	p->middle.item = q->left.item;
	p->left.item = merge_item;
	free(q);

	/* If the parent of p and q had three children, then two will be left
	 * after the merge, and merging will not be needed at the next level
	 * up.
	 */
	if(parent->key_node2) {
	    parent->middle.node = parent->right.node;
	    parent->key_node1 = parent->key_node2;
	    parent->right.node = NULL;
	    parent->key_node2 = NULL;

	    merge_node = p;
	    p = parent;

	    return return_item;
	}
        /* else */
    
        /* The parent node only has child p remaining.  To correct this, a
	 * merge must be performed.
         */
        merge_node = p;
        p = parent;
    }
    
    /* If this point is reached, a final merge step must be performed.  In
     * order to have O(1) worst case time complexity for delete_min, this final
     * merge must not result in further merges being necessary.
     */

    /* The final merge step is only needed if the root level has not been
     * reached yet.
     */
    if(p->parent) {
        parent = p->parent;

        /* p is always a left child, and always has a sibling to its right. */
	q = parent->middle.node;  /* Sibling to the right of node p. */

        /* For the final merge, we want the tree rooted at merge_node to be
	 * merged with child trees of the same height.  The first of these
	 * child trees is pointed to by parent->key_node1, and has the parent:
	 * r = parent->key_node1->parent.  Thus, the children of nodes p and r
	 * are to be merged.
	 */
	r = parent->key_node1->parent;

	/* Merging depends on how many children node r currently has. */
	if(r->key_node2) {  /* Node r has three children. */
	    
	    /* Keep node p by assigning it the left child of r as the
	     * sibling of merge_item.  Note that parent->key_node1 points to
	     * r's left child, so will be updated.
	     */
	    p->left.node = merge_node;  merge_node->parent = p;
	    p->key_node1 = p->middle.node = r->left.node;
	    r->left.node->parent = p;
 
	    parent->key_node1 = r->left.node = r->middle.node;
	    r->key_node1 = r->middle.node = r->right.node;
	    r->key_node2 = r->right.node = NULL;
	    return return_item;
	}
        else {  /* Node r has two children. */
	
	    /* Make merge_item a child of node r, and delete p. */
	    r->key_node2 = r->right.node = r->middle.node;
	    r->key_node1 = r->middle.node = r->left.node;
	    r->left.node = merge_node;  merge_node->parent = r;
	    free(p);
	    
	    /* If the parent of p and q had three children, then two will
	     * be left after the merge, so merging can stop.
	     */
	    if(parent->key_node2) {
		parent->left.node = q;
		parent->middle.node = parent->right.node;
		parent->key_node1 = parent->key_node2;
		parent->right.node = NULL;
		parent->key_node2 = NULL;
		return return_item;
	    }
	    /* else */

	    /* The parent only has child q remaining.  Merging will be
	     * terminated as though the root level of the heap were reached
	     * (see the code below).
	     */
	    p = parent;
	    merge_node = q;
	}
    }

    /* Normally, merging would have continued until the root level of the tree
     * was reached.  If the root level has not been reached yet, we terminate
     * the merging by deleting node p and replacing it with its child,
     * merge_node.
     */
    free(p);
    if(p->parent) {
	/* Stop merging. */
        parent = p->parent;
	parent->left.node = merge_node;  merge_node->parent = parent;
    }
    else {    
        /* Remove the old root node, p, making node x the new root node. */
        t->root = merge_node;  merge_node->parent = NULL;
        t->height--;
    }
    
    /* NOTE:  In a normal 2-3 tree path lengths to leaves from the root are all
     * equal.  When merging is terminated before reaching the root level, the
     * lengths of paths to leaf nodes under p shrink by one.  This O(1)
     * delete_min implementation allows path lengths to leaves to vary at left
     * branches, while still keeping the tree height bounded by O(log n), so
     * that the time for insert, delete, and find is still O(log n) worst case.
     */

    return return_item;
}
