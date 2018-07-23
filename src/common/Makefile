CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)

######## FILES

OBJS = environment.o \
       environment_common.o \
       file_lock.o \
       math_operations.o \
       sockets.o \
       string_enhanced.o \
       types.a

ifdef DB_BASE
OBJS += database.a
endif

database_OBJS = $(patsubst %.c, %.o, $(wildcard ./database/*.c))

types_SOURCES = $(shell find ./types/ -type f -name '*.c' -o -name '*.h')

types_OBJECTS = $(shell find ./types/ -type f -name '*.o') \
                string_enhanced.o \
                math_operations.o

######## RULES

all: all.components $(OBJS)

environment.o: environment.c environment.h environment_common.h
	$(CC) $(CFLAGS) -c environment.c

environment_common.o: environment_common.c environment_common.h
	$(CC) $(CFLAGS) -c environment_common.c

file_lock.o: file_lock.c file_lock.h
	$(CC) $(CFLAGS) -c file_lock.c

math_operations.o: math_operations.c math_operations.h
	$(CC) $(CFLAGS) -c math_operations.c

sockets.o: sockets.c sockets.h
	$(CC) $(CFLAGS) -c sockets.c

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c string_enhanced.c

database.a: $(database_OBJS)
	$(AR) rvs database.a $(database_OBJS)

types.a: $(types_SOURCES)
	$(AR) rvs types.a $(types_OBJECTS)

######## OPTIONS

install: install.components
	cp states.h $(DESTDIR)/include/common

clean: clean.components
	rm -f *.o *.a *.so

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'types' $* || exit
ifdef DB_BASE
	@ $(MAKE) -C 'database' $* || exit
endif

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend