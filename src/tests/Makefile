######## FILES

COMPONENTS = check functionals app_api

######## RULES

.PHONY: check functionals

all: all.components

install: ;

%.install: ;

######## OPTIONS

check:
	@ $(MAKE) -C check check || exit

clean: clean.components

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'check' $* || exit
	@ $(MAKE) -C 'functionals' $* || exit
