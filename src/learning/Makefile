DIR     = $(ROOTDIR)/src/learning
CFLAGS  = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/library $(GSL_CFLAGS) $(CPUF_CLFAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

BINARIES = compute_coefficients compute_coefficients_remote

FOREIGN_LIBS = $(ROOTDIR)/src/common/types.a

######## RULES

all: $(BINARIES)

compute_coefficients: compute_coefficients.c $(FOREIGN_LIBS)
	$(CC) $(CFLAGS) -o compute_coefficients compute_coefficients.c $(LDFLAGS) $(FOREIGN_LIBS)
compute_coefficients_remote: compute_coefficients_remote.c $(FOREIGN_LIBS)
	$(CC) $(CFLAGS) -o compute_coefficients_remote compute_coefficients_remote.c  $(FOREIGN_LIBS) $(GSL_LDFLAGS)

######## OPTIONS

install:
	mv compute_coefficients $(DESTDIR)/bin
	mv compute_coefficients_remote $(DESTDIR)/bin
	cp learning_phase_*.sh $(DESTDIR)/etc/scripts/learning

clean:
	rm -f $(BINARIES)
	rm -f *.o
