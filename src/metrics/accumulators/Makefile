CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -DSHOW_DEBUGS

######## FILES

band_OBJS = \
    power_metrics.o \
    energy_cpu.o

######## RULES

all: all.components $(band_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## SUBFOLDERS

%.components: ;

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
