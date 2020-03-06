CFLAGS = $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) $(PAPI_CFLAGS) -O2 -fPIC
LDFLAGS = $(PAPI_LDFLAGS) -ldl

######## RULES

tracer_OBJS = \
    tracer.o \
    tracer_paraver.o

tracer_BINS = \
    tracer_paraver.so

tracer_PATH = $(DESTDIR)/lib/plugins/tracer
tracer_PERM = 0775

########

all: $(tracer_OBJS) $(tracer_BINS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.so: %.o
	$(CC) -shared -o $@ $< $(LDFLAGS)

######## OPTIONS

install: tracer.ginstall;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
