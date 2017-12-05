CC = gcc
LDFLAGS = -pthread

TARGET = async
OBJS = async.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

async.o: async.c
	$(CC) -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)