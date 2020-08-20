CC     = /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc
CFLAGS = -I ../.. -g

gpu: gpu.c Makefile
	$(CC) $(CFLAGS) -fPIC -c gpu.c
	$(CC) $(CFLAGS) -rdynamic -o gpu gpu.o ../libmetrics.a ../../common/libcommon.a -lpthread -ldl
	$(CC) $(CFLAGS) -fPIC -c gpu.c -DPLUGIN=1
	$(CC) $(CFLAGS) -shared -o libplugin.so gpu.o ../../common/libcommon.a -lpthread -ldl

all:
	$(CC) $(CFLAGS) -o bandwidth bandwidth.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl
	$(CC) $(CFLAGS) -o cpi cpi.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl

clean:
	rm -f bandwidth
	rm -f cpi
