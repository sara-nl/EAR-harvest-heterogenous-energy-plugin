CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS) -O3

######## FILES

OBJS = environment.o \
       environment_common.o \
       file.o \
       math_operations.o \
       process.o \
       sockets.o \
       string_enhanced.o \
       user.o \
       types.a

ifdef DB_BASE
OBJS += database.a
endif

database_OBJS = $(patsubst %.c, %.o, $(wildcard ./database/*.c))

types_OBJECTS = $(shell find ./types/ -type f -name '*.o') \
                string_enhanced.o \
                math_operations.o \
                process.o \
                file.o \
                user.o

types_SOURCES = $(shell find ./types/ -type f -name '*.c' -o -name '*.h') \
                string_enhanced.c \
                string_enhanced.h \
                math_operations.c \
                math_operations.h \
                process.c \
                process.h \
                file.c \
                file.h \
                user.c \
                user.h

######## RULES

all: all.components $(OBJS)

environment.o: environment.c environment.h environment_common.h
	$(CC) $(CFLAGS) -c $<

environment_common.o: environment_common.c environment_common.h
	$(CC) $(CFLAGS) -c $<

file.o: file.c file.h
	$(CC) $(CFLAGS) -c $<

math_operations.o: math_operations.c math_operations.h
	$(CC) $(CFLAGS) -c $<

process.o: process.c process.h
	$(CC) $(CFLAGS) -c $<

sockets.o: sockets.c sockets.h
	$(CC) $(CFLAGS) -c $<

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c $<

user.o: user.c user.h
	$(CC) $(CFLAGS) -c $<

database.a: $(database_OBJS)
	$(AR) rvs $@ $(database_OBJS)

types.a: $(types_SOURCES)
	$(AR) rvs $@ $(types_OBJECTS)

######## OPTIONS

install: install.components

devel.install: devel.install.components;
	cp states.h $(DESTDIR)/include/common

%.install: %.install.components;

clean: clean.components
	rm -f *.o *.a *.so

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'types' $* || exit
ifdef DB_BASE
	@ $(MAKE) -C 'database' $* || exit
endif

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.depend
