all:stream_mpi
stream_mpi:stream_mpi.f Makefile
	mpiifort -O3  -qopenmp -mcmodel=medium  -qopt-streaming-stores always  \
	    -DSTREAM_ARRAY_SIZE=80000000 -DNTIMES=20 -DVERBOSE   \
	    stream_mpi.f second.f -o stream_mpi

stream_mpi_collab:stream_mpi.f Makefile
	mpif77 -O3  -fopenmp -mcmodel=medium  \
	    -DSTREAM_ARRAY_SIZE=80000000 -DNTIMES=20 -DVERBOSE   \
	    stream_mpi.f second.f -o stream_mpi
	
clean:
	rm -rf stream_mpi
