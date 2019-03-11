CFLAGS  = -I$(SRCDIR) $(DEBUG)
LDFLAGS = $(PAPI_LDFLAGS) $(IPMI_LDFLAGS) -lm
CCC     = $(CCC)

######## INSTALLATION

coma_BINS = ear_set_node_freq \
    ear_get_node_freq \
    ear_set_ondemand \
    ear_new_job \
    ear_end_job \
    ear_set_freq \
    ear_red_freq \
    ear_inc_th \
    ear_set_th \
    ear_check_node_test
#   ear_dbd_stress_test \
coma_PATH = $(DESTDIR)/bin
coma_PERM = 0770

######## FILES

freq_OBJS = $(SRCDIR)/control/frequency.o $(mets_OBJS)

comm_OBJS = $(SRCDIR)/common/libcommon.a

remt_OBJS = $(SRCDIR)/daemon/eard_rapi.o \
    $(comm_OBJS)

mets_OBJS = ../eard_checkpoint.o \
    $(SRCDIR)/metrics/libmetrics.a \
    $(comm_OBJS)

sock_OBJS = $(SRCDIR)/common/sockets.o \
    $(SRCDIR)/database_cache/eardbd_api.o \
    $(comm_OBJS)

######## RULES

all: $(coma_BINS)

ear_set_node_freq: ear_set_node_freq.c $(freq_OBJS)
	$(CCC) -o $@ $< $(freq_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_set_ondemand: ear_set_ondemand.c $(freq_OBJS)
	$(CCC) -o $@ $< $(freq_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_get_node_freq: ear_get_node_freq.c $(freq_OBJS)
	$(CCC) -o $@ $< $(freq_OBJS) $(CPUF_LDFLAGS) $(PAPI_LDFLAGS)

ear_new_job: ear_new_job.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_check_node_test: ear_check_node_test.c $(mets_OBJS)
	$(CCC) -o $@ $< $(mets_OBJS) $(LDFLAGS)

ear_set_freq: ear_set_freq.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_red_freq: ear_red_freq.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_inc_th: ear_inc_th.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_set_th: ear_set_th.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_end_job: ear_end_job.c $(remt_OBJS)
	$(CCC) -o $@ $< $(remt_OBJS)

ear_dbd_stress_test:ear_dbd_stress_test.c $(sock_OBJS)
	$(CCC) -o $@ $< $(sock_OBJS) $(DB_LDFLAGS))

######## OPTIONS

install: ;

devel.install: coma.ginstall;

%.install: ;

clean: coma.gclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
