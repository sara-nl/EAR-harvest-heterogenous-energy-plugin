DIR    = $(ROOTDIR)/src/tools
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/library

######## FILES

BINARIES = traces_to_paraver

######## RULES

all: $(BINARIES)

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CFLAGS) -o traces_to_paraver traces_to_paraver.cpp

######## OPTIONS

install:

clean:
	rm -rf $(BINARIES)
