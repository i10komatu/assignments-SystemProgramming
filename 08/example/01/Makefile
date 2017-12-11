CFLAGS = -g -DDEBUG
LDFLAGS = -pthread

TARGET = deadlock
OBJS = deadlock.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(LDFLAGS)

deadlock.o: deadlock.c
	$(CC) -g -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)

.PHONY: validate
validate: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) -d > output.log 2> helgrind.log