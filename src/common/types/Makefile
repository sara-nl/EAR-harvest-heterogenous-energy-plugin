CFLAGS = -fPIC -I$(DIR) -I$(ROOTDIR)/src/common $(CONSTANTS)

######## RULES

types_application.o: types_application.h types_application.c
	$(CC) $(CFLAGS) -c types_application.c types_projection.c

types_projection.o: types_application.h types_application.c
	$(CC) $(CFLAGS) -c types_projection.c types_projection.c

######## OPTIONS

clean:
	rm -rf *.o
