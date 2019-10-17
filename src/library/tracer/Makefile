CFLAGS  = -fPIC -O3 -I$(SRCDIR) $(CONSTANTS) $(PAPI_CFLAGS)
LDFLAGS = -shared $(PAPI_LDFLAGS) -ldl
######## RULES

all: tracer.o tracer_dynais.o tracer_paraver.o tracer_dynais.so tracer_paraver.so tracer_debug.o tracer_debug.so tracer_cache.o tracer_cache.so

allt: DEFINES=-DEAR_GUI
allt: tracer.o tracer_dynais.o tracer_paraver.o

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES) -c $<

%.so:%.o
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES) $(LDFLAGS)  -shared $< -o $@

tracer_cache.so:tracer_cache.o $(SRCDIR)/metrics/cache/cpu/papi.o
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES) $(LDFLAGS) $(SRCDIR)/metrics/cache/cpu/papi.o  -shared $< -o $@


######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
