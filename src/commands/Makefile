CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## DEPENDENCIES

HEADERS = $(SRCDIR)/common/config.h \
          $(SRCDIR)/common/states.h

ifdef DB_BASE
EREP_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o \
          $(SRCDIR)/common/types.a
endif

######## FILES
		
ifdef DB_BASE
BINARIES = ereport 

SBINARIES += ear_create_database \
             ear_energy_over_time \
             ear_store_database
endif

######## RULES

all: $(BINARIES) $(SBINARIES) $(COMPONENTS)

ereport: ear_report.c $(EREP_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ereport ear_report.c $(DB_LDFLAGS) $(EREP_OBJS)

ear_create_database: ear_create_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS)

ear_energy_over_time: ear_energy_over_time.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_energy_over_time ear_energy_over_time.c $(DB_LDFLAGS)

ear_store_database: ear_store_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_store_database ear_store_database.c $(DB_LDFLAGS)

######## OPTIONS

install:
ifdef DB_BASE
	@ cp $(BINARIES) $(DESTDIR)/bin/
	@ cp $(SBINARIES) $(DESTDIR)/sbin/
endif

clean:
	rm -f $(BINARIES) $(SBINARIES)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend