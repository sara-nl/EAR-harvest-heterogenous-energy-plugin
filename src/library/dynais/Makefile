all:
	gcc -O2 -march=native -I ../.. -c dynais.c
	gcc -O2 -march=native -I ../.. -o main main.c dynais.o

.c.o :
	$(CC) $(CFLAGS) -march=native -c -o $@ $<

clean:
	rm -rf *.o
