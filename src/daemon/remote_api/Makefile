CFLAGS  = $(CC_FLAGS) -I$(SRCDIR) $(CONSTANTS) 

######## INSTALLATION


######## FILES

remote_api_OBJS = \
	eard_rapi_internals.o \
    eard_rapi.o \
    eard_server_api.o \
    dynamic_configuration.o \
	dyn_conf_theading.o 

######## RULES

all: all.components $(remote_api_OBJS) all.others

dynamic_configuration.o: dynamic_configuration.c dynamic_configuration.h
	$(CC) $(CFLAGS) -c $<

dyn_conf_theading.o: dyn_conf_theading.c dyn_conf_theading.h
	$(CC) $(CFLAGS) -c $<

eard_api.o: eard_api.c eard_api.h 
	$(CC) $(CFLAGS) -fPIC -c $<

eard_rapi_internals.o: eard_rapi_internals.c eard_rapi_internals.h 
	$(CC) $(CFLAGS) -fPIC -c $<

eard_rapi.o: eard_rapi.c eard_rapi.h 
	$(CC) $(CFLAGS) -fPIC -c $<

eard_server_api.o: eard_server_api.c eard_server_api.h
	$(CC) $(CFLAGS) -c $< 


######## OPTIONS


clean: clean.components clean.others eard.gclean rclean;

######## SUBFOLDERS

%.others: ;

######## DEPENDENCIES

depend: depend.components depend.others

depend-clean: depend-clean.components depend-clean.others

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
