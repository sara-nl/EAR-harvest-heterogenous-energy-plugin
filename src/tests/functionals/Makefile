CFLAGS = -I$(SRCDIR) $(PAPI_CFLAGS) $(IPMI_CFLAGS)

######## FILES

BINS = ear_conf

######## RULES

all: $(BINS)

ear_conf: ear_conf.c $(SRCDIR)/common/libcommon.a
	$(CC) $(CC_FLAGS) $(CFLAGS) -o ear_conf $^

######## OPTIONS

clean:
	rm -f *.o *.a *.so $(BINS)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
