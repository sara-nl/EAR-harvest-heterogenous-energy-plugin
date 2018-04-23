LIBNAME=libpowermon.so 
LIB=libpowermon.a


CFLAGS  = -fPIC -I$(SRCDIR) $(CONSTANTS)
EAR_OBJECTS = $(SRCDIR)/common/ear_daemon_client.o \
			   $(SRCDIR)/metrics/ipmi/energy_node.o \
               $(SRCDIR)/metrics/ipmi/energy_node/ibm.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_act.o \
               $(SRCDIR)/metrics/ipmi/energy_node/lenovo_wct.o \
			   $(SRCDIR)/metrics/papi/energy_cpu.o  \
			   $(SRCDIR)/metrics/custom/hardware_info.o \
			   $(SRCDIR)/common/math_operations.o



all:$(LIBNAME) $(LIB)

$(LIBNAME):ear_power_monitor.o $(EAR_OBJECTS)
	$(CC) -shared ear_power_monitor.o $(EAR_OBJECTS) -o $(LIBNAME)

$(LIB):ear_power_monitor.o $(EAR_OBJECTS)
	ar rcs $(LIB) ear_power_monitor.o $(EAR_OBJECTS)


ear_power_monitor.o:ear_power_monitor.c ear_power_monitor.h 
	$(CC) $(CFLAGS) -c ear_power_monitor.c 


install:
	cp $(LIBNAME) $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/include/metrics/power_monitoring
	cp ear_power_monitor.h $(DESTDIR)/include/metrics/power_monitoring


clean:
	rm -rf $(LIBNAME) *.o
