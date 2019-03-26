######## FILES

OBJS = dynais.o \
       dynais_core.o

######## RULES

all: dynais.a

dynais.a: $(OBJS)
	$(AR) rvs $@ $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -march=native -c $<

######## OPTIONS

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra