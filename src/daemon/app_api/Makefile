CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)

######## OBJS

APP_OBJ = app_api.o
APP_INC = app_api.h
APP_LIB = libEARapi.a

APP_SERVER= app_server_api.o
######## RULES

all: $(APP_LIB) $(APP_SERVER)

$(APP_LIB):$(APP_OBJ)
	$(AR) rcs $(APP_LIB) $(APP_OBJ)

%.o: %.c %.h

######## OPTIONS
APP_INC = app_api.h

install: 
	cp $(APP_LIB) $(DESTDIR)/lib
	cp $(APP_INC) $(DESTDIR)/include/

clean:
	rm -f *.o *.a *.so

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend

