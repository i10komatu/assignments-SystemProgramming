CFLAGS = -g -DDEBUG
LDFLAGS = -pthread

TARGET = server
OBJS = threadsafe_list.o logutil.o server.o
LOGS = output.log helgrind.log

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) $(LDFLAGS)

server.o: server.c
	$(CC) -g -c -o $@ $< $(LDFLAGS)

threadsafe_list.o: threadsafe_list.c
	$(CC) -g -c -o $@ $< $(LDFLAGS)

logutil.o: logutil.c
	$(CC) -g -c -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(TARGET)

.PHONY: distclean
distclean:
	$(RM) -f $(OBJS) $(TARGET) $(LOGS)

.PHONY: validate
validate: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) -d > output.log 2> helgrind.log