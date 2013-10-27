#
# Makefile for Array Sorting Algorithms
#
CFLAGS = -Wall -O3

#--- Overall Compilations ---#

# All compilations done by this makefile.
all: build_sort_test

# Shared files need to be compiled separately.
shared:
	cd ../timing; $(MAKE)

#--- Programs; sort_test ---#

# Linked with some shared code.
build_sort_test: shared sort_test

# Link
sort_test: sort_test.o sort.o ../timing/timing.o
	$(LINK.c) -o sort_test sort_test.o sort.o ../timing/timing.o

# Compile
sort_test.o: sort_test.c sort.h ../timing/timing.h

#--- Sorting Algorithms ---#

# Compile
sort.o: sort.c sort.h

#--- Cleaning ---#

clean:
	rm -f *.o
cleanbin:
	rm -f sort_test
