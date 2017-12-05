CC = gcc
LDFLAGS = -pthread

TARGET = server
OBJS = logutil.o server.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

logutil.o: logutil.c
	$(CC) -c -o $@ $< $(LDFLAGS)

server.o: server.c
	$(CC) -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)

.PHONY: validate
validate: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) -d > output.log 2> helgrind.log