#
# Makefile for Graph Algorithms
#
CFLAGS = -Wall -O

#--- File Locations ---#

# Object files for each program.
da_test_obj = da_test.o da.o ../graphs/dgraph.o ../timing/timing.o
da_simple_obj = da_simple.o ../graphs/dgraph.o ../timing/timing.o
dfs_bfs_test_obj = dfs_bfs_test.o dfs_bfs.o ../graphs/dgraph.o
sc_test_obj = sc_test.o sc.o ../graphs/dgraph.o
mf_test_obj = mf_test.o mf.o ../timing/timing.o
mst_test_obj = mst_test.o mst.o ../graphs/dgraph.o ../timing/timing.o

# Object and header files for each heap.
heap_obj = ../heaps/bheap.o ../heaps/fheap.o ../heaps/ttheap.o ../heaps/triheap.o ../heaps/triheap_ext.o
heap_h = ../heaps/bheap.h ../heaps/fheap.h ../heaps/ttheap.h ../heaps/triheap.h ../heaps/triheap_ext.h

#--- Overall Compilations ---#

# All compilations done by this makefile.
all: build_da_test build_da_simple build_dfs_bfs_test build_sc_test build_mf_test build_mst_test

# Shared files need to be compiled separately.
shared_graphs:
	cd ../graphs; $(MAKE) dgraph.o
shared_heaps:
	cd ../heaps; $(MAKE)
shared_timing:
	cd ../timing; $(MAKE) timing.o

#--- Programs; da_test, da_simple, dfs_bfs_test, sc_test, mst_test ---#

# Linked with some shared code.
build_da_test: shared_graphs shared_heaps shared_timing da_test
build_da_simple: shared_graphs shared_timing da_simple
build_dfs_bfs_test: shared_graphs dfs_bfs_test
build_sc_test: shared_graphs sc_test
build_mf_test: shared_timing mf_test
build_mst_test: shared_graphs shared_heaps shared_timing mst_test

# Link
da_test: $(da_test_obj) $(heap_obj)
	$(LINK.c) -o da_test $(da_test_obj) $(heap_obj) -lm
da_simple: $(da_simple_obj)
	$(LINK.c) -o da_simple $(da_simple_obj)
dfs_bfs_test: $(dfs_bfs_test_obj)
	$(LINK.c) -o dfs_bfs_test $(dfs_bfs_test_obj)
sc_test: $(sc_test_obj)
	$(LINK.c) -o sc_test $(sc_test_obj)
mf_test: $(mf_test_obj)
	$(LINK.c) -o mf_test $(mf_test_obj)
mst_test: $(mst_test_obj) $(heap_obj)
	$(LINK.c) -o mst_test $(mst_test_obj) $(heap_obj) -lm

# Compile
da_test.o: da_test.c da.h ../graphs/dgraph.h ../heaps/heap_info.h ../timing/timing.h $(heap_h)
da_simple.o: da_simple.c ../graphs/dgraph.h ../timing/timing.h
dfs_bfs_test.o: dfs_bfs_test.c dfs_bfs.h ../graphs/dgraph.h
sc_test.o: sc_test.c sc.h ../graphs/dgraph.h
mf_test.o: mf_test.c mf.h ../timing/timing.h
mst_test.o: mst_test.c mst.h ../graphs/dgraph.h ../heaps/heap_info.h ../timing/timing.h $(heap_h)

#--- Algorithms ---#

# Compile
da.o: da.c da.h ../graphs/dgraph.h ../heaps/heap_info.h ../timing/timing.h
dfs_bfs.o: dfs_bfs.c dfs_bfs.h ../graphs/dgraph.h
sc.o: sc.c sc.h ../graphs/dgraph.h
mf.o: mf.c mf.h
mst.o: mst.c mst.h ../graphs/dgraph.h ../heaps/heap_info.h

#--- Cleaning ---#

clean:
	rm -f *.o
cleanbin:
	rm -f da_test da_simple dfs_bfs_test sc_test mst_test mf_test
