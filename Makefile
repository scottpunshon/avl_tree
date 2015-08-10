
all: avl_tree.o

clean:
	rm -f *.o

.PHONY: all clean

avl_tree.c: $(@:.c=.h)

avl_tree.o: avl_tree.c
	gcc -c -o $@ $^
