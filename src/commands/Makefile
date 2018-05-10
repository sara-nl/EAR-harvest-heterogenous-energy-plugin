CFLAGS    = -I$(SRCDIR) 
LDFLAGS   = $(CPUF_CFLAGS) $(PAPI_LDFLAGS)

######## FILES

FREQ_OBJS = $(SRCDIR)/metrics/papi/generics.o \
            $(SRCDIR)/control/frequency.o

REMOTE_OBJS=$(SRCDIR)/common/remote_daemon_client.o


#BINARIES = ear_control ear_set_node_freq ear_get_node_freq ear_report ear_new_job ear_end_job ear_energy_over_time

TYPES = $(SRCDIR)/common/types.a
DB_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o


BINARIES = ear_set_node_freq ear_get_node_freq ear_report ear_create_database ear_new_job ear_end_job ear_energy_over_time ear_store_database

######## RULES

all: $(BINARIES)

ear_control:ear_control.c $(SRCDIR)/common/remote_daemon_client.o 
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(SRCDIR)/common/remote_daemon_client.o

ear_set_node_freq:ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq:ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_report: ear_report.c $(TYPES) $(DB_OBJS)
	$(CC) $(CFLAGS) $(DB_OBJS) $(TYPES) $(DB_CFLAGS) -o ear_report ear_report.c $(DB_LDFLAGS)

ear_new_job:ear_new_job.c $(REMOTE_OBJS) $(SRCDIR)/common/types.a
	$(CC) $(CFLAGS) -o ear_new_job ear_new_job.c  $(REMOTE_OBJS) $(SRCDIR)/common/types.a

ear_end_job:ear_end_job.c $(REMOTE_OBJS) $(SRCDIR)/common/types.a
	$(CC) $(CFLAGS) -o ear_end_job ear_end_job.c $(REMOTE_OBJS) $(SRCDIR)/common/types.a
	
ear_create_database: ear_create_database.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS)

ear_energy_over_time: ear_energy_over_time.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_energy_over_time ear_energy_over_time.c $(DB_LDFLAGS)

ear_store_database: ear_store_database.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_store_database ear_store_database.c $(DB_LDFLAGS)

######## OPTIONS

install:
	cp $(BINARIES) $(DESTDIR)/bin

clean:
	rm -rf $(BINARIES)
