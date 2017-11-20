CC = gcc
LDFLAGS = -pthread
TARGETS = test datarace

.PHONY: all
all: $(TARGETS)

test: test.c
	$(CC) -o $@ $< $(LDFLAGS)

datarace: datarace.c
	$(CC) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(TARGETS)
