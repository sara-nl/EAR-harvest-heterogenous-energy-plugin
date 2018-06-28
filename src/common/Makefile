CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)

######## FILES

OBJS = environment.o \
       environment_common.o \
       file_lock.o \
       database.a \
       types.a

database_OBJS = $(patsubst %.c, %.o, $(wildcard ./database/*.c))

types_OBJS = $(patsubst %.c, %.o, $(wildcard ./types/*.c)) \
			 string_enhanced.o \
             math_operations.o

######## RULES

all: all.components $(OBJS) all.others

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

types.a: $(types_OBJS)
	$(AR) rvs types.a $(shell find ./types/ -type f -name '*.o')

######## OPTIONS

install: install.components install.others
	cp states.h $(DESTDIR)/include/common

clean: clean.components clean.others
	rm -f *.o *.a *.so

depend: depend.components depend.others

depend-clean: depend-clean.components depend-clean.others

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'types' $* || exit

%.others:
	@ $(MAKE) -C 'database' $* || exit

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
