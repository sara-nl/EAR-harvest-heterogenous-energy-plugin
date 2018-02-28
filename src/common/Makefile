CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)

####### 

export DIR

######## FILES

TYPES_OBJS = types/application.o \
             types/projection.o

BINARIES = ear_daemon_client.o environment.o string_enhanced.o types.a shared_configuration.o

######## RULES

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

all: $(BINARIES)

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

shared_configuration.o: shared_configuration.c shared_configuration.h ear_verbose.h
	$(CC) $(CFLAGS) -c shared_configuration.c

environment.o: environment.c environment.h
	$(CC) $(CFLAGS) -c environment.c

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c string_enhanced.c

types.a: $(TYPES_OBJS)
	ar rvs types.a $(TYPES_OBJS) string_enhanced.o

install:
	cp ear_daemon_client.o $(DESTDIR)/lib
	cp eards_api.h $(DESTDIR)/include
	cp states.h $(DESTDIR)/include

clean:
	rm -rf *.o *.so *.a
	rm -f $(TYPES_OBJS)
