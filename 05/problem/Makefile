.PHONY: all
all: simplefs

simplefs: simplefs.o
	$(CC) simplefs.o `pkg-config fuse --libs` -o simplefs

simplefs.o: simplefs.c
	$(CC) -c simplefs.c `pkg-config fuse --cflags --libs`

.PHONY: clean
clean:
	rm -f simplefs.o simplefs

.PHONY: test
test: fstest.sh simplefs
	VERBOSE=1 sh fstest.sh ./simplefs
