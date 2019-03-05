DEBUG   =
CFLAGS  = -I$(SRCDIR) $(DEBUG)
LDFLAGS = -lm

######## FILES

FREQ_OBJS = \
            $(SRCDIR)/control/frequency.o \
            $(SRCDIR)/metrics/libmetrics.a 

REMOTE_OBJS = $(SRCDIR)/daemon/eard_rapi.o

TYPES_OBJS = $(SRCDIR)/common/types.a

BINARIES = ear_set_node_freq \
           ear_get_node_freq \
           ear_set_ondemand \
           ear_new_job \
           ear_end_job \
           ear_set_freq \
           ear_red_freq \
           ear_inc_th \
           ear_set_th \
           ear_check_node_test
#          ear_dbd_stress_test \

######## RULES

all: $(BINARIES)

ear_set_node_freq: ear_set_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS) $(LDFLAGS)

ear_set_ondemand: ear_set_ondemand.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS) $(LDFLAGS)

ear_get_node_freq: ear_get_node_freq.c $(FREQ_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(FREQ_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS) $(LDFLAGS)

ear_new_job: ear_new_job.c $(REMOTE_OBJS) $(TYPES_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_check_node_test:ear_check_node_test.c $(TYPES_OBJS) ../eard_checkpoint.o $(SRCDIR)/metrics/libmetrics.a
	$(CC) $(CFLAGS) $(PAPI_LDFLAGS) $(IPMI_LDFLAGS) -o $@ $<   ../eard_checkpoint.o $(SRCDIR)/metrics/libmetrics.a $(TYPES_OBJS) $(LDFLAGS)

ear_set_freq: ear_set_freq.c $(REMOTE_OBJS) $(TYPES_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_red_freq: ear_red_freq.c $(REMOTE_OBJS) $(TYPES_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_inc_th: ear_inc_th.c $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_set_th: ear_set_th.c $(REMOTE_OBJS) $(TYPES_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_end_job: ear_end_job.c $(REMOTE_OBJS) $(TYPES_OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(REMOTE_OBJS) $(TYPES_OBJS) $(LDFLAGS)

ear_dbd_stress_test:ear_dbd_stress_test.c $(TYPES_OBJS) $(SRCDIR)/database_cache/eardbd_api.o $(SRCDIR)/common/sockets.o
	$(CC) $(CFLAGS) -o $@  $<  $(TYPES_OBJS) $(DB_LDFLAGS) $(SRCDIR)/database_cache/eardbd_api.o $(SRCDIR)/common/sockets.o $(LDFLAGS)

######## OPTIONS

install:

devel.install: ;
	cp $(BINARIES) $(DESTDIR)/bin

%.install: ;

clean:
	rm -f $(BINARIES)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
