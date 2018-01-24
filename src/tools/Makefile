DIR    = $(ROOTDIR)/src/tools
CFLAGS = -I$(DIR) -I$(ROOTDIR)/src/common -I$(ROOTDIR)/src/library

######## FILES

BINARIES = show_metrics traces_to_paraver coefficients_to_csv
BINARIES = show_metrics coefficients_to_csv compute_projections compute_projections_from_csv

######## RULES

all: $(BINARIES)

show_metrics: show_metrics.c
	$(CC) $(CFLAGS) -o show_metrics show_metrics.c

traces_to_paraver: traces_to_paraver.cpp
	$(CC) $(CFLAGS) -o traces_to_paraver traces_to_paraver.cpp

coefficients_to_csv: coefficients_to_csv.c
	$(CC) $(CFLAGS) -o coefficients_to_csv coefficients_to_csv.c

compute_projections:compute_projections.c projections.o
	$(CC) $(CFLAGS) -o compute_projections compute_projections.c projections.o

compute_projections_from_csv:compute_projections_from_csv.c projections.o coefficients_tools.o
	$(CC) $(CFLAGS) -o compute_projections_from_csv compute_projections_from_csv.c projections.o coefficients_tools.o

projections.o:projections.c
	$(CC) $(CFLAGS) -DVERBOSE=1 -c projections.c

coefficients_tools.o:coefficients_tools.c
	$(CC) $(CFLAGS)  -c coefficients_tools.c

######## OPTIONS

install:
	mv show_metrics $(DESTDIR)/bin
	#mv traces_to_paraver $(DESTDIR)/bin
	mv coefficients_to_csv $(DESTDIR)/bin
	mv compute_projections $(DESTDIR)/bin
	mv compute_projections_from_csv $(DESTDIR)/bin
clean:
	rm -rf $(BINARIES)
