#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mf.h"

/*** Maximum Flow Algorithm Implementations ***/

/* Implementation Notes
 *
 * MPM algorithm:
 *    - blocks unuseful edges in the layered network before calculating the
 *      maximal flow.
 *    - uses a lookup array for scanning layered network vertices.
 */

/*** --- Algorithm Constants --------------------------------------------- ***/

#define VERY_LARGE_INT 2000000000  /* Large integer used as infinity. */


/*** --- Ford and Fulkerson's Maximum Flow Algorithm --------------------- ***/

/* Computes the maximum flow for the network pointed to by g.  This updates the
 * flow field of the edges of g.  Before using this algorithm, the edges of the
 * graph should have been  assigned  some legal initial flow.
 */
void mf_ford(network_t *g)
{
    int i, n, tos;  /* indexing */
    int v, s, t;  /* vertices */
    
    int max_adjust, flow_delta;   /* flow calculations */
    network_edge_t *edge_ptr, *out_edge_ptr, *in_edge_ptr;  /* edge pointers */

    network_vertex_t *vertices;          /**/
    network_edge_t **stack;              /**/ /* Arrays */
    int *delta;                          /**/
    unsigned char *edge_type, *visited;  /**/

    
    /* Set up local variables for quick access to network data. */
    s = g->s;
    t = g->t;
    n = g->n;
    vertices = g->vertices;

    /* Allocate arrays used by the algorithm. */
    stack = malloc(n * sizeof(network_edge_t *));
    edge_type = malloc(n);
    delta = malloc(n * sizeof(int));
    visited = malloc(n);

    /* When searching for an augmenting path, the algorithm uses a stack to
     * keep track of the edges that have been traversed.  For the edge pointed
     * to by stack[i], the array entry edge_type[i] records the direction of
     * the traversed edge (0 for a forward and 1 for backwards).  The array
     * entry delta[i] specifies the amount of increase or decrease in flow that
     * the edge pointed to by stack[i] allows.
     */    
    tos = 0;
    for(;;) {
	/*** Search for an augmenting path. ***/

	/* Initially no vertices have been visited. */
        for(i = 0; i < n; i++) {
	    visited[i] = 0;
	}
	
	/* Starting from vertex s, search for an augmenting path. */
	v = s;
	visited[v] = 1;
	out_edge_ptr = vertices[v].out_head;
	in_edge_ptr = vertices[v].in_head;
	do {

	    /* Search for a useful edge in the OUT set of vertex v.         .
	     * A useful forward edge points to a vertex not already visited,
	     * and has a flow which can be increased.
	     */
	    while(out_edge_ptr) {
		flow_delta = out_edge_ptr->capacity - out_edge_ptr->flow;
		if(!visited[out_edge_ptr->dest_no] && flow_delta > 0) {
		    edge_type[tos] = 0;
		    delta[tos] = flow_delta;
		    stack[tos++] = out_edge_ptr;
		    v = out_edge_ptr->dest_no;
		    goto useful_edge_found;
		}
		out_edge_ptr = out_edge_ptr->next_out;
	    }

	    /* Search for a useful edge in the IN set of vertex v.
	     * A useful backwards edge originates from a vertex not already
	     * visited, and has a flow which can be decreased.
	     */
	    while(in_edge_ptr) {
		flow_delta = in_edge_ptr->flow;
		if(!visited[in_edge_ptr->source_no] && flow_delta > 0) {
		    edge_type[tos] = 1;
		    delta[tos] = flow_delta;
		    stack[tos++] = in_edge_ptr;
		    v = in_edge_ptr->source_no;
		    goto useful_edge_found;
		}
		in_edge_ptr = in_edge_ptr->next_in;
	    }

            /* If this point is reached, then no useful edge was found. */
	    
	    /* If there are no edges stored on the stack, then there is no
	     * augmenting path, meaning flow is maximum.
	     */
	    if(tos == 0) goto calculation_complete;
	    /* else */

	    /* Restart the search for a useful edge using the edge stored at
	     * the top of the stack.
	     */
	    tos--;
	    if(edge_type[tos] == 0) {
		out_edge_ptr = stack[tos];
		v = out_edge_ptr->source_no;
		in_edge_ptr = vertices[v].in_head;
		out_edge_ptr = out_edge_ptr->next_out;
	    }
	    else {
		in_edge_ptr = stack[tos];
		v = in_edge_ptr->dest_no;
		out_edge_ptr = NULL;
		in_edge_ptr = in_edge_ptr->next_in;
	    }
	    continue;

	    
	  useful_edge_found:
	    /* A useful edge was found, and v was updated.  The search for
	     * useful edges continues using the IN and OUT sets of v.
	     */
	    visited[v] = 1;
	    out_edge_ptr = vertices[v].out_head;
	    in_edge_ptr = vertices[v].in_head;

	    /* If v is equal to t, an augmenting path has been found, and this
	     * search loop exits.
	     */
	} while(v != t);


	/* Determine the maximum amount of adjustment in flow, max_adjust, that
	 * the augmenting path allows.
	 */
	max_adjust = delta[0];
	for(i = 1; i < tos; i++) {
            if(delta[i] < max_adjust) max_adjust = delta[i];
	}

	/* Update the flow of edges on this path by max_adjust. */
	while(tos > 0) {	    
	    edge_ptr = stack[--tos];

	    /* The flow through an edge is either increased or decreased,
	     * depending on the edge's direction.
	     */
	    if(edge_type[tos] == 0) {
	        edge_ptr->flow += max_adjust;
	    }
            else {
	        edge_ptr->flow -= max_adjust;
	    }
	}
    } 

  calculation_complete:
    /* The maximum flow has been computed.  Free the arrays that were allocated
     * and exit.
     */
    free(stack);
    free(edge_type);
    free(delta);
    free(visited);
}

/*** --- End of Ford and Fulkerson's Maximum Flow Algorithm -------------- ***/



/*** --- Dinic Maximum Flow Algorithm ------------------------------------ ***/

/** Layered Network structure type used by Dinic's algorithm. **/

/* Edge structure type.  The copy field is a pointer to the corresponding edge
 * in the main network.  For each vertex in the layered network, only the OUT
 * set is maintained.  The next pointer of edges is used in such a way to
 * constructs a single list of concatenated OUT sets.
 */
typedef struct lnetwork_edge {
    int source_no;
    int dest_no;
    int capacity;
    int flow;
    network_edge_t *copy;
    struct lnetwork_edge *next;
    unsigned char reverse, blocked;
} lnetwork_edge_t;

/* Vertex structure type. */
typedef struct lnetwork_vertex {
    lnetwork_edge_t *out_head;  /* Points to first edge in OUT set. */
    lnetwork_edge_t *out_tail;  /* Points to last edge in OUT set. */
} lnetwork_vertex_t;

/* Layered network structure type. */
typedef struct lnetwork {
    int n;
    int s, t;
    lnetwork_vertex_t *vertices; /* Array of vertices. */
    lnetwork_edge_t *first_edge; /* Points to first edge in layered network. */
    lnetwork_edge_t *last_edge;  /* Points to last edge in layered network. */
} lnetwork_t;

/* Prototypes for functions that maintain the layered network used by Dinic's
 * algorithm.
 */
lnetwork_t *lnetwork_create_blank(network_t *g);
void lnetwork_add_edge(lnetwork_t *g, network_edge_t *copy,
		       unsigned char reverse, int capacity);
void lnetwork_flush(lnetwork_t *g);
void lnetwork_free(lnetwork_t *g);

/* Dinic's maximum flow algorithm.  Computes the maximum flow for the
 * network pointed to by g.  This updates the flow of the edges of g.
 * Before using this algorithm, the edges of the graph should have been
 * assigned  some legal initial flow.
 */
void mf_dinic(network_t *g)
{
    int i, j, n, tos;  /* indexing */
    int v, w, s, t; /* vertices */
    int tos1, tos2, tos3;  /* stack indexing */

    int flow_delta, max_adjust;  /* flow calculations */

    network_edge_t *edge_ptr;                             /**/ /* Pointer   */
    lnetwork_t *h;                                        /**/ /* Variables */
    lnetwork_edge_t *out_edge_ptr, *stop_ptr, *path_edge; /**/
    
    network_vertex_t *vertices;                 /**/
    lnetwork_vertex_t *hvertices;               /**/
    lnetwork_edge_t **stack;                    /**/ /* Arrays */
    int *stack1, *stack2, *stack3, *stack_tmp;  /**/
    int *delta;                                 /**/
    unsigned char *visited, *used;              /**/

    
    /* Set up local variables for quick access to network data. */
    n = g->n;
    s = g->s;
    t = g->t;
    vertices = g->vertices;
    
    /* Allocate arrays used by the algorithm. */
    visited = malloc(n);
    used = malloc(n);
    stack1 = malloc(n * sizeof(int));
    stack2 = malloc(n * sizeof(int));
    stack3 = malloc(n * sizeof(int));
    stack = malloc(n * sizeof(lnetwork_edge_t *));
    delta = malloc(n * sizeof(int));

    /* Allocate space for a layered network. */
    h = lnetwork_create_blank(g);
    hvertices = h->vertices;

    for(;;) {
	
      /*** Construct the layered network. ***/
	
	/* The array `visited' keeps track of whether a vertex has been visited
	 * through a useful edge.  The array `used' keeps track of which
	 * vertices are in previously constructed layers.
	 */
       	memset(visited, 0, n);
	memset(used, 0, n);

	/* Two stacks are used for keeping track of vertices as layers are
	 * constructed.  Each time a new layer has been constructed, the stacks
	 * are flipped between by swapping the pointers. The arrays pointed to
	 * by stack1 and stack2 store vertices of the last constructed layer.
	 * The array pointed to by stack3 stores vertices that are a part of
	 * the layer currently under construction.
	 */
	stack1[0] = s;
	stack2[0] = t;
	tos1 = 1;
	tos2 = 1;
	tos3 = 0;
	visited[s] = used[s] = 1;
	visited[t] = used[t] = 2;
	j = 0;  /* flag: j becomes 1 when the source and sink are connected. */

	/* Construct each layer, until the final layer containing the sink
	 * vertex is constructed.
	 */
	for(;;) {
	    
	    /* An edge v-->w will be added to the layered network if there
	     * is a `useful' edge, v-->w or v<--w, and w is not
	     * used in any previous layer (i.e. used[w] == 0).  Edges in
	     * the layered network have a `reverse' field which specifies
	     * the direction of the corresponding edge in the main network.
	     */

	    /* The layered network can have several edges pointing to a
	     * vertex, w, but w must only be placed on stack3 once.
	     * To ensure a vertex, w, is only placed on stack3 at most
	     * once the array entry visited[w] is set to 1 once w has been
	     * added to stack3.
	     */
	    
            /* Construct the next forward layer using vertices which can be
	     * reached through useful edges from the last forward layer.
	     */
	    for(i = 0; i < tos1; i++) {
                v = stack1[i];
				
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetwork_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}

		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetwork_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 1;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */

	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack1;
	    stack1 = stack3;
	    stack3 = stack_tmp;
	    tos1 = tos3;
	    tos3 = 0;

	    
            /* Construct the next backward layer using vertices which can be
	     * reached through useful edges from the last backward layer.
	     */
	    for(i = 0; i < tos2; i++) {
                v = stack2[i];
		
		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetwork_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetwork_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 2;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */
	    
	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack2;
	    stack2 = stack3;
	    stack3 = stack_tmp;
	    tos2 = tos3;
	    tos3 = 0;	    
	}
	
      /*** Find a maximal flow in the layered network. ***/	

	for(;;) {
	
	    /* Starting from vertex s, search for an augmenting path. */
	    tos = 0;
	    v = s;
	    out_edge_ptr = hvertices[v].out_head;
	    do {
		
		/* Search for an unblocked edge in the next layer. */
		stop_ptr = out_edge_ptr != NULL ?
		           hvertices[v].out_tail->next : NULL;
		while(out_edge_ptr != stop_ptr) {
		    if(!out_edge_ptr->blocked) {
		        flow_delta = out_edge_ptr->capacity
			             - out_edge_ptr->flow;
			delta[tos] = flow_delta;
			stack[tos++] = out_edge_ptr;
			v = out_edge_ptr->dest_no;
			goto useful_edge_found;
		    }
		    out_edge_ptr = out_edge_ptr->next;
		}
		
                /* If this point is reached, then no useful edge was found. */
	    
	        /* If there are no edges stored on the stack, then there is no
	         * augmenting path, meaning flow is maximal.
	         */
		if(tos == 0) goto flow_is_maximal;
		/* else */
		
		/* Restart the traversal using the edge stored on the top of
		 * the stack, and mark that edge as blocked so that it will not
		 * be traversed again.
		 */
		tos--;
		out_edge_ptr = stack[tos];
		out_edge_ptr->blocked = 1;
		v = out_edge_ptr->source_no;
		out_edge_ptr = out_edge_ptr->next;
		continue;
		
	      useful_edge_found:
		/* A useful edge was found and v was updated.  The search for
		 * useful edges continues using the OUT set of v.
		 */
		out_edge_ptr = hvertices[v].out_head;
		
	        /* If v is equal to t, an augmenting path has been found, and
		 * this search loop exits.
	         */
	    } while(v != t);
	    
	    /* An augmenting path has been found.  Find the maximum increase
	     * in flow allowed by the path.
	     */
	    max_adjust = delta[0];
	    for(i = 1; i < tos; i++) {
		if(delta[i] < max_adjust) max_adjust = delta[i];
	    }
	    
	    /* Update the flow of edges on this path. */
	    while(tos > 0) {	    
		path_edge = stack[--tos];
		path_edge->flow += max_adjust;
                if(path_edge->flow == path_edge->capacity)
		    path_edge->blocked = 1;
	    }
	}
	
      flow_is_maximal:
	/* The maximal flow in the layered network has been computed. */

	/* Use the flow in the layered network to increase the flow in the
	 * main network, and reset the layered network to blank.
	 */
        lnetwork_flush(h);
    }
    
  calculation_complete:
    /* The maximum flow has been computed.  Free the arrays that were allocated
     * and exit.
     */
    free(visited);
    free(used);
    free(stack1);
    free(stack2);
    free(stack3);
    free(stack);
    free(delta);
    lnetwork_free(h);
}

/* Create a blank layered network using the same vertex numbering as the
 * network pointed to by g.  By `blank' we mean a network containing no edges.
 */
lnetwork_t *lnetwork_create_blank(network_t *g)
{
    int i, n;
    lnetwork_t *h;
    lnetwork_vertex_t *vertices, *vertex;

    n = g->n;
    h = malloc(sizeof(lnetwork_t));
    vertices = h->vertices = malloc(n * sizeof(lnetwork_vertex_t));
    
    /* The layered network has the same number of vertices and the same sink
     * and source vertices as the network pointed to by g.  Edges in the
     * layered network are different from those in the network pointed to by g.
     */
    h->s = g->s;
    h->t = g->t;
    h->n = n;
    
    h->first_edge = h->last_edge = NULL;
    for(i = 0; i < n; i++) {
	vertex = &vertices[i];
	vertex->out_head = vertex->out_tail = NULL;
    }
    
    return h;
}

/* Insert a single edge into the layered network pointed to by g.  The `copy'
 * parameter is a pointer to the edge in the main network that the new edge is
 * to correspond to.  The `reverse' parameter indicates the direction that edge
 * `copy' was traversed in.
 */
void lnetwork_add_edge(lnetwork_t *g, network_edge_t *copy,
		       unsigned char reverse, int capacity)
{
    int u, v;
    lnetwork_vertex_t *vertex;
    lnetwork_edge_t *e;

    /* The reverse parameter indicates the direction of edge traversal when
     * constructing the layer.
     */
    if(reverse) {
	/* Edge was traversed as v<--w from v to w. */
	u = copy->dest_no;
	v = copy->source_no;
    }
    else {
	/* Edge was traversed as v-->w from v to w. */
	u = copy->source_no;
	v = copy->dest_no;
    }

    /* Create the new edge and initialise its fields. */
    e = malloc(sizeof(lnetwork_edge_t));
    e->copy = copy;
    e->source_no = u;
    e->dest_no = v;
    e->capacity = capacity;
    e->flow = 0;
    e->reverse = reverse;
    e->blocked = 0;

    /* Update the edge list pointers. */
    vertex = &g->vertices[u];
    if(vertex->out_head != NULL) {
	e->next = vertex->out_tail->next;
	vertex->out_tail->next = e;
	if(!e->next) g->last_edge = e;
    }
    else {
	e->next = NULL;
	if(g->first_edge != NULL) g->last_edge->next = e;
	else g->first_edge = e;
	g->last_edge = vertex->out_head = e;
    }
    vertex->out_tail = e;
}

/* For all edges in the layered network pointed to by g, update the flow of
 * corresponding edges in the main network.  After the flow has been updated,
 * edges are removed from the layered network, leaving a blank layered network.
 */
void lnetwork_flush(lnetwork_t *g)
{
    int i;
    lnetwork_vertex_t *vertices, *vertex;
    lnetwork_edge_t *edge_ptr, *next_edge_ptr;

    vertices = g->vertices;

    /* Traverse the entire edge-list of g, updating edge flows in the main
     * network, and freeing edges from g.
     */
    next_edge_ptr = g->first_edge;
    while(next_edge_ptr) {
	edge_ptr = next_edge_ptr;
	next_edge_ptr = edge_ptr->next;
	if(edge_ptr->reverse == 0) {
	    edge_ptr->copy->flow += edge_ptr->flow;
	}
	else {
	    edge_ptr->copy->flow -= edge_ptr->flow;
	}
	free(edge_ptr);
    }

    /* Reset the OUT set pointer of each vertex. */
    for(i = 0; i < g->n; i++) {
	vertex = &vertices[i];
	vertex->out_head = vertex->out_tail = NULL;
    }
    g->first_edge = g->last_edge = NULL;
}

/* Destroy the layered network pointed to by g, including all of its edges. */
void lnetwork_free(lnetwork_t *g)
{
    lnetwork_edge_t *edge_ptr, *next_edge_ptr;

    next_edge_ptr = g->first_edge;

    while(next_edge_ptr != NULL) {
	edge_ptr = next_edge_ptr;
	next_edge_ptr = edge_ptr->next;
	free(edge_ptr);
    }

    free(g->vertices);
    free(g);
}

/*** --- End of Dinic Maximum Flow Algorithm ----------------------------- ***/



/*** --- MPM Maximum Flow Algorithm -------------------------------------- ***/

/*** Layered Network structure type used by the MPM algorithm. ***/
/*   The MPM max-flow algorithm requires the layered network to maintain
 *   a list of incoming edges for each vertex, so the `b' in the naming
 *   `lnetworkb' indicates that the layered network maintains a list of
 *   back-edges.
 */

/* Edge structure type.  The copy field is a pointer to the corresponding edge
 * in the main network.  The next_out pointer of the last edge in the OUT set
 * will be NULL.  The next_in pointer of the last edge in the IN set will be
 * NULL.  The other next pointer is used to construct a list containing every
 * edge in the layered network.
 */
typedef struct lnetworkb_edge {
    int source_no;
    int dest_no;
    int capacity;
    int flow;
    network_edge_t *copy;
    struct lnetworkb_edge *next_out;
    struct lnetworkb_edge *next_in;
    struct lnetworkb_edge *next;
    unsigned char reverse, blocked;
} lnetworkb_edge_t;

/* Vertex structure type. */
typedef struct lnetworkb_vertex {
    lnetworkb_edge_t *out_head;
    lnetworkb_edge_t *out_tail;
    lnetworkb_edge_t *in_head;
    lnetworkb_edge_t *in_tail;
} lnetworkb_vertex_t;

/* Layered network structure type. */
typedef struct lnetworkb {
    int n;
    int s, t;
    lnetworkb_vertex_t *vertices;
    lnetworkb_edge_t *first_edge;
    lnetworkb_edge_t *last_edge;
} lnetworkb_t;

/* Prototypes for functions that maintain the layered network used by Dinic's
 * algorithm and the MPM algorithm.
 */
signed char *dfs_used;
lnetworkb_t *lnetworkb_create_blank(network_t *g);
void lnetworkb_add_edge(lnetworkb_t *g, network_edge_t *copy,
		       unsigned char reverse, int capacity);
void lnetworkb_flush(lnetworkb_t *g);
void lnetworkb_free(lnetworkb_t *g);

/* MPM maximum flow algorithm.  Computes the maximum flow for the
 * network pointed to by g.  This updates the flow of the edges of g.
 * Before using this algorithm, the edges of the graph should have been
 * assigned  some legal initial flow.
 */
void mf_mpm(network_t *g)
{
    int i, j, n, tos1, tos2, tos3;  /* indexing */
    int u, v, w, s, t;  /* vertices */

    int min_i, min_p, new_p, p_edge, flow_delta;
        /* flow calculations */

    network_edge_t *edge_ptr;     /**/ /* Pointer Variables */
    lnetworkb_t *h;               /**/
    lnetworkb_edge_t *h_edge_ptr; /**/
    
    lnetworkb_vertex_t *hvertices;              /**/
    network_vertex_t *vertices;                 /**/
    lnetworkb_edge_t **next_in, **next_out;     /**/
    int *stack1, *stack2, *stack3, *stack_tmp;  /**/ /* Arrays */
    int *delta;                                 /**/
    int *ip, *op, *p, *in_flow, *out_flow;      /**/
    unsigned char *visited, *used;              /**/
    int *lookup;                                /**/

    
    /* Set up local variables for quick access to network data. */
    n = g->n;
    s = g->s;
    t = g->t;
    vertices = g->vertices;
    
    /* Allocate arrays used by the algorithm. */
    next_in = malloc(n * sizeof(lnetworkb_edge_t *));
    next_out = malloc(n * sizeof(lnetworkb_edge_t *));
    visited = malloc(n);
    used = malloc(n);
    ip = malloc(n * sizeof(int));
    op = malloc(n * sizeof(int));
    p = malloc(n * sizeof(int));
    in_flow = malloc(n * sizeof(int));
    out_flow = malloc(n * sizeof(int));
    stack1 = malloc(n * sizeof(int));
    stack2 = malloc(n * sizeof(int));
    stack3 = malloc(n * sizeof(int));
    delta = malloc(n * sizeof(int));
    lookup = malloc(n * sizeof(int));
    
    /* Allocate space for a layered network. */
    h = lnetworkb_create_blank(g);
    hvertices = h->vertices;

    for(;;) {

      /*** Construct the layered network. ***/
	
	/* The array `visited' keeps track of whether a vertex has been visited
	 * through a useful edge.  The array `used' keeps track of which
	 * vertices are in previously constructed layers.
	 */
       	memset(visited, 0, n);
	memset(used, 0, n);

	/* Two stacks are used for keeping track of vertices as layers are
	 * constructed.  Each time a new layer has been constructed, the stacks
	 * are flipped between by swapping the pointers. The arrays pointed to
	 * by stack1 and stack2 store vertices of the last
	 * constructed layer.  The array pointed to by stack3 stores vertices
	 * that are a part of the layer currently under construction.
	 */
	stack1[0] = s;
	stack2[0] = t;
	tos1 = 1;
	tos2 = 1;
	tos3 = 0;
	visited[s] = used[s] = 1;
	visited[t] = used[t] = 2;
	j = 0;  /* flag: j becomes 1 when the source and sink are connected. */

	/* Construct each layer, until the final layer containing the sink
	 * vertex is constructed.
	 */
	for(;;) {
	    
	    /* An edge v-->w will be added to the layered network if there
	     * is a `useful' edge, v-->w or v<--w, and w is not
	     * used in any previous layer (i.e. used[w] == 0).  Edges in
	     * the layered network have a `reverse' field which specifies
	     * the direction of the corresponding edge in the main network.
	     */

	    /* The layered network can have several edges pointing to a
	     * vertex, w, but w must only be placed on stack3 once.
	     * To ensure a vertex, w, is only placed on stack3 at most
	     * once the array entry visited[w] is set to 1 once w has been
	     * added to stack3.
	     */
	    
            /* Construct the next forward layer using vertices which can be
	     * reached through useful edges from the last forward layer.
	     */
	    for(i = 0; i < tos1; i++) {
                v = stack1[i];
				
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetworkb_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}

		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetworkb_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 1;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */

	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack1;
	    stack1 = stack3;
	    stack3 = stack_tmp;
	    tos1 = tos3;
	    tos3 = 0;

	    
            /* Construct the next backward layer using vertices which can be
	     * reached through useful edges from the last backward layer.
	     */
	    for(i = 0; i < tos2; i++) {
                v = stack2[i];
		
		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetworkb_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetworkb_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 2;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */
	    
	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack2;
	    stack2 = stack3;
	    stack3 = stack_tmp;
	    tos2 = tos3;
	    tos3 = 0;	    
	}

      /*** Scan the layered network. ***/

	/* Perform forward and reverse depth-first searches from t, unblocking
	 * edges, and calculating ip[v] (in-potential) and
	 * op[v] (out-potential) for each vertex v.  Edges added to the layered
	 * network which do not lead from s to t will remain blocked.  The
	 * lookup[] will be initialised to index all useful vertices.
	 */
        memset(ip, 0, n * sizeof(int));
        memset(op, 0, n * sizeof(int));
        ip[g->s] = VERY_LARGE_INT;
        op[g->t] = VERY_LARGE_INT;
	j = 0;  /* number of useful vertices seen */
        memset(visited, 0, n);
	tos1 = 0;

        for(i = 0; i < tos3; i++) {
	    if(used[stack3[i]] != 3) continue;
	    /* else */

	    lookup[j++] = stack3[i];
	    
	    /* Unblock forward-traversed edges v-->w, and update potentials */
            v = stack3[i];
	    for(;;) {
		h_edge_ptr = hvertices[v].out_head;
		while(h_edge_ptr != NULL) {
		    h_edge_ptr->blocked = 0;
		    w = h_edge_ptr->dest_no;
		    op[v] += h_edge_ptr->capacity;
		    ip[w] += h_edge_ptr->capacity;
		    if(!visited[w]) {  /* Add w to the stack at most once. */
			visited[w] = 1;
			stack1[tos1++] = w;
		    }
		    h_edge_ptr = h_edge_ptr->next_out;
		}
		if(tos1==0) break;
		v = stack1[--tos1];
		lookup[j++] = v;
	    }
	    
	    /* Unblock reverse-traversed edges v-->w, and update potentials */
	    w = stack3[i];
	    for(;;) {
		h_edge_ptr = hvertices[w].in_head;
		while(h_edge_ptr != NULL) {
		    h_edge_ptr->blocked = 0;
		    v = h_edge_ptr->source_no;
		    op[v] += h_edge_ptr->capacity;
		    ip[w] += h_edge_ptr->capacity;
		    if(!visited[v]) {  /* Add v to the stack at most once. */
			visited[v] = 1;
			stack1[tos1++] = v;
		    }
		    h_edge_ptr = h_edge_ptr->next_in;
		}
		if(tos1==0) break;
		w = stack1[--tos1];
		lookup[j++] = w;
	    }
	}	
	
      /*** Find a maximal flow in the layered network. ***/
	
	/* Set up pointers to the next unused edge of each vertex in the
	 * layered network.
	 */
	for(i = 0; i < n; i++) {
	    next_out[i] = hvertices[i].out_head;
	    next_in[i] = hvertices[i].in_head;
	}
	
        for(;;) {

	  /*** Find v such that p[v] is minimum among unused vertices v. ***/
	    
	    /* Calculate the array entries p[v] = min(ip[v], op[v]), and
	     * find the minimum p[v] among unused vertices v.  Unused
	     * vertices v have p[v] equal to -1.
	     */
	    min_p = VERY_LARGE_INT;
	    for(i = 0; i < j; i++) {
		v = lookup[i];
		new_p = ip[v] < op[v] ? ip[v] : op[v];
		p[v] = new_p;
		if(new_p < min_p) {
		    min_p = new_p;
		    min_i = i;
		}
	    }
	    v = lookup[min_i];  /* select minimum vertex, v */
	    j--;
	    lookup[min_i] = lookup[j];  /* replace v in lookup */
	    

            /* Take care of vertices with a potential of zero.  Such vertices
	     * are unuseful dead ends resulting after other vertices and
	     * edges connected to them were used up.  Once the flow is maximal
	     * the potential of s or t will eventually become zero.
	     */
	    if(p[v] == 0) {		
                if(v == s || v == t) break;  /* flow is maximal */
                /* else */
		
		goto skip_push_pull;  /* just block of edges */
	    }
	    /* else */	    

	    
	  /*** Initialise vertex flow information. ***/

	    for(i = 0; i < j; i++) {
		w = lookup[i];
		visited[w] = in_flow[w] = out_flow[w] = 0;
	    }	    
	    in_flow[v] = out_flow[v] = p[v];
	    visited[v] = 1;
	    
          /*** Push the flow to the forward layers. ***/

	    stack1[0] = v;
	    tos1 = 1;
	    tos2 = 0;
	
            /* For each forward layer, we push the flow through affected
	     * vertices.
	     */
	    for(;;) {

		/* For all affected vertices u of the current layer, push flow
		 * through the outgoing edges of u.  At the same time, affected
		 * vertices of the next layer are added to stack2.
		 */
		for(i = 0; i < tos1; i++) {
		    u = stack1[i];
		
		    /* Try outgoing edges u--e-->w from the current vertex u.
		     */
		    h_edge_ptr = next_out[u];
		    while(h_edge_ptr != NULL) {
			if(!h_edge_ptr->blocked) {
			    w = h_edge_ptr->dest_no;
			    if(!visited[w]) {
				stack2[tos2++] = w;
				visited[w] = 1;
			    }
			    
			    p_edge = h_edge_ptr->capacity - h_edge_ptr->flow;
			    if(out_flow[u] >= p_edge) {
				out_flow[u] -= p_edge;
				out_flow[w] += p_edge;
				op[u] -= p_edge;
				ip[w] -= p_edge;
				h_edge_ptr->flow = h_edge_ptr->capacity;
				h_edge_ptr->blocked = 1;
			    }
			    else {
				out_flow[w] += out_flow[u];
				op[u] -= out_flow[u];
				ip[w] -= out_flow[u];
				h_edge_ptr->flow += out_flow[u];
				break;  /* No flow left for this vertex. */
			    }	    
			}
			h_edge_ptr = h_edge_ptr->next_out;
		    }
		    next_out[u] = h_edge_ptr;
		}

		/* If vertex t has been visited, then the final layer was
		 * reached and the pushing of flows is complete.
		 */
		if(visited[t]) break;	    

		/* Prepare to push flow through the next layer. */
		stack_tmp = stack1;
		stack1 = stack2;
		stack2 = stack_tmp;
		tos1 = tos2;
		tos2 = 0;		
	    } /* for (;;)  (next forward layer) */

	    
	  /*** Pull the flow through the back layers. ***/

	    stack1[0] = v;
	    tos1 = 1;
	    tos2 = 0;
	    
            /* For each back layer, we pull the flow through affected vertices.
	     */
	    for(;;) {

		/* For all affected vertices u of the current layer, pull flow
		 * through the incoming edges of u.  At the same time,
		 * affected vertices of the next layer are added to stack2.
		 */
		for(i = 0; i < tos1; i++) {
		    u = stack1[i];
		
		    /* Try incoming edges w--e-->u to the current vertex u.
		     */
		    h_edge_ptr = next_in[u];
		    while(h_edge_ptr != NULL) {			
			if(!h_edge_ptr->blocked) {
			    w = h_edge_ptr->source_no;
			    if(!visited[w]) {
				stack2[tos2++] = w;
				visited[w] = 1;
			    }
			    
			    p_edge = h_edge_ptr->capacity - h_edge_ptr->flow;
			    if(in_flow[u] >= p_edge) {
				in_flow[u] -= p_edge;
				in_flow[w] += p_edge;
				ip[u] -= p_edge;
				op[w] -= p_edge;
				h_edge_ptr->flow = h_edge_ptr->capacity;
				h_edge_ptr->blocked = 1;
			    }
			    else {
				in_flow[w] += in_flow[u];
				ip[u] -= in_flow[u];
				op[w] -= in_flow[u];
				h_edge_ptr->flow += in_flow[u];
				break;  /* No flow left for this vertex. */
			    }	    
			}
			h_edge_ptr = h_edge_ptr->next_in;
		    }
		    next_in[u] = h_edge_ptr;
		}

		/* If vertex s has been visited, then the final layer was
		 * reached and the pushing of flows is complete.
		 */
		if(visited[s]) break;	    

		/* Prepare to pull flow through the next layer. */
		stack_tmp = stack1;
		stack1 = stack2;
		stack2 = stack_tmp;
		tos1 = tos2;
		tos2 = 0;
		
	    } /* for(;;)  (next back layer) */

	  skip_push_pull:
	    
          /*** Vertex v, and its edges are no longer useful. ***/

            /* Update the in-potential of vertices through unused outgoing
	     * edges.
	     */
	    h_edge_ptr = next_out[v];
	    while(h_edge_ptr != NULL) {
		if(!h_edge_ptr->blocked) {
		    ip[h_edge_ptr->dest_no] -= h_edge_ptr->capacity
			- h_edge_ptr->flow;
		    h_edge_ptr->blocked = 1;
		}
		h_edge_ptr = h_edge_ptr->next_out;
	    }

            /* Update the out-potential of vertices through unused incoming
	     * edges.
	     */
	    h_edge_ptr = next_in[v];
	    while(h_edge_ptr != NULL) {
		if(!h_edge_ptr->blocked) {
		    op[h_edge_ptr->source_no] -= h_edge_ptr->capacity
			- h_edge_ptr->flow;
		    h_edge_ptr->blocked = 1;
		}
		h_edge_ptr = h_edge_ptr->next_in;
	    }
	    
	} /* for(;;)  (next minimum p[v] vertex) */
	
	/* After the main loop above exits, the maximal flow in the layered
	 * network has been computed.
	 */

	/* Use the flow in the layered network to increase the flow in the
	 * main network, and reset the layered network to blank.
	 */
        lnetworkb_flush(h);
    }
    
  calculation_complete:
    /* The maximum flow has been computed.  Free the arrays that were allocated
     * and exit.
     */
    free(next_in);
    free(next_out);
    free(visited);
    free(used);
    free(stack1);
    free(stack2);
    free(stack3);
    free(delta);
    lnetworkb_free(h);
    free(ip);  free(op);  free(p);
    free(in_flow);  free(out_flow);
    free(lookup);
}

/* Create a blank layered network using the same vertex numbering as the
 * network pointed to by g.  By `blank' we mean a network containing no edges.
 */
lnetworkb_t *lnetworkb_create_blank(network_t *g)
{
    int i, n;
    lnetworkb_t *h;
    lnetworkb_vertex_t *vertices, *vertex;

    n = g->n;
    h = malloc(sizeof(lnetworkb_t));
    vertices = h->vertices = malloc(n * sizeof(lnetworkb_vertex_t));
    
    /* The layered network has the same number of vertices and the same sink
     * and source vertices as the network pointed to by g.
     */
    h->s = g->s;
    h->t = g->t;
    h->n = n;
    
    h->first_edge = h->last_edge = NULL;
    for(i = 0; i < n; i++) {
	vertex = &vertices[i];
	vertex->in_head = vertex->in_tail
	    = vertex->out_head = vertex->out_tail = NULL;
    }
    
    return h;
}

/* Insert a single edge into the layered network pointed to by g.  The `copy'
 * parameter is a pointer to the edge in the main network that the new edge is
 * to correspond to.  The `reverse' parameter indicates the direction that edge
 * `copy' was traversed in.
 */
void lnetworkb_add_edge(lnetworkb_t *g, network_edge_t *copy,
		       unsigned char reverse, int capacity)
{
    int u, v;
    lnetworkb_vertex_t *vertex;
    lnetworkb_edge_t *e;

    /* The reverse parameter indicates the direction of edge traversal when
     * constructing the layer.
     */
    if(reverse) {
	/* Edge was traversed as v<--w from v to w. */
	u = copy->dest_no;
	v = copy->source_no;
    }
    else {
	/* Edge was traversed as v-->w from v to w. */
	u = copy->source_no;
	v = copy->dest_no;
    }

    /* Create the new edge and initialise its fields. */
    e = malloc(sizeof(lnetworkb_edge_t));
    e->copy = copy;
    e->source_no = u;
    e->dest_no = v;
    e->capacity = capacity;
    e->flow = 0;
    e->reverse = reverse;
    e->blocked = 1;
    e->next_in = e->next_out = e->next = NULL;

    /* Add to the list of outgoing edges. */
    vertex = &g->vertices[u];
    if(vertex->out_head != NULL) vertex->out_tail->next_out = e;
    else vertex->out_head = e;
    vertex->out_tail = e;

    /* Add to the list of incoming edges. */
    vertex = &g->vertices[v];
    if(vertex->in_head != NULL) vertex->in_tail->next_in = e;
    else vertex->in_head = e;
    vertex->in_tail = e; 

    /* Add to the list of all edges in the graph. */
    if(g->first_edge != NULL) g->last_edge->next = e;
    else g->first_edge = e;    
    g->last_edge = e;
}

/* For all edges in the layered network pointed to by g, update the flow of
 * corresponding edges in the main network.  After the flow has been updated,
 * edges are removed from the layered network, leaving a blank layered network.
 */
void lnetworkb_flush(lnetworkb_t *g)
{
    int i;
    lnetworkb_vertex_t *vertices, *vertex;
    lnetworkb_edge_t *edge_ptr, *next_edge_ptr;

    vertices = g->vertices;

    /* Traverse the entire edge-list of g, updating edge flows in the main
     * network, and freeing edges from g.
     */
    next_edge_ptr = g->first_edge;
    while(next_edge_ptr) {
	edge_ptr = next_edge_ptr;
	next_edge_ptr = edge_ptr->next;
	if(edge_ptr->reverse == 0) {
	    edge_ptr->copy->flow += edge_ptr->flow;
	}
	else {
	    edge_ptr->copy->flow -= edge_ptr->flow;
	}

        free(edge_ptr);
    }

    /* Reset the OUT set pointer of each vertex. */
    for(i = 0; i < g->n; i++) {
	vertex = &vertices[i];
	vertex->in_head = vertex->in_tail =
	    vertex->out_head = vertex->out_tail = NULL;
    }
    g->first_edge = g->last_edge = NULL;
}

/* Destroy the layered network pointed to by g, including all of its edges. */
void lnetworkb_free(lnetworkb_t *g)
{
    lnetworkb_edge_t *edge_ptr, *next_edge_ptr;

    next_edge_ptr = g->first_edge;

    while(next_edge_ptr != NULL) {
	edge_ptr = next_edge_ptr;
	next_edge_ptr = edge_ptr->next;
	free(edge_ptr);
    }

    free(g->vertices);
    free(g);
}

/*** --- End of MPM Maximum Flow Algorithm ------------------------------- ***/



/*** --- Karzanov Maximum Flow Algorithm ----------------------------------***/

/* Note:  This implementation of the Karzanov algorithm borrows the layered
 * network implementation used by the MPM algorithm.
 */

/* Karzanov maximum flow algorithm.  Computes the maximum flow for the
 * network pointed to by g.  This updates the flow of the edges of g.
 * Before using this algorithm, the edges of the graph should have been
 * assigned  some legal initial flow.
 */
void mf_karzanov(network_t *g)
{
    /* Edge list structure type. */
    typedef struct edge_list {
        lnetworkb_edge_t *edge;
        struct edge_list *next;
    } edge_list_t;    
    
    int i, j, n, tos1, tos2, tos3, from_i, to_i;  /* indexing */
    int u, v, w, s, t;  /* vertices */

    int p_edge, flow_delta;
        /* flow calculations */

    network_edge_t *edge_ptr;            /**/ /* Pointer Variables */
    lnetworkb_t *h;                      /**/
    lnetworkb_edge_t *h_edge_ptr;        /**/
    edge_list_t *list_item, *next_item;  /**/
    
    lnetworkb_vertex_t *hvertices;              /**/
    network_vertex_t *vertices;                 /**/
    lnetworkb_edge_t **next_out;                /**/
    int *stack1, *stack2, *stack3, *stack_tmp;  /**/ /* Arrays */
    int *delta;                                 /**/
    int *excess;                                /**/
    unsigned char *visited, *used;              /**/
    edge_list_t **edge_stack;                   /**/

    
    /* Set up local variables for quick access to network data. */
    n = g->n;
    s = g->s;
    t = g->t;
    vertices = g->vertices;
    
    /* Allocate arrays used by the algorithm. */
    next_out = malloc(n * sizeof(lnetworkb_edge_t *));
    visited = malloc(n);
    used = malloc(n);
    excess = malloc(n * sizeof(int));
    stack1 = malloc(n * sizeof(int));
    stack2 = malloc(n * sizeof(int));
    stack3 = malloc(2 * n * sizeof(int));
    delta = malloc(n * sizeof(int));
    edge_stack = malloc(n * sizeof(edge_list_t *));
    
    /* Allocate space for a layered network. */
    h = lnetworkb_create_blank(g);
    hvertices = h->vertices;

    for(;;) {

      /*** Construct the layered network. ***/
	
	/* The array `visited' keeps track of whether a vertex has been visited
	 * through a useful edge.  The array `used' keeps track of which
	 * vertices are in previously constructed layers.
	 */
       	memset(visited, 0, n);
	memset(used, 0, n);

	/* Two stacks are used for keeping track of vertices as layers are
	 * constructed.  Each time a new layer has been constructed, the stacks
	 * are flipped between by swapping the pointers. The arrays pointed to
	 * by stack1 and stack2 store vertices of the last
	 * constructed layer.  The array pointed to by stack3 stores vertices
	 * that are a part of the layer currently under construction.
	 */
	stack1[0] = s;
	stack2[0] = t;
	tos1 = 1;
	tos2 = 1;
	tos3 = 0;
	visited[s] = used[s] = 1;
	visited[t] = used[t] = 2;
	j = 0;  /* flag: j becomes 1 when the source and sink are connected. */

	/* Construct each layer, until the final layer containing the sink
	 * vertex is constructed.
	 */
	for(;;) {
	    
	    /* An edge v-->w will be added to the layered network if there
	     * is a `useful' edge, v-->w or v<--w, and w is not
	     * used in any previous layer (i.e. used[w] == 0).  Edges in
	     * the layered network have a `reverse' field which specifies
	     * the direction of the corresponding edge in the main network.
	     */

	    /* The layered network can have several edges pointing to a
	     * vertex, w, but w must only be placed on stack3 once.
	     * To ensure a vertex, w, is only placed on stack3 at most
	     * once the array entry visited[w] is set to 1 once w has been
	     * added to stack3.
	     */
	    
            /* Construct the next forward layer using vertices which can be
	     * reached through useful edges from the last forward layer.
	     */
	    for(i = 0; i < tos1; i++) {
                v = stack1[i];
				
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetworkb_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}

		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&1) && flow_delta > 0) {
                        if(!(visited[w]&1)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 1;
			}
			lnetworkb_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 1;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */

	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack1;
	    stack1 = stack3;
	    stack3 = stack_tmp;
	    tos1 = tos3;
	    tos3 = 0;

	    
            /* Construct the next backward layer using vertices which can be
	     * reached through useful edges from the last backward layer.
	     */
	    for(i = 0; i < tos2; i++) {
                v = stack2[i];
		
		/* Try incoming edges for the current vertex. */
		edge_ptr = vertices[v].in_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->capacity - edge_ptr->flow;
		    if(!(used[w=edge_ptr->source_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetworkb_add_edge(h, edge_ptr, 0, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_in;
		}
		
		/* Try outgoing edges from the current vertex. */
		edge_ptr = vertices[v].out_head;
		while(edge_ptr) {
		    flow_delta = edge_ptr->flow;
		    if(!(used[w=edge_ptr->dest_no]&2) && flow_delta > 0) {
                        if(!(visited[w]&2)) {
			    stack3[tos3++] = w;
			    visited[w] = visited[w] | 2;
			}
			lnetworkb_add_edge(h, edge_ptr, 1, flow_delta);
		    }
		    edge_ptr = edge_ptr->next_out;
		}		    
	    }

            /* If there were no useful vertices to construct the new layer,
	     * then the present flow is maximum.
	     */
	    if(tos3 == 0) goto calculation_complete;
	    
	    /* A new forward layer has been constructed, mark vertices which
	     * are in it as `used' by the forward scan.  If the source and sink
	     * have been connected, then no more layers need to be added.  This
	     * shows up as used[v] == 3, where v is a connecting vertex in the
	     * added layer.
	     */
            for(i = 0; i < tos3; i++) {
		w = stack3[i];
		used[w] = used[w] | 2;
                if(used[w] == 3) j = 1;
	    }
	    if(j) break;  /* construction completed */
	    
	    /* Make the new layer the current forward layer. */
	    stack_tmp = stack2;
	    stack2 = stack3;
	    stack3 = stack_tmp;
	    tos2 = tos3;
	    tos3 = 0;	    
	}

      /*** Scan the layered network. ***/

	/* Perform forward and reverse depth-first searches from t, unblocking
	 * edges, for each vertex v.  Edges added to the layered
	 * network which do not lead from s to t will remain blocked.
	 */
        memset(visited, 0, n);
	tos1 = 0;

        for(i = 0; i < tos3; i++) {
	    if(used[stack3[i]] != 3) continue;
	    /* else */
	    
	    /* Unblock forward-traversed edges v-->w, and update potentials */
            v = stack3[i];
	    for(;;) {
		h_edge_ptr = hvertices[v].out_head;
		while(h_edge_ptr != NULL) {
		    h_edge_ptr->blocked = 0;
		    w = h_edge_ptr->dest_no;
		    if(!visited[w]) {  /* Add w to the stack at most once. */
			visited[w] = 1;
			stack1[tos1++] = w;
		    }
		    h_edge_ptr = h_edge_ptr->next_out;
		}
		if(tos1==0) break;
		v = stack1[--tos1];
	    }
	    
	    /* Unblock reverse-traversed edges v-->w, and update potentials */
	    w = stack3[i];
	    for(;;) {
		h_edge_ptr = hvertices[w].in_head;
		while(h_edge_ptr != NULL) {
		    h_edge_ptr->blocked = 0;
		    v = h_edge_ptr->source_no;
		    if(!visited[v]) {  /* Add v to the stack at most once. */
			visited[v] = 1;
			stack1[tos1++] = v;
		    }
		    h_edge_ptr = h_edge_ptr->next_in;
		}
		if(tos1==0) break;
		w = stack1[--tos1];
	    }
	}	
	

      /*** Find a maximal flow in the layered network. ***/
	
	/* Set up pointers to the next unused edge of each vertex in the
	 * layered network.  Initialise excess[v] to zero.
	 */
	for(i = 0; i < n; i++) {
	    next_out[i] = hvertices[i].out_head;
	    excess[i] = 0;
	    visited[i] = 0;
	    edge_stack[i] = NULL;
	}
	excess[s] = VERY_LARGE_INT;


	/* stack1[] implements breadth-first-search. */
        /* stack3[] stores all vertices which have excess flow available. */

        /* The excess flow starts at vertex s. */
	stack3[0] = -1;  /* New layer marker. */
	stack3[1] = s;
	tos3 = 2;
	
        /* This loop repeatedly performs push+rebalance until the flow becomes
	 * maximal.
	 */
        for(;;) {
            	    
	  /*** Get vertices of the forward-most layer containing excess flow
	   ***/
	    tos1 = 0;
	    for(;;) {
		v = stack3[--tos3];
		if(v < 0) break;  /* Encountered new layer marker */
		stack1[tos1++] = v;
	    }
	    
          /*** Push excess flow forward... ***/
            from_i = 0;
	    to_i = tos1;
	    for(;;) {  /* ... layer by layer */
		
 	        stack3[tos3++] = -1;  /* Layer marker. */
		
		/* For all affected vertices u of the current layer, push flow
		 * through the outgoing edges of u.  At the same time, affected
		 * vertices of the next layer are added to stack2.
		 */
		for(i = from_i; i < to_i; i++) {
		    u = stack1[i];
		
		    /* Push as mush flow as possible through edges u--e-->w
		     * from the current vertex u.
		     */
		    h_edge_ptr = next_out[u];		    
		    for(;;) {
			
			if(h_edge_ptr == NULL) {
			    stack3[tos3++] = u;
				/* May have excess flow left. */
			    break;
			}
			/* else */
			
			/* Ignore edges blocked from flow rebalancing. */
			if(!h_edge_ptr->blocked) {
			
			    w = h_edge_ptr->dest_no;
                            /* Add w to the stack once only. */
			    if(!visited[w]) {
				stack1[tos1++] = w;			    
				visited[w] = 1;
			    }

                            /* Add this edge to w's stack. */
			    list_item = malloc(n * sizeof(edge_list_t));
			    list_item->edge = h_edge_ptr;
			    list_item->next = edge_stack[w];
			    edge_stack[w] = list_item;

			    /* Push excess flow through the edge. */
			    p_edge = h_edge_ptr->capacity - h_edge_ptr->flow;
			    if(excess[u] >= p_edge) {
				excess[u] -= p_edge;
				excess[w] += p_edge;
				h_edge_ptr->flow = h_edge_ptr->capacity;
				h_edge_ptr->blocked = 1;
			    }
			    else {
				excess[w] += excess[u];
				h_edge_ptr->flow += excess[u];
				excess[u] = 0;
				break;  /* No flow left for this vertex. */
			    }
			} /* if( edge not blocked ) */

			h_edge_ptr = h_edge_ptr->next_out;
		    } /* for(;;) */
		    
		    next_out[u] = h_edge_ptr;

		} /* for( each affected vertex u in the current layer ) */
		
		/* If the flow reached t, or could not be pushed any further
		 * then stop pushing.
		 */
		if(tos1 == to_i || visited[t]) break;
		/* else */

		/* Prepare to push flow through the next layer. */
		from_i = to_i;
		to_i = tos1;
	    } /* for (;;)  (next forward layer) */

          /*** Rebalance the forward-most excess flow back one layer. ***/

	    /* Reset all visited vertices to unvisited. */
	    for(i = 0; i < tos1; i++) {
                visited[stack1[i]] = 0;
	    }
	    
	    /* Back up until the forward-most layer with excess is found. */
	    do {
                tos3--;
	    } while(stack3[tos3] < 0);
	    tos1 = 0;

	    /* Get vertices in the forward-most layer with excess flow. */
	    v = stack3[tos3];
	    do {
		stack1[tos1++] = v;
		v = stack3[--tos3];
	    } while(v > 0);

	    /* There is no need to attempt rebalancing at s. */
	    if(tos3 == 0) break;  /* Flow is maximal. */

	    /* For each affected vertex, rebalance excess flow by pushing it
	     * back one layer.
	     */
	    for(i = 0; i < tos1; i++) {		
                w = stack1[i];

		/* Use as many edges from w's stack as needed to rebalance. */
		list_item = edge_stack[w];
                    /* There will be least one edge on w's stack. */
		do {
		    h_edge_ptr = list_item->edge;
		    u = h_edge_ptr->source_no;
		    if(!excess[u]) stack3[tos3++] = u;
		    
		    if(excess[w] > h_edge_ptr->flow) {
			excess[w] -= h_edge_ptr->flow;
			excess[u] += h_edge_ptr->flow;
			h_edge_ptr->flow = 0;
		    }
		    else {
			h_edge_ptr->flow -= excess[w];
			excess[u] += excess[w];
			excess[w] = 0;
			break;  /* No flow left for this vertex. */
		    }
		    list_item = list_item->next;
		} while(list_item != NULL);

		/* Block all incoming edges of w. */
		h_edge_ptr = hvertices[w].in_head;
		do {
		    h_edge_ptr->blocked = 1;
		    h_edge_ptr = h_edge_ptr->next_in;
		} while(h_edge_ptr != NULL);
	    }

	    /* Continue by pushing the rebalanced flow. */
	} /* for(;;) */
	
	/* After the main loop above exits, the maximal flow in the layered
	 * network has been computed.
	 */

	/* Free all items on each vertex's edge stack. */
	for(i = 0; i < n; i++) {
	    next_item = edge_stack[i];
	    while(next_item != NULL) {
		list_item = next_item;
		next_item = list_item->next;
                free(list_item);
	    }
	}

	/* Use the flow in the layered network to increase the flow in the
	 * main network, and reset the layered network to blank.
	 */
        lnetworkb_flush(h);
    } /* for(;;) */
    
  calculation_complete:
    
    /* The maximum flow has been computed.  Free the arrays that were allocated
     * and exit.
     */
    free(next_out);
    free(visited);
    free(used);
    free(stack1);
    free(stack2);
    free(stack3);
    free(delta);
    lnetworkb_free(h);
    free(excess);
    free(edge_stack);
}

/*** --- End of Karzanov Maximum Flow Algorithm ---------------------------***/



/*** --- Random Network Functions ---------------------------------------- ***/

/* Structure type for the breadth first search edge queue used by the graph
 * generating algorithm and graph printing algorithm.
 */
typedef struct queue_node {
    struct queue_node *next;
    int vertex;
} queue_node_t;

/* This algorithm generates an almost random network with n vertices and m
 * edges, with edge capacities uniformly distributed between min_cap and
 * max_cap.  The time complexity of the algorithm is  at worst O(mn), and the
 * algorithm uses O(m) space.  All edge flows in the network are initialised to
 * zero.
 *
 * It is up to the user of this function to use values of m between n-1 and
 * n*(n-1).
 */
network_t *network_rand(int n, int m, int min_cap, int max_cap)
{
    int i, j, k;  /* indexing */
    int v;  /* vertex */
    
    int sources_left;
    int source_no, dest_no;
    int delta_cap;

    queue_node_t qhead_node;    /* queue's head node */ 
    queue_node_t *qtail, *qptr; /* queue node pointers */

    network_t *g;                             /**/
    network_vertex_t *vertex;                 /**/ /* Pointer Variables */
    network_edge_t *e, *edge_ptr, *last_edge; /**/

    network_vertex_t *vertices; /**/
    int *dests_left;            /**/ /* Arrays */
    int *visited;               /**/
    unsigned char *used;        /**/

    /* Range for random edge capacities. */
    delta_cap = max_cap-min_cap;

    /* Initialise the range of source and destination vertex numbers left. */
    sources_left = n;
    dests_left = malloc(n * sizeof(int));
    for(i = 0; i < n; i++) {
	/* There are only n-1 edges since we will not allow and edge from a
	 * vertex to itself.
	 */
        dests_left[i] = n-1;
    }
    used = malloc(n);

    /* Initialise the breadth-first-search edge queue to empty. */
    qtail = &qhead_node;
    qhead_node.next = NULL;

    /* Create the empty network. */
    g = malloc(sizeof(network_t));
    g->n = n;
    vertices = g->vertices = malloc(n * sizeof(network_vertex_t));
    g->first_edge = NULL;

    /* Initialise the network to contain no edges. */
    for(i = 0; i < n; i++) {
        vertex = &vertices[i];
	vertex->in_head = vertex->in_tail =
	    vertex->out_head = vertex->out_tail = NULL;
    }
    
    /*** Create m unique random edges. ***/

    /* A chain of n-1 edges is created separately, beginning with the first
     * edge of the chain.  This ensures that all vertices in the graph are
     * connected.
     */
    source_no = 0;
    dest_no = 1;
    if(n == 2) sources_left = 1;  /* Special case. */
    g->first_edge = last_edge = e = malloc(sizeof(network_edge_t));
    e->source_no = 0;
    e->dest_no = 1;
    e->capacity = min_cap + rand() % delta_cap;
    e->flow = 0;    
    for(;;) {
        /* Add to the source vertex's OUT set list. */
	vertex = &vertices[source_no];
	vertex->out_head = e;
	vertex->out_tail = e;	
	e->next_out = NULL;

        /* Add to the destination vertex's IN set list. */   
	vertex = &vertices[dest_no];
	vertex->in_head = e;
	vertex->in_tail = e;
	e->next_in = NULL;

        /* Update the range of source and destination vertex numbers left. */
	dests_left[source_no]--;

        /* If the edge chain is complete, then exit this loop. */
        source_no++;
	dest_no++;
        if(dest_no >= n) break;
	/* else */

	/* Create the next edge in the chain. */
        e = malloc(sizeof(network_edge_t));
        e->source_no = source_no;
        e->dest_no = dest_no;
        e->capacity = min_cap + rand() % delta_cap;
        e->flow = 0;

	/* Add to the list of all edges. */
	last_edge = last_edge->next = e;	
    }

    /* Create the remaining edges. */
    j = n - 1;
    while(j < m) {

	/* Choose a random values from the range of source and destination
	 * vertex numbers available.
	 */
	source_no = rand() % sources_left;

	/* Adjust the source number according to unused vertices.  A vertex is
	 * `used' if it has all n-1 possible edges in its OUT set.  Variable
	 * k counts the number of unused vertex numbers.
	 */
	k = 0;
        for(i = 0; i < n; i++) {
            if(dests_left[i] != 0) k++;
	    if(k > source_no) break;
	}
	source_no = i;
	
	/* Determine which edges already exist. */
	memset(used, 0, n);
        vertex = &vertices[source_no];
	edge_ptr = vertex->out_head;
	while(edge_ptr != NULL) {
            used[edge_ptr->dest_no] = 1;
	    edge_ptr = edge_ptr->next_out;
	}
	used[source_no] = 1;

	dest_no = rand() % dests_left[source_no];

	/* Adjust the destination number according to unused edges.  Variable k
	 * counts the number of unused edges.
	 */
	k = 0;
	for(i = 0; i < n; i++) {
	    if(used[i] == 0) k++;
	    if(k > dest_no) break;
	}
	dest_no = i;

	/* Update the range of source and destination vertex numbers left. */
	dests_left[source_no]--;
	if(dests_left[source_no] == 0) sources_left--;

	/* Create the edge. */
	e = malloc(sizeof(network_edge_t));
	e->source_no = source_no;
	e->dest_no = dest_no;
	e->capacity = min_cap + rand() % delta_cap;
	e->flow = 0;

	/* Add to the list of all edges. */
	last_edge = last_edge->next = e;

	/* Add the edge to the source vertex's OUT set list. */
	if(vertex->out_head) {
	    vertex->out_tail->next_out = e;
        }
	else vertex->out_head = e;
	vertex->out_tail = e;	
	e->next_out = NULL;

	/* Add the edge to the destination vertex's IN set list. */
	vertex = &vertices[dest_no];
	if(vertex->in_tail) {
	    vertex->in_tail->next_in = e;
	}
	else vertex->in_head = e;
	vertex->in_tail = e;
	e->next_in = NULL;

	j++;
    }

    /* Terminate the list of all edges. */
    last_edge->next = NULL;
    g->last_edge = last_edge;
    
    /* Initialise breath first search from the source vertex (vertex 0). */
    visited = calloc(n, sizeof(int));
    g->s = 0;  /* source vertex */
    qptr = malloc(sizeof(queue_node_t));
    qptr->next = NULL;  qptr->vertex = 0;
    qtail = qtail->next = qptr;

    /* All vertices are reachable from the source vertex.  For the sink vertex
     * use the `furtherest' vertex in a breadth first search from the source.
     */
    visited[0] = 1;
    while(qhead_node.next != NULL) {
	qptr = qhead_node.next;
	qhead_node.next = qptr->next;
	edge_ptr = vertices[v = qptr->vertex].out_head;
	free(qptr);
	if(qhead_node.next == NULL) qtail = &qhead_node;
	    
	while(edge_ptr != NULL) {
	    if(!visited[edge_ptr->dest_no]) {
		visited[edge_ptr->dest_no] = 1;
		qptr = malloc(sizeof(queue_node_t));
		qptr->next = NULL; qptr->vertex = edge_ptr->dest_no;
		qtail = qtail->next = qptr;
	    }
	    edge_ptr = edge_ptr->next_out;
	}
    }	
    g->t = v;

    /* Free space used by temporary arrays. */
    free(visited);
    free(used);

    return g;
}

/* Reset all edge flows in the network pointed to by g to zero. */
void network_reset(network_t *g)
{
    network_edge_t *edge_ptr;

    edge_ptr = g->first_edge;

    while(edge_ptr != NULL) {
	edge_ptr->flow = 0;
	edge_ptr = edge_ptr->next;
    }
}

/* Free space which is used by the graph pointed to by g, including space used
 * by the edges of g.
 */
void network_free(network_t *g)
{
    network_edge_t *edge_ptr, *next_edge_ptr;

    next_edge_ptr = g->first_edge;

    while(next_edge_ptr != NULL) {
	edge_ptr = next_edge_ptr;
	next_edge_ptr = edge_ptr->next;
	free(edge_ptr);
    }

    free(g->vertices);
    free(g);
}

/* This dumps a text representation of the network pointed to by g to the
 * standard output.
 */
void network_print(network_t *g)
{
    int n;
    int v;

    network_edge_t *edge_ptr;
    queue_node_t qhead_node, *qtail, *qptr;

    network_vertex_t *vertices;
    int *visited;

    
    qtail = &qhead_node;
    qhead_node.next = NULL;
    
    n = g->n;
    vertices = g->vertices;

#if OUTPUT_GRAPH_LANG
    printf("digraph mfnetwork {\n");
    printf("    v%d [shape=box];\n", g->s);
    printf("    v%d [shape=box];\n", g->t);
#else
    printf("s = %d, t = %d\n", g->s, g->t);
#endif
    
    /* Print the edges of the graph as traversed in a breadth first search
     */
    
    visited = calloc(n, sizeof(int));
    qptr = malloc(sizeof(queue_node_t));
    qptr->next = NULL;  qptr->vertex = g->s;
    qtail = qtail->next = qptr;
    visited[0] = 1;

    while(qhead_node.next != NULL) {
	qptr = qhead_node.next;
	qhead_node.next = qptr->next;
	edge_ptr = vertices[v = qptr->vertex].out_head;
	free(qptr);
	if(qhead_node.next == NULL) qtail = &qhead_node;

#if OUTPUT_GRAPH_LANG	
        printf("    v%d -> {", v);
#else
        printf("[%d]:", v);
#endif
	
	while(edge_ptr) {
	    if(!visited[edge_ptr->dest_no]) {
		visited[edge_ptr->dest_no] = 1;
		qptr = malloc(sizeof(queue_node_t));
		qptr->next = NULL; qptr->vertex = edge_ptr->dest_no;
		qtail = qtail->next = qptr;
	    }

#if OUTPUT_GRAPH_LANG	    
	    printf(" v%d", edge_ptr->dest_no);
#else
	    printf("  (%d)=%d/%d", edge_ptr->dest_no, edge_ptr->flow,
		                   edge_ptr->capacity);
#endif
	    
	    edge_ptr = edge_ptr->next_out;
	}
	
#if OUTPUT_GRAPH_LANG
	printf(" };\n");
#else
	putchar('\n');
#endif
	
    }
    
#if OUTPUT_GRAPH_LANG
    printf("}\n");
#endif
}

/* This function returns the total flow going through the network. */
int network_flow(network_t *g)
{
    int flow;
    network_edge_t *edge_ptr;

    /* Sum up all flow leaving the source vertex. */
    flow = 0;
    edge_ptr = g->vertices[g->s].out_head;
    while(edge_ptr != NULL) {
        flow += edge_ptr->flow;
	edge_ptr = edge_ptr->next_out;
    }

    return flow;
}

/*** --- End of Random Network Functions --------------------------------- ***/
