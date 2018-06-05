######## FILES

COMPONENTS = check functionals

######## RULES

.PHONY: check functionals

all: all.components

install:

functionals:
	@ $(MAKE) -C functionals all || exit

######## OPTIONS

check:
	@ $(MAKE) -C check check || exit

clean: clean.components

######## SUBFOLDERS

%.components:
	@ $(MAKE) -C 'check' $* || exit
	@ $(MAKE) -C 'functionals' $* || exit
