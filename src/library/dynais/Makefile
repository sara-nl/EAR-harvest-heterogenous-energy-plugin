CFLAGS = $(CC_FLAGS) -fPIC -O2 -I$(SRCDIR) $(FEAT_AVX512)

######## FILES

ifdef FEAT_AVX512
FOLD = avx512
else
FOLD = avx2
endif

OBJS = $(FOLD)/dynais.o \
       $(FOLD)/dynais_core_0.o \
       $(FOLD)/dynais_core_n.o
DEPS = $(FOLD)/dynais.o \
       $(FOLD)/dynais_core.o

######## RULES

all: dynais.a

dynais.a: $(DEPS)
	$(AR) rvs $@ $(OBJS)

$(FOLD)/dynais_core.o: $(FOLD)/dynais_core.c $(FOLD)/dynais_core.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -DDYN_CORE_N -march=native -o $(FOLD)/dynais_core_n.o -c $<
	$(CC) $(CC_FLAGS) $(CFLAGS) -DDYN_CORE_0 -march=native -o $(FOLD)/dynais_core_0.o -c $<

$(FOLD)/dynais.o: $(FOLD)/dynais.c
	$(CC) $(CFLAGS) -march=native -o $(FOLD)/dynais.o -c $< 

######## OPTIONS

install: ;

clean: rclean
	rm -f $(OBJS)

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
