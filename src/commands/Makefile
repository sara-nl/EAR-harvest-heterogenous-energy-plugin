CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## DEPENDENCIES

FREQ_OBJS   = $(SRCDIR)/metrics/papi/generics.o \
              $(SRCDIR)/control/frequency.o

REMOTE_OBJS = $(SRCDIR)/daemon/eard_command_api.o

TYPES_OBJS  = $(SRCDIR)/common/types.a

ifdef DB_BASE
DB_OBJS     = $(SRCDIR)/common/database/mysql_io_functions.o
endif

HEADERS     = $(SRCDIR)/common/config.h \
              $(SRCDIR)/common/states.h

######## FILES

		
ifdef DB_BASE
BINARIES = ereport 
SBINARIES += ear_create_database \
             ear_energy_over_time \
             ear_store_database

endif

######## RULES

all: $(BINARIES) $(SBINARIES) $(COMPONENTS)

ear_control: ear_control.c $(REMOTE_OBJS)
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(REMOTE_OBJS) $(TYPES_OBJS)

ereport: ear_report.c $(DB_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ereport ear_report.c $(DB_LDFLAGS) $(DB_OBJS) $(TYPES_OBJS)


ear_create_database: ear_create_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS)

ear_energy_over_time: ear_energy_over_time.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_energy_over_time ear_energy_over_time.c $(DB_LDFLAGS)

ear_store_database: ear_store_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_store_database ear_store_database.c $(DB_LDFLAGS)


######## OPTIONS

install:
	@ cp $(BINARIES) $(DESTDIR)/bin
	@ cp $(SBINARIES) $(DESTDIR)/sbin

clean:
	rm -rf $(BINARIES) $(SBINARIES)
