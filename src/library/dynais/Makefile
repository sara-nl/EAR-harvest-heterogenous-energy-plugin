all:
	gcc -Wall -O2 -march=native -I ../.. -c dynais.c
	gcc -Wall -O2 -march=native -I ../.. -o main main.c dynais.o

.c.o :
	$(CC) $(CFLAGS) -march=native -c -o $@ $<

clean:
	rm -rf *.o
