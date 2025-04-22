# C compiler
CC = gcc

# options
CFLAGS = -O2 -march=native -maes -msse2 -msse

all: main

# compile test cases for LSTable
main:
	$(CC) $(CFLAGS) ciphers/lstable.c test/test_framework.c main.c ciphers/pc16.c aes_software/aes.c aes_software/gmult.c ciphers/space16.c -o main

# remove all targets
clean:
	rm -f main