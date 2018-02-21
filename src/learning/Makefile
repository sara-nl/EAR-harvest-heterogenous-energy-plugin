CFLAGS  = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CLFAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

COMMON_OBJS = $(SRCDIR)/common/types.a

BINARIES = compute_coefficients compute_coefficients_remote

######## RULES

all: $(BINARIES)

compute_coefficients: compute_coefficients.c
	$(CC) $(CFLAGS) -o compute_coefficients compute_coefficients.c $(LDFLAGS) $(COMMON_OBJS)

compute_coefficients_remote: compute_coefficients_remote.c
	$(CC) $(CFLAGS) -o compute_coefficients_remote compute_coefficients_remote.c $(LDFLAGS) $(COMMON_OBJS)

######## OPTIONS

install:
	mv compute_coefficients $(DESTDIR)/bin
	mv compute_coefficients_remote $(DESTDIR)/bin
	cp learning_phase_*.sh $(DESTDIR)/etc/scripts/learning

clean:
	rm -f $(BINARIES)
	rm -f *.o *.a *.so
