CFLAGS = $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC

######## FILES

OBJS = \
    api.o \
    api_ear.o \
    earc_syms.o \
    earf_syms.o \

######## RULES

all: $(OBJS)

api_ear.o: api_ear.c api_ear.h
	$(CC) $(CFLAGS)  $(PAPI_CFLAGS) -c $<	

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
