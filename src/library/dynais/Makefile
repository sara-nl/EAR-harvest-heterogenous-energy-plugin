all:

%.o: %.c %.h $(DEPS)
	$(CC) $(CFLAGS) -march=native -c -o $@ $<

clean:
	rm -rf *.o
