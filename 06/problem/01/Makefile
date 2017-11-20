CC = gcc
LDFLAGS = -pthread

datarace: datarace.o
	$(CC) -o $@ $< $(LDFLAGS)

datarace.o: datarace.c
	$(CC) -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) datarace.o datarace helgrind.log

.PHONY: validate
validate: datarace
	valgrind --tool=helgrind ./datarace 2> helgrind.log