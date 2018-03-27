CFLAGS  = -I$(SRCDIR) 
LDFLAGS = 

BINARIES = ear_control
all: $(BINARIES)


ear_control:ear_control.c $(SRCDIR)/common/remote_daemon_client.o
	$(CC) $(CFLAGS) -o ear_control ear_control.c $(SRCDIR)/common/remote_daemon_client.o

clean:
	rm -rf $(BINARIES)
