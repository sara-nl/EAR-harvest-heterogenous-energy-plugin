CFLAGS  = -I$(DIR) -I$(ROOTDIR)/src/common -fPIC $(CONSTANTS)

######## FILES

OBJS = ear_api.o \
       MPI_intercept.o \
       MPI_interface.o \
       process_MPI.o

LIBRARIES = ear_api.a

######## RULES

MPI_interface.o: MPI_interface.h MPI_interface.c MPI_calls_coded.h
	$(MPICC) $(CFLAGS) -c MPI_interface.c

MPI_intercept.o: MPI_intercept.c
	$(MPICC) $(CFLAGS) -c MPI_intercept.c

process_MPI.o: process_MPI.h process_MPI.c MPI_types.h 
	$(MPICC) $(CFLAGS) -c process_MPI.c

ear_api.o: ear_api.h ear_api.c MPI_types.h MPI_calls_coded.h
	$(MPICC) $(CFLAGS) -c ear_api.c

ear_api.a: $(OBJS) 
	ar rvs ear_api.a $(OBJS)

clean:
	rm -f $(OBJS) $(LIBRARIES)
