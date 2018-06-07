######## FILES

COMPONENTS = check functionals

######## RULES

.PHONY: check functionals

all: all.components

install:

######## OPTIONS

check:
	@ $(MAKE) -C check check || exit

clean: clean.components

depend: depend.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'check' $* || exit
	@ $(MAKE) -C 'functionals' $* || exit