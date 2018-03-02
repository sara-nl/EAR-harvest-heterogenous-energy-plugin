LIBNAME=libpowermon.so
LIBNAME_PRIV=libpowermon_priv.so

CFLAGS= -fPIC -shared -I$(SRCDIR)

CFLAGS_P = $(CFLAGS) -DEAR_INTERNAL
EAR_OBJECTS = $(SRCDIR)/common/ear_daemon_client.o
EAR_OBJECTS_P = $(SRCDIR)/metrics/libear_metrics.so




all:$(LIBNAME) $(LIBNAME_PRIV)

$(LIBNAME):ear_power_monitor.o $(EAR_OBJECTS)
	$(CC) -shared ear_power_monitor.o $(EAR_OBJECTS) -o $(LIBNAME)

$(LIBNAME_PRIV):ear_power_monitor_priv.o $(EAR_OBJECTS_P)
	$(CC) -shared ear_power_monitor_priv.o $(EAR_OBJECTS_P) -o $(LIBNAME_PRIV)


ear_power_monitor.o:ear_power_monitor.c ear_power_monitor.h specific_api.h
	$(CC) $(CFLAGS) -c ear_power_monitor.c 

ear_power_monitor_priv.o:ear_power_monitor.c ear_power_monitor.h specific_api.h
	$(CC) $(CFLAGS_P) -c ear_power_monitor.c -o ear_power_monitor_priv.o

install:
	cp $(LIBNAME) $(DESTDIR)/lib
	cp $(LIBNAME_PRIV) $(DESTDIR)/lib
	cp ear_power_monitor.h $(DESTDIR)/include
	cp specific_api.h $(DESTDIR)/include


clean:
	rm -rf $(LIBNAME) $(LIBNAME_PRIV) ear_power_monitor_priv.o ear_power_monitor.o
