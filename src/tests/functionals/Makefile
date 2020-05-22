CFLAGS = $(CC_FLAGS) -I$(SRCDIR) $(PAPI_CFLAGS) $(DB_FLAGS) -lm
LDFLAGS = -lm

######## FILES

ips_check_OBJS = \
        $(SRCDIR)/common/libcommon.a 

BINS = ear_conf ips_check postgres_test temperature_check

######## RULES

all: $(BINS)

postgres_test: postgres_test.c $(SRCDIR)/common/libcommon.a
	$(CC) $(CFLAGS) -o postgres_test $(DB_LDFLAGS) $^ $(LDFLAGS)

temperature_check: temperature_check.c $(SRCDIR)/metrics/libmetrics.a $(SRCDIR)/common/libcommon.a 
	$(CC) $(CFLAGS) -o temperature_check $^ $(LDFLAGS)

ear_conf: ear_conf.c $(SRCDIR)/common/libcommon.a
	$(CC) $(CFLAGS) -o ear_conf $^ $(LDFLAGS)

ips_check: ips_check.c $(ips_check_OBJS)
	$(CC) $(CFLAGS) -o ips_check $^ $(LDFLAGS)
######## OPTIONS

clean:
	rm -f *.o *.a *.so $(BINS)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
