CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)

######## FILES

OBJS = string_enhanced.o \
       math_operations.o \
       environment.o \
       environment_common.o \
       file_lock.o \
       types.a

ifdef DB_BASE
OBJS += database.a
endif

database_OBJS = $(patsubst %.c, %.o, $(wildcard ./database/*.c))

types_SRCS = $(shell find ./types/ -type f -name '*.c' -o -name '*.h')

types_OBJS = $(shell find ./types/ -type f -name '*.o') \
             string_enhanced.o \
             math_operations.o

######## RULES

all: all.components $(OBJS)

file_lock.o: file_lock.c file_lock.h
	$(CC) $(CFLAGS) -c file_lock.c

environment.o: environment.c environment.h environment_common.h
	$(CC) $(CFLAGS) -c environment.c

environment_common.o: environment_common.c environment_common.h
	$(CC) $(CFLAGS) -c environment_common.c

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c string_enhanced.c

math_operations.o: math_operations.c math_operations.h
	$(CC) $(CFLAGS) -c math_operations.c

database.a: $(database_OBJS)
	$(AR) rvs database.a $(database_OBJS)

types.a: $(types_SRCS)
	$(AR) rvs types.a $(types_OBJS)

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
