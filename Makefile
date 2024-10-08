# C compiler
CC = gcc

# options
CFLAGS = -O2 -march=native -maes -msse2 -msse

all: test_lstable8 test_lstable12 test_lstable16 test_space16 test_pc16

# compile test cases for LSTable
test_lstable8: LSTable8.c
	$(CC) $(CFLAGS) LSTable8.c -o test_lstable8

test_lstable12: LSTable12.c
	$(CC) $(CFLAGS) LSTable12.c -o test_lstable12

test_lstable16: LSTable16.c
	$(CC) $(CFLAGS) LSTable16.c -o test_lstable16

# compile test cases for existing ciphers
test_space16: SPACE16.c
	$(CC) $(CFLAGS) SPACE16.c -o test_space16

test_pc16: PC16.c
	$(CC) $(CFLAGS) PC16.c aes_software/aes.c aes_software/gmult.c -o test_pc16

# remove all targets
clean:
	rm -f test_lstable8 test_lstable12 test_lstable16 test_space16 test_pc16