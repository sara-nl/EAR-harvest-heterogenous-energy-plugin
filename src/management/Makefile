######## RULES

all: all.components

######## OPTIONS

install: ;

devel.install: ;

%.install: ;

clean: rclean;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'gpu' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

include $(SRCDIR)/Makefile.extra