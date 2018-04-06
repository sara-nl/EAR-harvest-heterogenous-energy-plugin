.c.o :
	$(CC) $(CFLAGS)  -c -o $@ $<

%_trace.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c -o $@ $<


clean:
	rm -rf *.o
