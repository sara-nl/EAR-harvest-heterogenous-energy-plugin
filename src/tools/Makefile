CFLAGS  = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

COMMON_OBJS = $(ROOTDIR)/src/common/types.a

BINARIES = compute_coefficients \
		   compute_coefficients_v2 \
           compute_coefficients_remote \
           coefficients_quality_control \
		   coefficients_to_csv \
		   coefficients_v2_to_v3 \
		   get_app_list

######## RULES

all: $(BINARIES)

compute_coefficients: compute_coefficients.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

compute_coefficients_v2: compute_coefficients_v2.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

compute_coefficients_remote: compute_coefficients_remote.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

coefficients_quality_control: coefficients_quality_control.c $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(COMMON_OBJS)

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $<

get_app_list: get_app_list.c $(ROOTDIR)/src/common/database.a $(COMMON_OBJS)
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(ROOTDIR)/src/common/database.a $(DB_LDFLAGS) $(COMMON_OBJS)

coefficients_to_csv: coefficients_to_csv.c $(COMMON_OBJS
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

coefficients_v2_to_v3:coefficients_v2_to_v3.c $(COMMON_OBJS
	$(CC) $(CC_FLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS) $(COMMON_OBJS)

######## OPTIONS

install:
	cp compute_coefficients $(DESTDIR)/sbin/
	cp coefficients_quality_control $(DESTDIR)/sbin/

%.install: ;

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
