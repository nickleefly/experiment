#
# Makefile for Dictionaries and Test Programs
#
CFLAGS = -O -Wall

#--- Overall Compilations ---

# All compilations
all: build_dict_test bst_test hash_test avl_test tree23_test rbtree_test skip_list_test dst_test rst_test

# Shared files need to be compiled separately.
shared:
	cd ../timing; $(MAKE)

#--- Main Test Program; dict_test ---#

# Linked with the timing code which is shared.
build_dict_test: shared dict_test

# Link
dict_test: dict_test.o rand.o bst.o avl.o tree23.o rbtree.o dst.o rst.o ../timing/timing.o
	$(LINK.c) -o dict_test dict_test.o rand.o bst.o avl.o tree23.o rbtree.o dst.o rst.o ../timing/timing.o

# Compile
dict_test.o: dict_test.c bst.h avl.h tree23.h rbtree.h dst.h rst.h dict_info.h rand.h ../timing/timing.h
rand.o: rand.c rand.h

#--- Individual Test Programs ---#

# Link
bst_test: bst_test.o bst.o
	$(LINK.c) -o bst_test bst_test.o bst.o
hash_test: hash_test.o hashtbl.o
	$(LINK.c) -o hash_test hash_test.o hashtbl.o
avl_test: avl_test.o avl.o
	$(LINK.c) -o avl_test avl_test.o avl.o -lm
tree23_test: tree23_test.o tree23.o
	$(LINK.c) -o tree23_test tree23_test.o tree23.o
rbtree_test: rbtree_test.o rbtree.o
	$(LINK.c) -o rbtree_test rbtree_test.o rbtree.o
skip_list_test: skip_list_test.o skip_list.o
	$(LINK.c) -o skip_list_test skip_list_test.o skip_list.o -lm
dst_test: dst_test.o dst.o
	$(LINK.c) -o dst_test dst_test.o dst.o
rst_test: rst_test.o rst.o
	$(LINK.c) -o rst_test rst_test.o rst.o

# Compile
bst_test.o: bst_test.c bst.h
hash_test.o: hash_test.c hashtbl.h
avl_test.o: avl_test.c avl.h
tree23_test.o: tree23_test.c tree23.h
rbtree_test.o: rbtree_test.c rbtree.h
skip_list_test.o: skip_list_test.c skip_list.h
dst_test.o: dst_test.c dst.h
rst_test.o: rst_test.c rst.h

#--- Individual Dictionaries ---#

# Compile
bst.o: bst.c bst.h dict_info.h
hashtbl.o: hashtbl.c hashtbl.h
avl.o: avl.c avl.h dict_info.h
tree23.o: tree23.c tree23.h dict_info.h
rbtree.o: rbtree.c rbtree.h dict_info.h
skip_list.o: skip_list.c skip_list.h
dst.o: dst.c dst.h dict_info.h
rst.o: rst.c rst.h dict_info.h

#--- Cleaning ---#

clean:
	rm -f *.o
cleanbin:
	rm -f dict_test bst_test hash_test avl_test tree23_test rbtree_test skip_list_test dst_test rst_test
