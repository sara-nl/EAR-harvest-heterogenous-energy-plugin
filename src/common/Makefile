DIR    = $(ROOTDIR)/src/common
CFLAGS = -I$(DIR) -fPIC $(CONSTANTS)

#################### RULES ##########################

all: ear_daemon_client.o ear_configuration.o hardware.o

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

ear_configuration.o: ear_configuration.c ear_configuration.h ear_default_values.h
	$(CC) $(CFLAGS) -c ear_configuration.c

hardware.o: hawrdware.c hardware.h
	$(CC) $(CFLAGS) -c hardware.c

install:

clean:
	rm -rf ear_daemon_client.o ear_configuration.o ear_arch_type.o
