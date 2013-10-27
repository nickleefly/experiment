#
# Makefile for Array Search Algorithms
#
CFLAGS = -Wall -O

#--- Overall Compilations ---#

# All compilations done by this makefile.
all: build_search_test

# Shared files need to be compiled separately.
shared:
	cd ../timing; $(MAKE)

#--- Programs; search_test ---#

# Linked with some shared code.
build_search_test: shared search_test

# Link
search_test: search_test.o search.o ../timing/timing.o
	$(LINK.c) -o search_test search_test.o search.o ../timing/timing.o -lm

# Compile
search_test.o: search_test.c search.h ../timing/timing.h

#--- Search Algorithms ---#

# Compile
search.o: search.c search.h

#--- Cleaning ---#

clean:
	rm -f *.o
cleanbin:
	rm -f search_test
