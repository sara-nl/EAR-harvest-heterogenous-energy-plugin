######## RULES

all: all.components

######## OPTIONS

install: install.components;

devel.install: ;

%.install: ;

clean: clean.components;

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'gpu' $* || exit

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components

######## DEPENDENCIES

include $(SRCDIR)/Makefile.depend
include $(SRCDIR)/Makefile.extra
