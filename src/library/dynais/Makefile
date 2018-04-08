.c.o :
	$(CC) $(CFLAGS) -qopt-prefetch -axMIC-AVX512 -O3 -c -funroll-loops -o $@ $<

clean:
	rm -rf *.o
