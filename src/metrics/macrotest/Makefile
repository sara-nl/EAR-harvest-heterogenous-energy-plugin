CC     = /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc
CFLAGS = -I ../.. -g

all:
	$(CC) $(CFLAGS) -o flops flops.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl
	$(CC) $(CFLAGS) -o rapl_test rapl_test.c -lm
	$(CC) $(CFLAGS) -o bandwidth bandwidth.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl
	$(CC) $(CFLAGS) -o cpi cpi.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl
	$(CC) $(CFLAGS) -o rapl rapl.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl -lm

clean:
	rm -f bandwidth
	rm -f cpi
	rm -f rapl
	rm -f rapl_test
	rm -f flops
