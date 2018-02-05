DIR    = $(ROOTDIR)/src/common
CFLAGS = -I$(DIR) -fPIC $(CONSTANTS)

######## FILES

BINARIES = ear_daemon_client.o environment.o hardware.o string_enhanced.o types.a

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

types.a: types_generic.h types_application.c types_projection.c
	$(CC) $(CFLAGS) -c types_application.c types_projection.c
	ar rvs types.a types_application.o types_projection.o

install:

clean:
	rm -rf $(BINARIES)
