CFLAGS    = -I$(SRCDIR) 
LDFLAGS   =

######## FILES

TYPES_OBJS = $(SRCDIR)/common/types.a

HEADERS = $(SRCDIR)/common/config.h 

BINARIES = 

ifdef DB_BASE
BINARIES += eargmd \
		eargm_new_job \
		eargm_end_job

DB_OBJS = $(SRCDIR)/common/database/mysql_io_functions.o
endif


EARGM_API = eargm_api.o eargm_server_api.o

######## RULES

all: $(BINARIES) $(EARGM_API) $(COMMANDS)

eargmd: ear_gm.c $(HEADERS) eargm_server_api.o $(TYPES_OBJS)
	$(CC) $(CFLAGS) $(DB_CFLAGS) -o eargmd ear_gm.c $(DB_LDFLAGS) eargm_server_api.o -lpthread $(TYPES_OBJS)

eargm_api.o:eargm_api.c eargm_api.h
	$(CC) $(CFLAGS) -c eargm_api.c

eargm_server_api.o:eargm_server_api.c eargm_server_api.h
	$(CC) $(CFLAGS) -c eargm_server_api.c

eargm_new_job:commands/eargm_new_job.c eargm_api.o
	$(CC) $(CFLAGS) -o eargm_new_job commands/eargm_new_job.c eargm_api.o

eargm_end_job:commands/eargm_end_job.c eargm_api.o
	$(CC) $(CFLAGS) -o eargm_end_job commands/eargm_end_job.c eargm_api.o


######## OPTIONS

install:
	cp $(BINARIES) $(DESTDIR)/sbin

clean:
	rm -rf $(BINARIES)
