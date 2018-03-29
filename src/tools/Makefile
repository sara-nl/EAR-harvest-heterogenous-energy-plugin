CFLAGS = -I$(SRCDIR) $(GSL_CFLAGS) $(CPUF_CFLAGS)
LDFLAGS = $(CPUF_LDFLAGS) $(GSL_LDFLAGS)

######## FILES

COMMON_OBJS = $(ROOTDIR)/src/common/string_enhanced.o \
              $(ROOTDIR)/src/common/types.a

BINARIES = compute_coefficients \
           compute_coefficients_remote \
           coefficients_quality_control

######## RULES

all: $(BINARIES)

compute_coefficients: compute_coefficients.c
	$(CC) $(CFLAGS) -o compute_coefficients compute_coefficients.c $(LDFLAGS) $(COMMON_OBJS)

compute_coefficients_remote: compute_coefficients_remote.c
	$(CC) $(CFLAGS) -o compute_coefficients_remote compute_coefficients_remote.c $(LDFLAGS) $(COMMON_OBJS)

coefficients_quality_control: coefficients_quality_control.c
	$(CC) $(CFLAGS) -o coefficients_quality_control coefficients_quality_control.c $(COMMON_OBJS)

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CFLAGS) -o traces_to_paraver traces_to_paraver.cpp

######## OPTIONS

install:
	mv compute_coefficients $(DESTDIR)/bin
	mv coefficients_quality_control $(DESTDIR)/bin/

clean:
	rm -rf $(BINARIES)
	rm -f *.o *.a *.so