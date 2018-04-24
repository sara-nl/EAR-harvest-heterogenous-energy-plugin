CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)
export CFLAGS

####### 

export DIR

######## FILES

TYPES_OBJS = types/application.o \
             types/projection.o \
             types/coefficient.o \
             types/log.o

BINARIES = ear_daemon_client.o \
           environment.o \
           string_enhanced.o \
           math_operations.o \
           shared_configuration.o \
           remote_daemon_client.o \
           file_lock.o \
           types.a

######## RULES

all: $(BINARIES)

%.o: %.c
	@ $(MAKE) -C `dirname $@` `basename $@` || exit

file_lock.o: file_lock.c file_lock.h
	$(CC) $(CFLAGS) -c file_lock.c

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

shared_configuration.o: shared_configuration.c shared_configuration.h ear_verbose.h
	$(CC) $(CFLAGS) -c shared_configuration.c

remote_daemon_client.o: remote_daemon_client.c remote_conf.h ear_verbose.h
	$(CC) $(CFLAGS) -c remote_daemon_client.c

environment.o: environment.c environment.h environment_common.h
	$(CC) $(CFLAGS) -c environment.c

environment_common.o: environment_common.c environment_common.h
	$(CC) $(CFLAGS) -c environment_common.c

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c string_enhanced.c

math_operations.o: math_operations.c math_operations.h
	$(CC) $(CFLAGS) -c math_operations.c

types.a: $(TYPES_OBJS)
	ar rvs types.a $(TYPES_OBJS) string_enhanced.o	

######## OPTIONS

install:
	mkdir -p $(DESTDIR)/include/common
	cp ear_daemon_client.o $(DESTDIR)/lib
	cp remote_daemon_client.o $(DESTDIR)/lib
	cp remote_daemon_client.h $(DESTDIR)/include
	cp eards_api.h $(DESTDIR)/include
	cp states.h $(DESTDIR)/include/common

clean:
	rm -rf *.o *.so *.a
	rm $(BINARIES) $(TYPES_OBJS)
