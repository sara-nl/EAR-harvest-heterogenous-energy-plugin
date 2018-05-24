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
				types/cluster_conf.h

HEADERS = $(SRCDIR)/common/ear_verbose.h \
          $(SRCDIR)/common/states.h \
          $(SRCDIR)/common/config.h

BINARIES = ear_daemon_client.o \
           environment.o \
           environment_common.o \
           string_enhanced.o \
           math_operations.o \
           file_lock.o \
		   types.a

ifdef FEAT_SHRMEM
BINARIES += remote_daemon_client.o \
            shared_configuration.o
endif

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

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_daemon_common.h $(HEADERS)
	$(CC) $(CFLAGS) -c ear_daemon_client.c

shared_configuration.o: shared_configuration.c shared_configuration.h $(HEADERS)
	$(CC) $(CFLAGS) -c shared_configuration.c

remote_daemon_client.o: remote_daemon_client.c remote_daemon_client.h remote_conf.h $(HEADERS)
	$(CC) $(CFLAGS) -c remote_daemon_client.c

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
	cp ear_daemon_client.o $(DESTDIR)/lib
	cp eards_api.h $(DESTDIR)/include
	cp states.h $(DESTDIR)/include/common
ifdef FEAT_SHRMEM
	cp remote_daemon_client.o $(DESTDIR)/lib
	cp remote_daemon_client.h $(DESTDIR)/include
endif

clean:
	rm $(BINARIES) $(TYPES_OBJS) || true
	rm -rf *.o *.so *.a
