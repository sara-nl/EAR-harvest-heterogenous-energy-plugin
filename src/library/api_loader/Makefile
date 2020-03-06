CFLAGS  = $(CC_FLAGS) $(MPI_CFLAGS) -I$(SRCDIR) -O2 -fPIC
LDFLAGS = $(SRCDIR)/common/libcommon.a

######## FILES

load_OBJS = \
    loader.o \
    mpic_syms.o \
    mpif_syms.o

load_LIBS = \
    libloader.so

######## RULES

all: clean $(load_OBJS) $(load_LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

libloader.so:
	$(CC) -shared -o libloader.so $(load_OBJS) $(LDFLAGS)

######## OPTIONS 

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
