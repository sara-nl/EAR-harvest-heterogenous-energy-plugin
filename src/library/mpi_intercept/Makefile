.c.o:
	$(MPICC) $(CFLAGS) -c -o $@ $<

%_trace.o: %.c
	$(MPICC) $(CFLAGS) $(DEBUG) -c -o $@ $<


clean:
	rm -rf *.o
