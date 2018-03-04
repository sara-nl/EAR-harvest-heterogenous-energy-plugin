LIBNAME=libpowermon.so
LIBNAME_PRIV=libpowermon_priv.so


CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)
EAR_OBJECTS = $(SRCDIR)/common/ear_daemon_client.o
EAR_OBJECTS_P = $(SRCDIR)/metrics/ipmi/energy_node.o \
               $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_act.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_wct.o \
			   $(SRCDIR)/metrics/papi/energy_cpu.o 





all:$(LIBNAME) $(LIBNAME_PRIV) ear_power_monitor.o

$(LIBNAME):ear_power_monitor_user.o $(EAR_OBJECTS)
	$(CC) -shared ear_power_monitor_user.o $(EAR_OBJECTS) -o $(LIBNAME)

$(LIBNAME_PRIV):ear_power_monitor_priv.o $(EAR_OBJECTS_P)
	$(CC) -shared ear_power_monitor_priv.o  $(EAR_OBJECTS_P) -o $(LIBNAME_PRIV)


ear_power_monitor_user.o:ear_power_monitor.c ear_power_monitor.h specific_api.h
	$(CC) $(CFLAGS) -c ear_power_monitor.c -DEAR_USER -o ear_power_monitor_user.o

ear_power_monitor_priv.o:ear_power_monitor.c ear_power_monitor.h specific_api.h
	$(CC) $(CFLAGS) -c ear_power_monitor.c -DNO_EARDS -o ear_power_monitor_priv.o

ear_power_monitor.o:ear_power_monitor.c ear_power_monitor.h specific_api.h
	$(CC) $(CFLAGS) -c ear_power_monitor.c -DEARDS -o ear_power_monitor.o


install:
	cp $(LIBNAME) $(DESTDIR)/lib
	cp $(LIBNAME_PRIV) $(DESTDIR)/lib
	cp ear_power_monitor.h $(DESTDIR)/include
	cp specific_api.h $(DESTDIR)/include


clean:
	rm -rf $(LIBNAME) $(LIBNAME_PRIV) ear_power_monitor_priv.o ear_power_monitor.o ear_power_monitor_user.o
