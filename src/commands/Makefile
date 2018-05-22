CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## FILES

FREQ_OBJS = $(SRCDIR)/metrics/papi/generics.o \
            $(SRCDIR)/control/frequency.o

REMOTE_OBJS = $(SRCDIR)/common/remote_daemon_client.o

TYPES_OBJS = $(SRCDIR)/common/types.a

HEADERS = $(SRCDIR)/common/config.h \
          $(SRCDIR)/common/states.h

BINARIES = ear_set_node_freq \
           ear_get_node_freq \
           ereport

ifdef FEAT_SHRMEM
BINARIES += ear_control \
            ear_new_job \
            ear_end_job
endif

ifdef DB_BASE
BINARIES += ear_create_database \
            ear_energy_over_time \
            ear_store_database

DB_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o
endif

######## RULES

all: $(BINARIES)

ear_control: ear_control.c $(REMOTE_OBJS)
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(REMOTE_OBJS) $(TYPES_OBJS)

ear_set_node_freq: ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq: ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ereport: ear_report.c $(DB_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ereport ear_report.c $(DB_LDFLAGS) $(DB_OBJS) $(TYPES_OBJS)

ear_new_job: ear_new_job.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_new_job ear_new_job.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_end_job: ear_end_job.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_end_job ear_end_job.c $(REMOTE_OBJS) $(TYPES_OBJS)

ear_create_database: ear_create_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS)

ear_energy_over_time: ear_energy_over_time.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_energy_over_time ear_energy_over_time.c $(DB_LDFLAGS)

ear_store_database: ear_store_database.c $(HEADERS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_store_database ear_store_database.c $(DB_LDFLAGS)

######## OPTIONS

install:
	cp $(BINARIES) $(DESTDIR)/bin

clean:
	rm -rf $(BINARIES)
