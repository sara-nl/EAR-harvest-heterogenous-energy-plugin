CFLAGS = -fPIC -I$(SRCDIR) $(CONSTANTS) $(DB_CFLAGS)

######## RULES

mysql_io_functions.o: mysql_io_functions.c mysql_io_functions.h 
	$(CC) $(CFLAGS) -c mysql_io_functions.c

db_helper.o: db_helper.c db_helper.h mysql_io_functions.c 
	$(CC) $(CFLAGS) $(DB_LDFLAGS) -c db_helper.c mysql_io_functions.c 

######## OPTIONS

clean:
	rm -rf *.o *.so *.a

