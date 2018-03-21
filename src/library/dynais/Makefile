all:
	gcc -O2 -I ../../ -c ../../library/dynais/dynais.c
	gcc -O2 -I ../../ -o main main.c dynais.o

.c.o :
	$(CC) $(CFLAGS) -c  -o $@ $<

clean:
	rm -rf *.o
