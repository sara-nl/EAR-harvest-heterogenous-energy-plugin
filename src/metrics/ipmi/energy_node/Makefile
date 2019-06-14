CFLAGS = $(CC_FLAGS) -fPIC -I$(SRCDIR) $(CONSTANTS) $(IPMI_CFLAGS)

######## FILES

ipmi_OBJS = ibm.o \
       lenovo_nm.o \
       lenovo_sd650.o

######## RULES

all: $(ipmi_OBJS)

lenovo_nm.o: lenovo_nm.c lenovo_nm.h
	$(CC) $(CFLAGS) -c $<

lenovo_sd650.o: lenovo_sd650.c lenovo_sd650.h
	$(CC) $(CFLAGS) -c $<

ibm.o: ibm.c ibm.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install:

%.install: ;

clean: rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra