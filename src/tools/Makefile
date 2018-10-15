CFLAGS  = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

COMMON_OBJS = $(ROOTDIR)/src/common/types.a

BINARIES = coeffs_compute \
           coeffs_jobproj \
           coeffs_quality \
           coeffs_show \
           coeffs_comparison \
		   get_app_list \
		   get_ear_events \
		   get_active_nodes \
		   learn_validate \
		   learn_delete

all: $(BINARIES)

coeffs_compute: coeffs_compute.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coeffs_jobproj: coeffs_jobproj.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coeffs_quality: coeffs_quality.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coeffs_show: coeffs_show.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -I$(SRCDIR) -o $@ $< $(COMMON_OBJS)

coeffs_comparison: coeffs_comparison.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -I$(SRCDIR) -o $@ $< $(COMMON_OBJS)

get_ear_events: get_ear_events.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

get_active_nodes: get_active_nodes.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

learning_show: learning_show.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

learning_delete: learning_delete.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

learning_validate: learning_validate.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

######## OPTIONS

install:
	cp coeffs_compute $(DESTDIR)/sbin/

%.install: ;

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
