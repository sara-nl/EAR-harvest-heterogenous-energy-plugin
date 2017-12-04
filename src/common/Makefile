DIR    = $(ROOTDIR)/src/common
CFLAGS = -I$(DIR) -fPIC $(CONSTANTS)

#################### RULES ##########################

all: ear_daemon_client.o ear_configuration.o

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

ear_configuration.o: ear_configuration.c ear_configuration.h ear_default_values.h
	$(CC) $(CFLAGS) -c ear_configuration.c

clean:
	rm -rf ear_daemon_client.o ear_configuration.o
