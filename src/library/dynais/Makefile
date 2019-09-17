CFLAGS = $(CC_FLAGS) -fPIC -O2 $(FEAT_AVX512)

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

dynais_core.o: dynais_core.c dynais_core.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -DDYN_CORE_N -march=native -o dynais_core_n.o -c $<
	$(CC) $(CC_FLAGS) $(CFLAGS) -DDYN_CORE_0 -march=native -o dynais_core_0.o -c $<

%.o: %.c %.h
	$(CC) $(CFLAGS) -march=native -c $< 

######## OPTIONS

install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
