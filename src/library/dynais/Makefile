######## FILES

OBJS = dynais.o \
       dynais_core_0.o \
       dynais_core_n.o
DEPS = dynais.o \
       dynais_core.o

######## RULES

all: dynais.a

dynais.a: $(DEPS)
	$(AR) rvs $@ $(OBJS)

dynais_core.o: dynais_core.c dynais_core.h
	$(CC) $(CFLAGS) -DDYN_CORE_N -march=native -o dynais_core_n.o -c $<
	$(CC) $(CFLAGS) -DDYN_CORE_0 -march=native -o dynais_core_0.o -c $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -march=native -c $< 

######## OPTIONS

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
