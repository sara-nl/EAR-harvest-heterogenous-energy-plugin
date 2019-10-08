CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC

######## FILES

cach_OBJS = cache.o

######## RULES

all: all.components $(cach_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

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