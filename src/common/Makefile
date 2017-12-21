DIR    = $(ROOTDIR)/src/common
CFLAGS = -I$(DIR) -fPIC $(CONSTANTS)

######## FILES

BINARIES = ear_daemon_client.o environment.o hardware.o

######## RULES

all: $(BINARIES)

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

environment.o: environment.c environment.h ear_default_values.h
	$(CC) $(CFLAGS) -c environment.c

hardware.o: hardware.c hardware.h
	$(CC) $(CFLAGS) -c hardware.c

install:

clean:
	rm -rf $(BINARIES)
