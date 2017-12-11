CFLAGS = -g -DDEBUG
LDFLAGS = -pthread

TARGET = threadsafe_list
OBJS = threadsafe_list.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(LDFLAGS)

threadsafe_list.o: threadsafe_list.c
	$(CC) -g -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET)

.PHONY: distclean
distclean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)

.PHONY: validate
validate: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) > output.log 2> helgrind.log