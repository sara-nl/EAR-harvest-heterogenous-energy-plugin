CFLAGS  = -I$(SRCDIR) 
LDFLAGS = $(CPUF_CFLAGS) $(PAPI_LDFLAGS)

FREQ_OBJS= $(SRCDIR)/metrics/papi/generics.o $(SRCDIR)/control/frequency.o
TYPES = $(SRCDIR)/common/types/application.o

BINARIES = ear_control ear_set_node_freq ear_get_node_freq ear_report.c
all: $(BINARIES)


ear_control:ear_control.c $(SRCDIR)/common/remote_daemon_client.o 
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(SRCDIR)/common/remote_daemon_client.o

ear_set_node_freq:ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq:ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_acct: ear_acct.c $(TYPES)
	$(CC) $(CFLAGS) -o ear_acct ear_acct.c $(TYPES)

install:
	cp $(BINARIES) $(DESTDIR)/bin

clean:
	rm -rf $(BINARIES)
