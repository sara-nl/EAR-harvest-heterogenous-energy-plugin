CFLAGS = -fPIC -O3 -I$(SRCDIR)

######## FILES

OBJS = avx2/dynais.o \
       avx2/dynais_core_0.o \
       avx2/dynais_core_n.o
DEPS = avx2/dynais.o \
       avx2/dynais_core.o

ifeq ($(FEAT_AVX512), 1)
OBJS += avx512/dynais.o \
        avx512/dynais_core_0.o \
        avx512/dynais_core_n.o
DEPS += avx512/dynais.o \
        avx512/dynais_core.o
DEFI  = -DFEAT_AVX512=1
endif

######## RULES

all: dynais.a

avx512/dynais_core.o: avx512/dynais_core.c avx512/dynais_core.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -march=native $(DEFI) -DDYN_CORE_N -o avx512/dynais_core_n.o -c $<
	$(CC) $(CC_FLAGS) $(CFLAGS) -march=native $(DEFI) -DDYN_CORE_0 -o avx512/dynais_core_0.o -c $<

avx2/dynais_core.o: avx2/dynais_core.c avx2/dynais_core.h
	$(CC) $(CC_FLAGS) $(CFLAGS) -mavx2 -DDYN_CORE_N -o avx2/dynais_core_n.o -c $<
	$(CC) $(CC_FLAGS) $(CFLAGS) -mavx2 -DDYN_CORE_0 -o avx2/dynais_core_0.o -c $<

$(FOLD)/dynais.o: $(FOLD)/dynais.c
	$(CC) $(CC_FLAGS) $(CFLAGS) $(DEFI) -o $(FOLD)/dynais.o -c $<

dynais.a: $(DEPS)
	$(AR) rvs $@ $(OBJS)

######## OPTIONS

install: ;

clean: rclean
	rm -f ./avx2/*.o ./avx512/*.o

depend: ;

depend-clean: ;

######## EXPORTS

include $(SRCDIR)/Makefile.extra