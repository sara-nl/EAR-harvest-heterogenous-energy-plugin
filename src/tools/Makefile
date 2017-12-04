DIR    = $(ROOTDIR)/tools
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common

######## RULES

all: show_metrics traces_to_paraver

show_metrics: show_metrics.c
	$(CC) $(CFLAGS) -o show_metrics show_metrics.c

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CFLAGS) -o traces_to_paraver traces_to_paraver.cpp

######## OPTIONS

install:
	mv show_metrics $(DESTDIR)/bin
	mv traces_to_paraver $(DESTDIR)/bin
clean:
	rm -rf show_metrics traces_to_paraver
