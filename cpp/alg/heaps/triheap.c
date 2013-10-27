/*** Trinomial Heap Implementation ***/
/*
 *   Shane Saunders
 */

/* This version is implemented using the node-pair pointer structure; that is,
 * nodes have a partner pointer, so that nodes can be paired.  In this
 * implementation, the child pointer points to the highest dimension child
 * node.
 */

#include "triheap.h"
#include <stdlib.h>
#include <math.h>
#if SHOW_trih
#include <stdio.h>
#endif


#define TRUE 1
#define FALSE 0



/*** Prototypes of functions only visible within this file. ***/

void trih_meld(triheap_t *h, triheap_node_t *tree_list);
int trih_merge(triheap_node_t **a, triheap_node_t **b);
void trih_add_child(triheap_node_t *c, triheap_node_t *p);
void trih_replace_child(triheap_node_t *old, triheap_node_t *new);



/*** Definitions of functions visible outside of this file. ***/


/* trih_alloc() - creates and returns a pointer to a trinomial heap.  Argument
 * max_nodes specifies the maximum number of nodes the heap can contain.
 */
triheap_t *trih_alloc(int max_nodes)
{
    triheap_t *h;
#if SHOW_trih
printf("init, ");  fflush(stdout);
#endif
 
    /* Create the heap. */
    h = malloc(sizeof(triheap_t));

    /* The maximum number of nodes and the maximum number of trees allowed. */
    h->max_nodes = max_nodes;
    h->max_trees = 1 + log(max_nodes)/log(3.0);

    /* Allocate space for an array of pointers to trees, and nodes in the heap.
     * calloc() initialises all array entries to zero, that is, NULL pointers.
     */
    h->trees = calloc(h->max_trees, sizeof(triheap_node_t *));
    h->active = calloc(h->max_trees, sizeof(triheap_node_t *));
    h->nodes = calloc(max_nodes, sizeof(triheap_node_t *));

    /* We begin with no nodes in the heap. */
    h->n = 0;

    /* The value of the heap helps to keep track of the maximum dimension while
     * nodes are inserted or deleted.
     */
    h->value = 0;

    /* For experimental purposes, we keep a count of the number of key
     * comparisons.
     */
    h->key_comps = 0;

    return h;
}


/* trih_free() - destroys the heap pointed to by h, freeing up any space that
 * was used by it.
 */
void trih_free(triheap_t *h)
{
    int i;

#if SHOW_trih
printf("free, ");  fflush(stdout);
#endif

    for(i = 0; i < h->max_nodes; i++) {
        free(h->nodes[i]);
    }

    free(h->nodes);
    free(h->trees);
    free(h->active);
    free(h);

#if SHOW_trih
printf("free-exited, ");  fflush(stdout);
#endif
}


/* trih_insert() - creates and inserts new a node representing vertex_no with
 * key k into the heap pointed to by h.
 */
void trih_insert(triheap_t *h, int vertex_no, long k)
{
    triheap_node_t *new;

#if SHOW_trih
printf("insert, ");  fflush(stdout);
#endif

    /* Create an initialise the new node.  The parent pointer will be set to
     * NULL by trih_meld().
     */
    new = malloc(sizeof(triheap_node_t));
    new->child = NULL;
    new->extra = FALSE;
    new->left = new->right = NULL;
    new->partner = NULL;

    new->dim = 0;
    new->vertex_no = vertex_no;
    new->key = k;

    /* Maintain a pointer to vertex_no's new node in the heap. */
    h->nodes[vertex_no] = new;

    /* Meld the new node into the heap. */
    trih_meld(h, new);

    /* Update the heap's node count. */
    h->n++;

#if SHOW_trih
printf("insert-exited, ");  fflush(stdout);
#endif
}


/* trih_delete_min() - deletes the minimum node from the heap pointed to by h
 * and returns its vertex number.
 */
int trih_delete_min(triheap_t *h)
{
    triheap_node_t *min_node, *child, *next, *partner;
    triheap_node_t *head, *tail, *break_node, *first_child;
    triheap_node_t *l, *parent, *child_zero, *child_higher;
    triheap_node_t *ptr, *next_partner, *next_parent, *next_first_child;
    triheap_node_t *next_child_zero, *next_child_higher;
    long k, k2;
    int d, next_dim, v, vertex_no;
    int was_extra;

#if SHOW_trih
printf("delete_min, ");  fflush(stdout);
#endif

    /* First we determine the maximum dimension tree in the heap. */
    v = h->value;
    d = -1;
    while(v) {
        v = v >> 1;
        d++;
    };

    /* Now locate the root node with the smallest key, scanning from the
     * maximum dimension root position, down to dimension 0 root position.
     * At the same time, scan active nodes.  Note that the maximum dimension of
     * a active node is one less than the maximum dimension main trunk since we
     * never get active nodes on a main trunk
     */
    min_node = h->trees[d];
    k = min_node->key;
    while(d > 0) {
        d--;
        next = h->trees[d];
        if(next) {
            if((k2 = next->key) < k) {
                k = k2;
                min_node = next;
            }
            h->key_comps++;
        }
        next = h->active[d];
        if(next) {
            if((k2 = next->key) < k) {
                k = k2;
                min_node = next;
            }
            h->key_comps++;
        }
    }

#if SHOW_trih
printf("on vertex no %d, ", min_node->vertex_no);  fflush(stdout);
#endif

    /* The resulting break-up depends on whether or not min_node is a root node
     * or an active node.
     */
    
    /* An active node may have been destroyed. */
    if(min_node->parent) {
	h->active[min_node->dim] = NULL;
    }
    
    /* During break_up break_node points to the node that `appears' to have
     * been removed.  Initially this is min_node.
     */
    break_node = min_node;
    d = break_node->dim;
    partner = break_node->partner;
    first_child = break_node->extra ? partner : break_node;
    parent = first_child->parent;  /* parent pointer of the broken node. */
    
    /* A nodes child pointer always points to its highest dimension child,
     * so child->right is the smallest dimension.  For melding the linked list
     * starting at child->right, we terminate the circular link with a NULL
     * pointer.
     *
     * For the linked list, only the right pointer is used, so the value of the
     * left pointer will be left undefined.
     */
    child = min_node->child;
    if(child) {
        head = child->right;
	ptr = tail = child;

	/* Nodes in this break up may go from active to inactive. */
	do {
	    ptr = ptr->right;
	    if(h->active[ptr->dim] == ptr) h->active[ptr->dim] = NULL;
	} while(ptr != tail);
	
	if(parent) {
	    tail->right = parent;
	    tail = parent;
	}
    }
    else {
        head = tail = parent;
    }

    
    /* Propagate break-up through the tree until the main trunk is reached.
     * Nodes on the main trunk are treated like first child and second child
     * with no parent.  Active nodes never occur as a second child.
     */
    if(parent) {
    child_zero = parent->child->right;
    child_higher = first_child->right;
    while(1) {
	/* At the start of the loop links are not yet updated to account for
	 * the node that appears to have been removed.
	 * parent - points to the parent of the last broken node.
	 * partner - points to the partner of the last broken node.
	 * first_child - is the first child on the trunk of the broken node.
	 * The smaller of parent and partner is pointed to by the linked list,
	 * Since the linked list is updated in advance.
	 */

	
	/* Make the partner of break_node `parent's partner.  Remember the
	 * old value of the parents partner pointer for the next dimension.
	 * Also remember the value of parent->dim.
	 */
	next_dim = parent->dim;
	parent->dim = d;
	next_partner = parent->partner;
        parent->partner = partner;
        partner->partner = parent;

       	/* For the last node pair added to the linked list
	 * (i.e. (parent,partner) or (partner,parent)), we ensure that the
	 * correct node is labelled as extra.
	 */
	was_extra = parent->extra;
	tail->extra = FALSE;
	tail->partner->extra = TRUE;

	/* Obtain future values of pointer variables now.  This is done because
	 * constructing the linked list overwrites pointers that are followed.
	 */
	if(was_extra) {
	    next_first_child = next_partner;
	}
	else {
            next_first_child = parent;
	}
        next_parent = next_first_child->parent;
	if(next_parent) {
	    next_child_zero = next_parent->child->right;
	    next_child_higher = next_first_child->right;
	}

	
        /* Add the child pairs of `parent' that have a greater dimension than
	 * first_child to the list of broken trunks.  Keep a pointer to
	 * parent->child->right for use below.
	 */
	if(parent->child != first_child) {
	    ptr = tail;
	    tail->right = child_higher;
	    tail = parent->child;

	    /* Nodes in this break up may go from active to inactive.
	     */
	    do {
		ptr = ptr->right;
		if(h->active[ptr->dim] == ptr) h->active[ptr->dim] = NULL;
	    } while(ptr != tail);
	}

	
	/* Update the list of children of `parent' to only include those of
	 * lower dimension than first_child.
	 */
	if(d) {
	    /* Lower dimension children exist.  Note that tail currently points
	     * to the highest dimension child.
	     */
	    l = first_child->left;
	    l->right = child_zero;
	    child_zero->left = l;
	    parent->child = l;
	}
	else {
	    /* No lower dimension children. */
	    parent->child = NULL;
	}

	
        /* Now continue break up at 1 dimension higher up by treating `parent'
	 * as the node that has been broken.
	 * Note that on ending, next_parent will be NULL, and we only require
	 * `partner' and d to be updated before exiting.
	 */
	partner = next_partner;
	d = next_dim;
	if(!next_parent) break;
        break_node = parent;
	parent = next_parent;
	first_child = next_first_child;
	

	/* Pointers to the dimension zero child of parent, and the next highest
	 * dimension child from first_child.
	 */
	child_zero = next_child_zero; 
	child_higher = next_child_higher;

	
	/* Update the linked list in advance to point to the next break_node,
	 * usually `parent', but possibly `partner' if `partner' is active.
	 */
	if(was_extra) {
	    
            /* Since break_node is not active, `partner' may be, so we may
	     * need to swap the order of `partner' and `parent' in the trunk
	     * resulting from break-up.
	     */
	    if(h->active[d] == partner) {
		h->active[d] = NULL;
		if(partner->key < parent->key) {
                    /* We make the linked list point to `partner' instead of
		     * `parent', and make parent an extra node.
		     */
		    tail->right = partner;
		    tail = partner;
		    continue;  /* Back to start of loop. */
		}
                h->key_comps++;
	    }
	}
	else {
            if(h->active[d] == break_node) h->active[d] = NULL;
	}
	tail->right = parent;
	tail = parent;

    }} /* if-while */

    
    /* Break up always propagates up to the main trunk level.  After break up
     * the length the main trunk decreases by one.  The current tree position
     * will become empty unless break_node has a partner node.
     */
    if(partner) {
	partner->partner = NULL;

	if(partner->extra) {
	    partner->extra = FALSE;
	    partner->parent = NULL;
	    partner->left = partner->right = partner;
	    h->trees[d] = partner;
	}
    }
    else {
	h->trees[d] = NULL;
	h->value -= (1 << d);
    }
    h->n--;

    /* Meld the linked list of trunks resulting from break-up into the main
     * trunk level of the heap.
     */
    if(head) {
	tail->right = NULL;
	trih_meld(h, head);
    }
    
    /* Record the vertex no to return. */
    vertex_no = min_node->vertex_no;

    /* Delete the old minimum node. */
    h->nodes[vertex_no] = NULL;
    free(min_node);

#if SHOW_trih
trih_dump(h);
printf("delete_min-exited, ");  fflush(stdout);
#endif

    return vertex_no;
}


/* trih_decrease_key() - For the heap pointed to by h, this function decreases
 * the key of the node corresponding to vertex_no to new_value.   No check is
 * made to ensure that new_value is in-fact less than or equal to the current
 * value, so it is up to the user of this function to ensure that this holds.
 */
void trih_decrease_key(triheap_t *h, int vertex_no, long new_value)
{
    triheap_node_t *v, *v2, *w, *w2, *p, *above, *partner, *active_node;
    triheap_node_t *l, *r, *low_child, *high_child, *ptr;
    int d;

#if SHOW_trih
printf("decrease_key on vn = %d (%ld), ", vertex_no, new_value);  fflush(stdout);
#endif

    
    /* Pointer v points to the decreased node. */
    v = h->nodes[vertex_no];
    v->key = new_value;
    d = v->dim;  /* dimension */

    
    /* This loop allows rearrangement to propagate to higher dimensions. */
    while(1) {
	

	v2 = v->partner;  /* partner */
	if(v->extra) {
	    p = v2->parent;  /* parent */
	    above = v2;
	}
	else {
	    above = p = v->parent; /* parent */
	}

	
	/* Determine if rearrangement is necessary */

	/* If v is a root node, then rearrangement is not necessary. */
	if(!above) return;

        if(p) {
	    /* Non-main trunk. */

            active_node = h->active[d];
	    
	    if(v->extra) {
		/* v is a second child.  If necessary, we swap with the first
		 * child to maintain the correct ordering.
	         */

		h->key_comps++;
		if(v->key < above->key) {
		    /* swap */		    
		    v->extra = FALSE;
		    v2->extra = TRUE;
                    trih_replace_child(v2, v);

                    /* If v2 is inconsistent try promotion.  By checking if
		     * v2 was an active node we can avoid key comparison, since
                     * v2 can only be inconsistent if it is active.
                     */
                    if(active_node == v2) {

                        h->key_comps++;
		        if(v2->key < p->key) goto promote;  /* see below */

                        /* At this point, v2 is active but is consistent, so v
                         * will replace it as the active node, regardless of
                         * whether v is inconsistent or not.
                         */
                        h->active[d] = v;
                        return;
                    }

                    /* If there is some other active node for this dimension.
                     * try rearrangement with it.
                     */
                    if(active_node) goto rearrange;  /* see below */

                    /* Otherwise we can make v an active node, regardless of
                     * whether v is inconsistent or not.
                     */
                    h->active[d] = v;
                    return;
		}

		/* Otherwise don't swap */

                /* We may need to promote, but only if both v and v2 are
                 * inconsistent.  By checking if v2 is already active, we
                 * can avoid key comparison since v2 won't be inconsistent
                 * if it is not active.
                 */
                if(active_node == v2) {
		    /* If v is inconsistent, then v2 is also, so promote. */
                    h->key_comps++;
                    if(v->key < p->key) {
		        /* Swap v and v2. */
                        v2 = v;
                        v = above;
                        goto promote;
                    }

                    /* Otherwise promotion is not necessary. */
                    return;
                }

                /* At this point, promotion is not necessary. */
                return;
            }

            /* Otherwise, v is a first child. */

	    if(active_node) {
		/* If v is already the active node leave it. */
		if(active_node == v) return;

		/* Otherwise some other node is active, so try
		 * rearrangement.
		 */
		goto rearrange;  /* see below */
	    }

            /* At this point, there is no active node, so make v the active
             * node, as it is possibly inconsistent.
             */
            h->active[d] = v;
            return;
        }

	/* Otherwise, v is the second node on a main trunk. */

        h->key_comps++;
	if(v->key < above->key) {
	    /* If v is smaller, we swap it with the first child
	     * (i.e. the root node) to maintain the correct ordering.
	     */
	    v->extra = FALSE;
	    v2->extra = TRUE;
	    v->parent = NULL;
	    v->left = v->right = v;
	    h->trees[d] = v;
            return;
	}

	/* Otherwise, no rearrangement is required since heap order is still
         * satisfied.
         */
        return;

    /*------------------------------*/
    rearrange:

	/* At this stage v is a first child and possibly inconsistent, but not
         * an active node.  Node v2 is consistent.  Rearrangement occurs with
         * the currently active node for this dimension, which at this point
         * must exist.
	 */	


	/* The current active node and its partner are w and w2 respectively.
	 */
	w = active_node;
	w2 = w->partner;

	/* The final rearrangement always pairs v with w and v2 with w2. */
	v->partner = w;    w->partner = v;
	v2->partner = w2;  w2->partner = v2;

	/* Determine the ordering in the rearrangement. */
        h->key_comps++;
	if(v2->key < w2->key) {
	    /* Make the (1st child, 2nd child) pair (v2,w2), replacing (v,v2).
	     */
	    v2->extra = FALSE;
	    trih_replace_child(v, v2);

            h->key_comps++;
	    if(v->key < w->key) {
		/* Make the pair (v,w), replacing (w,w2). */
		w->extra = TRUE;
		trih_replace_child(w,v);

                h->key_comps++;
		if(w->key < v->parent->key) {
		    /* Both v and w are inconsistent, continue with
		     * promotion.  Update v2, and p;
		     */
		    v2 = w;
		    p = v->parent;
                    goto promote;  /* see below */
		}

		/* Otherwise, although w is active, it is not inconsistent.
                 * In this case we do not do promotion.  Instead, v replaces w
                 * as the active node, although v may not be inconsistent.
                 * (Avoids a key comparison to check if v is consistent)
		 */
		h->active[d] = v;
		return;
	    }
	    else {
		/* Make the pair (w,v), replacing (w,w2). */
		v->extra = TRUE;

                h->key_comps++;
		if(v->key < w->parent->key) {
		    /* Both v and w are inconsistent, so continue with
		     * promotion.  Update v, v2, and p.
		     */
		    v2 = v;
		    v = w;
		    p = w->parent;
                    goto promote;  /* see below */
		}

		/* Only w is possibly inconsistent, so it remains the active
                 * node.
                 */
		return;
	    }
	}
	else {
	    /* Make the pair (w2,v2), replacing (w,w2). */
	    w2->extra = FALSE;
	    trih_replace_child(w, w2);

            h->key_comps++;
	    if(v->key < w->key) {
		/* Make the pair (v,w), replacing (v,v2). */
		w->extra = TRUE;

                h->key_comps++;
		if(w->key < v->parent->key) {
		    /* Both v and w are inconsistent, so continue with
                     * promotion.  Update v2.
		     */
		    v2 = w;
                    goto promote;  /* see below */
		}

		/* Only v is possibly inconsistent, so it becomes the active
                 * node.
		 */
		h->active[d] = v;
		return;
	    }
	    else {
		/* Make the pair (w,v), replacing (v,v2). */
		v->extra = TRUE;
		trih_replace_child(v,w);

                h->key_comps++;
                if(v->key < w->parent->key) {
		    /* Both v and w are inconsistent, so continue with
		     * promotion.  Update v, v2.
		     */
		    v2 = v;
		    v = w;
                    goto promote;
                }

		/* Only w is possibly inconsistent, so it remains the
                 * active node.
                 */
       	        return;
	    }
	}

    /*------------------------------*/
    promote:	
        /* Promotion Code.  Reached if the loop has not exited.  Uses variables
	 * p, v, v2, and d.  Must ensure that on the trunk (p,v,v2), both v and
         * v2 are inconsistent nodes, so that (v,v2,p) will give heap
         * ordering.
	 */
	
        /* First we make v2 a child node of v. */
	v2->extra = FALSE;
	trih_add_child(v2, v);
		
	/* Then v replaces p.  Any child nodes of p that have a higher
         * dimension than v will become child nodes of v.  Only child nodes of
	 * lower dimension than v will be left under p.
	 */
	
	v->dim = p->dim;
	partner = v->partner = p->partner;
	high_child = p->child;

	if(d) {
	    /* v has lower dimension siblings. */
            l = p->child = v->left;
	    r = high_child->right;
	    l->right = r;
	    r->left = l;
	}
	else {
	    /* v was an only child. */
	    p->child = NULL;
	}
	
	if(high_child != v) {
	    /* v has higher dimension siblings.  Add them to the list of v's
	     * children, and update their parent pointer.  Note that v
	     * currently has at least one child, since v2 was made a child
	     * of v.
	     */
            low_child = v->right;
            l = v->child;
	    r = v->child->right;
	    l->right = low_child;
	    low_child->left = l;
	    r->left = high_child;
	    high_child->right = r;

	    v->child = high_child;

	    ptr = v;
	    do {
                ptr = ptr->right;
		ptr->parent = v;
	    } while(ptr != high_child);
	}

	/* partner may be NULL if p is a root node, so don't update
	 * partner->partner yet.  See update below.
	 */

	/* If p was an extra node no further pointer updates are required. */

	/* Further pointer updates are only needed if p is a first child or a
	 * root node.
	 */
	if(!p->extra) {
	    if(p->parent) {
		/* p is non-root node and a first child. */
		partner->partner = v;
		trih_replace_child(p, v);
	    }
	    else {
		/* p is a root node, so update the tree pointer. */
		if(partner) partner->partner = v;
		h->trees[p->dim] = v;
		v->left = v->right = v;
		v->parent = NULL;
	    }

	    /* p will become an extra node, see below. */
	    p->extra = TRUE;
	}
	else {
	    /* If p was an extra node then v becomes an extra node. */
	    partner->partner = v;
	    v->extra = TRUE;
	}

	/* Finally, make p the partner of node v2 (i.e. the 2nd child of
	 * node v).
	 */
	p->dim = d;
	v2->partner = p;
	p->partner = v2;

	/* The result of promotion is to release the active node. */
	h->active[d] = NULL;

        d = v->dim;  /* next dimension, moving up */

        /* If p was an active node, it no longer is, and v takes its place
	 * as an active node.
	 */
	if(h->active[d] == p) {
	    h->active[d] = v;
	    return;
	}
	
        /* Rearrangement continues at a higher dimension, since the position of
	 * v has moved up, meaning v may be an inconsistent node.
	 */
    }

#if SHOW_trih
trih_dump(h);
printf("decrease_key-exited, ");  fflush(stdout);
#endif

}



/*** Definitions of functions only visible within this file. ***/

/* trih_meld() - melds  the linked list of trees pointed to by *tree_list into
 * the heap pointed to by h.  This function uses the `right' sibling pointer
 * of nodes to traverse the linked list from lower dimension nodes to higher
 * dimension nodes.  It expects the last nodes `right' pointer to be NULL.
 */
void trih_meld(triheap_t *h, triheap_node_t *tree_list)
{
    triheap_node_t *next, *add_tree;
    triheap_node_t *carry_tree;
    int d;

#if SHOW_trih
printf("meld - ");  fflush(stdout);
#endif

    /* add_tree points to the tree to be merged. */
    add_tree = tree_list;

    carry_tree = NULL;

    do {
        /* add_tree() gets merged into the heap, and also carry_tree if one
         * exists from a previous merge.
         */

        /* Keep a pointer to the next tree and remove sibling and parent links
         * from the current tree.  The dimension of the next tree is always
         * one greater than the dimension of the previous tree, so this merging
         * is like an addition of two ternary numbers.
         *
         * Note that if add_tree is NULL and the loop has not exited, then
         * there is only a carry_tree to be merged, so treat it like add_tree.
         */
        if(add_tree) {
            next = add_tree->right;
            add_tree->right = add_tree->left = add_tree;
            add_tree->parent = NULL;
        }
        else {
            add_tree = carry_tree;
            carry_tree = NULL;
        }

#if SHOW_trih
printf("%d, ", add_tree->vertex_no);  fflush(stdout);
#endif

        /* First we merge add_tree with carry_tree, if there is one.  Note that
         * carry_tree contains only one node in its main trunk, and add_tree
         * has at most two, so the result is at most one 3-node trunk, which is
         * treated as a 1-node main trunk one dimension higher up.
         */
	if(carry_tree) {
            h->key_comps += trih_merge(&add_tree, &carry_tree);
        }

        /* After the merge, if add_tree is NULL, then the resulting tree
         * pointed to by carry_tree carries to higher entry, so we do not need
         * to merge anything into the existing main trunk.
         * If add_tree is not NULL we add it to the existing main trunk.
         */
        if(add_tree) {
            d = add_tree->dim;
	    if(h->trees[d]) {
                /* Nodes already in this main trunk position, so merge. */
                h->key_comps += trih_merge(&h->trees[d], &add_tree);
                if(!h->trees[d]) h->value -= (1 << d);
                carry_tree = add_tree;
            }
            else {
                /* No nodes in this main trunk position, so use add_tree. */
                h->trees[d] = add_tree;
                h->value += (1 << d);
            }
        }

        /* Obtain a pointer to the next tree to add. */
        add_tree = next;

	/* We continue if there is still a node in the list to be merged, or
	 * a carry tree remains to be merged.
	 */
    } while(add_tree || carry_tree);


#if SHOW_trih
printf("meld-exited, ");  fflush(stdout);
#endif

}


/* trih_merge() - merges the two trunks pointed to by *a and *b, returning the
 * sum trunk through `a' and any carry tree through `b'.
 * When this function is used, both parameters `a' and `b' refer to either
 * a 1-node or 2-node trunk.
 *
 * Returns the number of key comparisons used.
 */
int trih_merge(triheap_node_t **a, triheap_node_t **b)
{
    triheap_node_t *tree, *next_tree, *other, *next_other;
    int c;

    /* Number of comparisons. */
    c = 0;

    /* `tree' always points to the node with the lowest key.
     * To begin with, `tree' points to the smaller head node, and `other'
     * points to the head node of the other trunk.
     */
    if((*a)->key <= (*b)->key) {
        tree = (*a);
        other = (*b);
    }
    else {
        tree = (*b);
        other = (*a);
    }
    c++;

    /* next_tree points to the next node on the trunk that `tree' is the head
     * of (if there is another node).
     * next_other points to the next node on the trunk that `other' is the head
     * of (if there is another node).
     */
    next_tree = tree->partner;
    next_other = other->partner;

    /* The merging depends on the existence of nodes and the values of keys. */
    if(!next_tree) {
        /* next_tree does not exist, so we simply make `other' the child of
         * `tree'. If next_other exist the resulting 3-node trunk is a carry
         * tree.
         */

        if(next_other) {
            trih_add_child(other, tree);
	    tree->dim++;
            *a = NULL;  *b = tree;
        }
        else {
	    tree->partner = other;
	    other->partner = tree;
	    other->extra = TRUE;
	    
            *a = tree;  *b = NULL;
        }
    }
    else if(!next_other) {
        /* next_tree exists but next_other does not, so the linked order of
         * next_tree and `other' in the resulting 3-node trunk depends on the
         * values of keys.  The resulting 3-node trunk becomes a carry tree.
         */

	tree->partner = NULL;
	other->partner = next_tree;
	next_tree->partner = other;
	
        if(other->key < next_tree->key) {    
            trih_add_child(other, tree);
        }
        else {
	    next_tree->extra = FALSE;
	    other->extra = TRUE;	    
            trih_add_child(next_tree, tree);
        }

	tree->dim++;
	
        c++;
        *a = NULL;  *b = tree;
    }
    else {
        /* Otherwise, both next_tree and next_other exist.  The result consists
         * of a 1 node trunk plus the 3-node trunk which becomes a carry tree.
         * We two trunks are made up as (tree, other, next_other)
         * and (next_tree).  This uses no key comparisons.
         */

	tree->partner=NULL;
	next_tree->partner = NULL;
	next_tree->extra = FALSE;
	next_tree->left = next_tree->right = next_tree;
        next_tree->parent = NULL;

        trih_add_child(other, tree);
	
	tree->dim++;
	
        *a = next_tree;  *b = tree;
    }

    return c;
}


/* trih_add_child() - Adds a new child, c, to node p.  The user should ensure
 * that the correct dimension child is being added.
 */
void trih_add_child(triheap_node_t *c, triheap_node_t *p)
{
    triheap_node_t *l, *r;

    /* If p already has child nodes we must update the sibling pointers.
     * Otherwise only initialise the left and right pointers of the added
     * child.
     */
    if((l = p->child)) {
	r = l->right;
	c->left = l;
	c->right = r;
	r->left = c;
	l->right = c;
    }
    else {
	c->left = c->right = c;
    }

    p->child = c;
    c->parent = p;
}


/* trih_replace_child() - replaces child node `old' and its sub-tree with child
 * node `new' and its sub-tree.
 */
void trih_replace_child(triheap_node_t *old, triheap_node_t *new)
{
    triheap_node_t *parent, *l, *r;

    r = old->right;

    /* If `old' is an only child we only need to initialise the sibling
     * pointers of the new node.  Otherwise we update sibling pointers of other
     * child nodes.
     */
    if(r == old) {
	new->right = new->left = new;
    }
    else {
        l = old->left;
	l->right = new;
	r->left = new;
	new->left = l;
	new->right = r;
    }

    /* Update parent pointer of the new node and possibly the child pointer
     * of the parent node.
     */
    parent = old->parent;
    new->parent = parent;
    if(parent->child == old) parent->child = new;
}



/* Recursively print the nodes of a trinomial heap. */
#if SHOW_trih
void trih_dump_nodes(triheap_node_t *ptr, int level)
{
     triheap_node_t *child_ptr, *partner;
     int i, ch_count;

     /* Print leading whitespace for this level. */
     for(i = 0; i < level; i++) printf("   ");

     printf("%d(%ld)\n", ptr->vertex_no, ptr->key);
     
     if((child_ptr = ptr->child)) {
	 child_ptr = ptr->child->right;
	 
         ch_count = 0;

         do {
             trih_dump_nodes(child_ptr, level+1);
	     if(child_ptr->dim != ch_count) {
                 for(i = 0; i < level+1; i++) printf("   ");
		 printf("error(dim)\n");  exit(1);
	     }
	     if(child_ptr->parent != ptr) {
                 for(i = 0; i < level+1; i++) printf("   ");
		 printf("error(parent)\n");
	     }
             child_ptr = child_ptr->right;
	     ch_count++;
         } while(child_ptr != ptr->child->right);

         if(ch_count != ptr->dim) {
	     for(i = 0; i < level; i++) printf("   ");
             printf("error(ch_count)\n");  exit(1);
         }
     }
     else { 
         if(ptr->dim != 0) {
             for(i = 0; i < level; i++) printf("   ");
	     printf("error(dim)\n"); exit(1);
	 }
     }
     
     if((partner=ptr->partner)) {
	 if(ptr->extra==partner->extra) {
            for(i = 0; i < level; i++) printf("   ");
	    printf("%d - error(extra?)\n", partner->vertex_no);  exit(1);
	 }
	 if(partner->extra) {
             if(partner->dim != ptr->dim) {
                 for(i = 0; i < level; i++) printf("   ");
		 printf("%d - error(dim)\n", partner->vertex_no);  exit(1);
	     }
	     trih_dump_nodes(partner, level);
	 }
     }
     else if(ptr->parent) {
         for(i = 0; i < level; i++) printf("   ");
	 printf("error(no partner)\n");  exit(1);
     }
}
#endif

/* Print out a trinomial heap. */
#if SHOW_trih
void trih_dump(triheap_t *h)
{
    int i;
    triheap_node_t *ptr;

    printf("\n");
    printf("value = %d\n", h->value);
    printf("array entries 0..max_trees =");
    for(i=0; i<h->max_trees; i++) {
        printf(" %d", h->trees[i] ? 1 : 0 );
    }
    printf("\nactive nodes =");
    for(i=0; i<h->max_trees-1; i++) {
	if(h->active[i]) {
            printf(" %d", h->active[i]->vertex_no);
	    if(h->active[i]->dim != i) {
		printf("-error(dim)\n");  exit(1);
	    }
	    if(h->active[i]->extra) {
		printf("-error(extra)\n");  exit(1);
	    }
	    if(!h->active[i]->parent) {
		printf("-error(parent)");  exit(1);
	    }
	}
	else {
	    printf(" _");
	}
    }
    printf("\n\n");
    for(i=0; i<h->max_trees; i++) {
        if((ptr = h->trees[i])) {
            printf("tree %d\n\n", i);
            trih_dump_nodes(ptr, 0);
	    printf("\n");
        }
    }
    fflush(stdout);
}
#endif


/*** Implement the univeral heap structure type ***/

/* Trinomial heap wrapper functions. */

int _trih_delete_min(void *h) {
    return trih_delete_min((triheap_t *)h);
}

void _trih_insert(void *h, int v, long k) {
    trih_insert((triheap_t *)h, v, k);
}

void _trih_decrease_key(void *h, int v, long k) {
    trih_decrease_key((triheap_t *)h, v, k);
}

int _trih_n(void *h) {
    return ((triheap_t *)h)->n;
}

long _trih_key_comps(void *h) {
    return ((triheap_t *)h)->key_comps;
}

void *_trih_alloc(int n) {
    return trih_alloc(n);
}

void _trih_free(void *h) {
    trih_free((triheap_t *)h);
}

void _trih_dump(void *h) {
#if TRIHEAP_DUMP
    trih_dump((triheap_t *)h);
#endif
}


/* Trinomail heap info. */
const heap_info_t TRIHEAP_info = {
    _trih_delete_min,
    _trih_insert,
    _trih_decrease_key,
    _trih_n,
    _trih_key_comps,
    _trih_alloc,
    _trih_free,
    _trih_dump
};
