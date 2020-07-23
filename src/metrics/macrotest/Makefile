CC     = /hpc/base/ctt/packages/compiler/gnu/9.1.0/bin/gcc
CFLAGS = -I ../.. -g

all:
	$(CC) $(CFLAGS) -o bandwidth bandwidth.c ../libmetrics.a ../../common/libcommon.a -lpthread -ldl

clean:
	rm -f bandwidth
