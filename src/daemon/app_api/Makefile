CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

all: libEARapi.a app_server_api.o

libEARapi.a: app_api.o
	$(AR) rcs $@ app_api.o

app_api.o: app_api.c ear.h
	$(CC) $(CFLAGS)	-c $<

app_server_api.o: app_server_api.c app_server_api.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;
	cp libEARapi.a $(DESTDIR)/lib
	cp ear.h $(DESTDIR)/include/ear.h

devel.install: 
	cp libEARapi.a $(DESTDIR)/lib
	cp ear.h $(DESTDIR)/include/ear.h

%.install: ;	

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

