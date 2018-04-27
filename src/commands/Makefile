CFLAGS    = -I$(SRCDIR) 
LDFLAGS   = $(CPUF_CFLAGS) $(PAPI_LDFLAGS)

######## FILES

FREQ_OBJS = $(SRCDIR)/metrics/papi/generics.o \
            $(SRCDIR)/control/frequency.o

TYPES = $(SRCDIR)/common/types.a

BINARIES = ear_control ear_set_node_freq ear_get_node_freq ear_report ear_create_database

######## RULES

all: $(BINARIES)

ear_control:ear_control.c $(SRCDIR)/common/remote_daemon_client.o 
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(SRCDIR)/common/remote_daemon_client.o

ear_set_node_freq:ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq:ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_report: ear_report.c $(TYPES)
	$(CC) $(CFLAGS) -o ear_report ear_report.c $(TYPES)

ear_create_database: ear_create_database.c
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o ear_create_database ear_create_database.c $(DB_LDFLAGS)

######## OPTIONS

install:
	cp $(BINARIES) $(DESTDIR)/bin

clean:
	rm -rf $(BINARIES)
