CFLAGS  = -I$(SRCDIR) 
LDFLAGS = $(CPUF_CFLAGS) $(PAPI_LDFLAGS)

FREQ_OBJS= $(SRCDIR)/metrics/papi/generics.o $(SRCDIR)/control/frequency.o

BINARIES = ear_control ear_set_node_freq ear_get_node_freq
all: $(BINARIES)


ear_control:ear_control.c $(SRCDIR)/common/remote_daemon_client.o 
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(SRCDIR)/common/remote_daemon_client.o

ear_set_node_freq:ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq:ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

install:
	cp $(BINARIES) $(EAR_INSTALL_PATH)/bin

clean:
	rm -rf $(BINARIES)
