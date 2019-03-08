CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## INSTALLATION

eard_BINS = eard
eard_PATH = $(DESTDIR)/lib
eard_PERM = 0660
oths_BINS = ear.h
oths_PATH = $(DESTDIR)/include
oths_PERM = 0660

######## RULES

all: app_server_api.o $(eard_BINS)

libEARapi.a: app_api.o
	$(AR) rcs $@ app_api.o

app_api.o: app_api.c ear.h
	$(CC) $(CFLAGS)	-c $<

app_server_api.o: app_server_api.c app_server_api.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

devel.install: eard.ginstall oths.ginstall;

%.install: ;	

clean: eard.gclean rclean;

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

