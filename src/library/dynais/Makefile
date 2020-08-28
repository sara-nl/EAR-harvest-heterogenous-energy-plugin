CFLAGS = $(CC_FLAGS) -I$(SRCDIR) -O3 -fPIC

######## FILES

ifeq ($(FEAT_AVX512), 1)
DEFI = -march=native -DFEAT_AVX512=1
FOLD = avx512
else
DEFI = -mavx2
FOLD = avx2
endif

OBJS = $(FOLD)/dynais.o \
       $(FOLD)/dynais_core_0.o \
       $(FOLD)/dynais_core_n.o
DEPS = $(FOLD)/dynais.o \
       $(FOLD)/dynais_core.o

######## RULES

all: dynais.a

$(FOLD)/dynais_core.o: $(FOLD)/dynais_core.c $(FOLD)/dynais_core.h
	$(CC) $(CFLAGS) $(DEFI) -DDYN_CORE_N -o $(FOLD)/dynais_core_n.o -c $<
	$(CC) $(CFLAGS) $(DEFI) -DDYN_CORE_0 -o $(FOLD)/dynais_core_0.o -c $<

$(FOLD)/dynais.o: $(FOLD)/dynais.c
	$(CC) $(CFLAGS) $(DEFI) -o $(FOLD)/dynais.o -c $< 

dynais.a: $(DEPS)
	$(AR) rvs $@ $(OBJS)

######## OPTIONS

install: ;

clean: rclean
	rm -f $(OBJS)

depend: ;

depend-clean: ;

######## EXPORTS

include $(SRCDIR)/Makefile.extra
