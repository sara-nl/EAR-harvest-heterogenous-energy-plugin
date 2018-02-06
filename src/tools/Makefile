DIR    = $(ROOTDIR)/src/tools
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/library

######## FILES

BINARIES = traces_to_paraver            \
           coefficients_quality_control

######## RULES

all: $(BINARIES)

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CFLAGS) -o traces_to_paraver traces_to_paraver.cpp

coefficients_quality_control: coefficients_quality_control.c
	$(CC) $(CFLAGS) -o coefficients_quality_control coefficients_quality_control.c \
	$(ROOTDIR)/src/common/string_enhanced.o \
	$(ROOTDIR)/src/common/types.a

######## MANUAL 

dynais_graph: dynais_graph.c
	gcc -D ANALYSIS -I ../../src/library/ -c ../../src/library/ear_dynais/ear_dynais.c -msse4.2
	gcc -I ../../src/common/ -c ../../src/common/string_enhanced.c
	gcc -D ANALYSIS -I /usr/include/graphviz $(CFLAGS) -o dynais_graph dynais_graph.c \
	-lgvc -lcgraph -lcdt ear_dynais.o string_enhanced.o

######## OPTIONS

install:
	mv traces_to_paraver $(DESTDIR)/bin/
	mv coefficients_quality_control $(DESTDIR)/bin/

clean:
	rm -rf $(BINARIES)
