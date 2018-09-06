CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## RULES

all: libEARapi.a app_server_api.o

libEARapi.a: app_api.o
	$(AR) rcs $@ app_api.o

app_api.o: app_api.c app_api.h
	$(CC) $(CFLAGS)	-c $<

app_server_api.o: app_server_api.c app_server_api.h
	$(CC) $(CFLAGS) -c $<

######## OPTIONS

install: ;

devel.install: 
	cp libEARapi.a $(DESTDIR)/lib
	cp app_api.h $(DESTDIR)/include/

%.install: ;	

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

