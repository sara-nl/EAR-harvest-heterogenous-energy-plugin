CFLAGS  = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)
CCC     = $(CC) $(CC_FLAGS) $(CFLAGS)

######## INSTALLATION

tool_BINS = \
    coeffs_quality \
    coeffs_compute \
    coeffs_compare \
    coeffs_jobproj \
    coeffs_show \
    coeffs_default \
    coeffs_null \
    get_ear_events \
    get_active_nodes \
    learning_show \
    learning_validate \
    learning_delete
tool_PATH = $(DESTDIR)/bin/tools
tool_PERM = 0750

######## FILES

comm_OBJS = $(ROOTDIR)/src/common/types.a
dtbs_OBJS = $(ROOTDIR)/src/common/database.a

######## RULES

all: $(tool_BINS)

coeffs_compute: coeffs_compute.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -o $@ $< $(LDFLAGS) $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

coeffs_jobproj: coeffs_jobproj.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

coeffs_quality: coeffs_quality.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

coeffs_show: coeffs_show.c $(ROOTDIR)/src/common/file.o $(comm_OBJS)
	$(CCC) -I$(SRCDIR) -o $@ $< $(ROOTDIR)/src/common/file.o $(comm_OBJS)

coeffs_null: coeffs_null.c $(comm_OBJS)
	$(CCC) -I$(SRCDIR) -o $@ $< $(comm_OBJS)

coeffs_default: coeffs_default.c $(ROOTDIR)/src/common/file.o $(comm_OBJS)
	$(CCC) -I$(SRCDIR) -o $@ $< $(ROOTDIR)/src/common/file.o $(comm_OBJS)

coeffs_compare: coeffs_compare.c $(comm_OBJS)
	$(CCC) -I$(SRCDIR) -o $@ $< $(comm_OBJS)

get_ear_events: get_ear_events.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -g -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

get_active_nodes: get_active_nodes.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -g -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

learning_show: learning_show.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

learning_delete: learning_delete.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -g -o $@ $< $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

learning_validate: learning_validate.c $(dtbs_OBJS) $(comm_OBJS)
	$(CCC) -o $@ $< $(LDFLAGS) $(dtbs_OBJS) $(DB_LDFLAGS) $(comm_OBJS)

######## OPTIONS

install: tool.ginstall;

%.install: ;

clean: tool.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
