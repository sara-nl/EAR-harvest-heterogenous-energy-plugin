CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## FILES

FREQ_OBJS = $(SRCDIR)/metrics/papi/generics.o \
            $(SRCDIR)/control/frequency.o

REMOTE_OBJS = $(SRCDIR)/daemon/eard_rapi.o

TYPES_OBJS = $(SRCDIR)/common/types.a

HEADERS = $(SRCDIR)/common/config.h \
          $(SRCDIR)/common/states.h

BINARIES = ear_set_node_freq \
           ear_get_node_freq \
		   ear_set_ondemand 
		

ifdef FEAT_SHRMEM
BINARIES += ear_new_job \
            ear_end_job \
			ear_set_freq \
			ear_red_freq \
			ear_inc_th \
			ear_set_th

endif

######## RULES

all: $(BINARIES)


ear_set_node_freq: ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_node_freq ear_set_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_set_ondemand: ear_set_ondemand.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_set_ondemand ear_set_ondemand.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq: ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o ear_get_node_freq ear_get_node_freq.c $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)


ear_new_job: ear_new_job.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_new_job ear_new_job.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_set_freq: ear_set_freq.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_set_freq ear_set_freq.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_red_freq: ear_red_freq.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_red_freq ear_red_freq.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_inc_th: ear_inc_th.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_inc_th ear_inc_th.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_set_th: ear_set_th.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_set_th ear_set_th.c  $(REMOTE_OBJS) $(TYPES_OBJS)

ear_end_job: ear_end_job.c $(REMOTE_OBJS) $(TYPES_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o ear_end_job ear_end_job.c $(REMOTE_OBJS) $(TYPES_OBJS)


######## OPTIONS

install:
	cp $(BINARIES) $(DESTDIR)/bin

clean:
	rm -rf $(BINARIES)
