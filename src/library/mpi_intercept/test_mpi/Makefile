CC = mpicc

all: clean
	
	$(CC) -Wall -Werror -fPIC -shared -D_GNU_SOURCE -o test0.so test0.c -ldl
	$(CC) -Wall -Werror -fPIC -shared -D_GNU_SOURCE -o test1.so test1.c symplug.c -ldl
	#$(CC) -Wall -Werror -fPIC -shared -D_GNU_SOURCE -o test2.so test2.c -ldl
	$(CC) -Wall -Werror -o main main.c test0.so

clean:
	rm -f *.o *.a *.so main
