CFLAGS  = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)


######## FILES

COMMON_OBJS = $(ROOTDIR)/src/common/types.a

BINARIES = compute_coefficients \
           coeffs_jobproj \
           coeffs_quality \
		   get_app_list \
		   get_ear_events \
		   get_projections \
		   get_active_nodes \
		   ear_lp_validation \
		   remove_learning \
		   translate_coefficients
		   
#compute_coefficients_v2 \
#compute_coefficients_remote \
#coefficients_to_csv \
#coefficients_v2_to_v3 \
#coefficients_comparison \
#show_coefficients \

all: $(BINARIES)

compute_coefficients: compute_coefficients.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

compute_coefficients_v2: compute_coefficients_v2.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

compute_coefficients_remote: compute_coefficients_remote.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

coeffs_jobproj: coeffs_jobproj.c coeffs_jobproj.h $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coeffs_quality: coeffs_quality.c coeffs_quality.h $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $<

get_app_list: get_app_list.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

get_projections: get_projections.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

get_ear_events: get_ear_events.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

get_active_nodes: get_active_nodes.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

remove_learning: remove_learning.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -g -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coefficients_to_csv: coefficients_to_csv.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

coefficients_comparison:coefficients_comparison.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -I$(SRCDIR) -o $@ $< $(COMMON_OBJS)

show_coefficients:show_coefficients.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -I$(SRCDIR) -o $@ $< $(COMMON_OBJS)

translate_coefficients:translate_coefficients.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) -I$(SRCDIR) -o $@ $< $(COMMON_OBJS) -lm

coefficients_v2_to_v3:coefficients_v2_to_v3.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

ear_lp_validation: ear_lp_validation.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

######## OPTIONS

install:
	cp compute_coefficients $(DESTDIR)/sbin/

%.install: ;

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
