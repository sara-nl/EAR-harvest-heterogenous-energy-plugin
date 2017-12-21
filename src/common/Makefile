DIR    = $(ROOTDIR)/src/common
CFLAGS = -I$(DIR) -fPIC $(CONSTANTS)

######## FILES

BINARIES = ear_daemon_client.o environment.o hardware.o string_enhanced.o

######## RULES

all: $(BINARIES)

ear_daemon_client.o: ear_daemon_client.c ear_daemon_client.h ear_verbose.h
	$(CC) $(CFLAGS) -c ear_daemon_client.c

environment.o: environment.c environment.h
	$(CC) $(CFLAGS) -c environment.c

hardware.o: hardware.c hardware.h
	$(CC) $(CFLAGS) -c hardware.c

string_enhanced.o: string_enhanced.c string_enhanced.h
	$(CC) $(CFLAGS) -c string_enhanced.c

install:

clean:
	rm -rf $(BINARIES)
