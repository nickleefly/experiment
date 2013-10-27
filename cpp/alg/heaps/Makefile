#
# Makefile for Heaps
#
CFLAGS = -g -Wall -ansi

#--- Overall Compilations ---#

# All compilations done by this makefile.
all: bheap.o fheap.o ttheap.o triheap.o triheap_ext.o

#--- Individual Heaps ---#

# Compile
bheap.o: bheap.c bheap.h heap_info.h
fheap.o: fheap.c fheap.h heap_info.h
ttheap.o: ttheap.c ttheap.h heap_info.h
triheap.o: triheap.c triheap.h heap_info.h
triheap_ext.o: triheap_ext.c triheap_ext.h heap_info.h

#--- Cleaning ---#

clean:
	rm -f *.o
