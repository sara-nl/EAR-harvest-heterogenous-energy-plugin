all:
	icc -axCORE_AVX512 -I ../../ -c ../../library/dynais/dynais.c
	icc -I ../../ -o main main.c dynais.o
	#icc -axCORE_AVX512 -o test test.c

.c.o :
	$(CC) $(CFLAGS) -c  -o $@ $<

clean:
	rm -rf *.o
