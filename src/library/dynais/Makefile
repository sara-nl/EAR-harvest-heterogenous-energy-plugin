all:
	gcc -O3 -I ../../ -c ../../library/dynais/dynais.c
	gcc -O3 -I ../../ -o main main.c dynais.o

.c.o :
	$(CC) $(CFLAGS) -c  -o $@ $<

clean:
	rm -rf *.o
