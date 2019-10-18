CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

energy_OBJS = energy_nm.o energy_rapl.o
energy_SOFILES = energy_nm.so energy_rapl.so

######## RULES

all: all.components $(flop_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.so: %.c
	$(CC) $(CC_FLAGS) $(CFLAGS) -shared $< -o $@ 


######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'cpu' $* || exit

######## OPTIONS

install: ;

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
