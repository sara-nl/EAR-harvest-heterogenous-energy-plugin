CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

freq_OBJS = frequency.o

######## RULES

all: all.components $(freq_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'cpu' $* || exit
	@ $(MAKE) -C 'imc' $* || exit

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra