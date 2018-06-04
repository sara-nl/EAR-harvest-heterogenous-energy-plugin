CFLAGS = -I$(SRCDIR) -fPIC $(CONSTANTS)

######## FILES

TYPES_OBJS = types/application.o \
             types/coefficient.o \
		     types/job.o \
			 types/log.o \
	    	 types/loop.o \
			 types/periodic_metric.o \
		     types/power_signature.o \
             types/projection.o \
		     types/signature.o \
			 types/gm_warning.o \
			 types/cluster_conf.o

TYPES_HEADERS = types/application.h \
                types/coefficient.h \
                types/generic.h \
                types/job.h \
                types/log.h \
                types/loop.h \
                types/periodic_metric.h \
                types/power_signature.h \
                types/projection.h \
                types/signature.h \
				types/gm_warning.h \
				types/cluster_conf.h

HEADERS = $(SRCDIR)/common/ear_verbose.h \
          $(SRCDIR)/common/states.h \
          $(SRCDIR)/common/config.h

BINARIES = environment.o \
           environment_common.o \
           string_enhanced.o \
           math_operations.o \
           file_lock.o \
		   types.a

ifdef DB_BASE
BINARIES += database/db_helper.o \
            database/mysql_io_functions.o
endif

####### VAR PROPAGATION 

export DIR
export CFLAGS
export HEADERS

######## RULES

all: $(BINARIES)

%.o: %.c %.h $(HEADERS)
	@ $(MAKE) -C `dirname $@` `basename $@` DEPS='$(HEADERS)' || exit

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

types.a: $(TYPES_OBJS) string_enhanced.o math_operations.o $(HEADERS)
	ar rvs types.a $(TYPES_OBJS) string_enhanced.o math_operations.o

######## OPTIONS

install:
	mkdir -p $(DESTDIR)/include/common
	cp states.h $(DESTDIR)/include/common

clean:
	rm $(BINARIES) $(TYPES_OBJS) || true
	rm -f *.o *.so *.a
