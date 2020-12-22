CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -fPIC -Wall

######## OPTIONS

DISABLE_AMD = 0

ifeq ($(DISABLE_AMD), 1)
DEFS += -DDISABLE_AMD=1
endif

######## FILES

arch_OBJS = \
    amd17.o \
    default.o

######## RULES

all: $(arch_OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFS) -c $<

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
