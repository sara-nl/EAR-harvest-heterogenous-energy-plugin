DIR     = $(ROOTDIR)/src/learning
CFLAGS  = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/library $(GSL_CFLAGS) $(CPUF_CLFAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

BINARIES = compute_coefficients

######## RULES

all: $(BINARIES)

compute_coefficients: compute_coefficients.c
	$(CC) $(CFLAGS) -o compute_coefficients compute_coefficients.c $(LDFLAGS)

######## OPTIONS

install:
	mv compute_coefficients $(DESTDIR)/bin
	cp learning_phase_compile.sh $(DESTDIR)/etc
	cp learning_phase_execute.sh $(DESTDIR)/etc
	cp learning_phase_helper.sh $(DESTDIR)/etc
clean:
	rm -f $(BINARIES)
	rm -f *.o
