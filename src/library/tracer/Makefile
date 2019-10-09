######## RULES

all: tracer.o tracer_dynais.o tracer_paraver.o tracer_dynais.so tracer_paraver.so tracer_debug.o tracer_debug.so

allt: DEFINES=-DEAR_GUI
allt: tracer.o tracer_dynais.o tracer_paraver.o

%.o: %.c %.h
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES) -c $<

%.so:%.o
	$(MPICC) $(CC_FLAGS) $(CFLAGS) $(DEFINES)  -shared $< -o $@


######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
