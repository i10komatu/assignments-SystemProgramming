CC = gcc
LDFLAGS = -pthread

TARGET = circular_buffer
OBJS = circular_buffer.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

circular_buffer.o: circular_buffer.c
	$(CC) -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)

.PHONY: validate
validate: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) > output.log 2> helgrind.log