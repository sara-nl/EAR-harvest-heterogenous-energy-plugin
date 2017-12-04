CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common $(IPMI_CFLAGS) $(CONSTANTS)

######## RULES

lenovo_act.o: lenovo_act.c lenovo_act.h
	$(CC) $(CFLAGS) -c lenovo_act.c

lenovo_wct.o: lenovo_wct.c lenovo_wct.h
	$(CC) $(CFLAGS) -c lenovo_wct.c

ibm.o: ibm.c ibm.h
	$(CC) $(CFLAGS) -c ibm.c

######## OPTIONS

clean:
	rm -rf *.o
