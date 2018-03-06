######## FILES

COMPONENTS = check functionals

######## RULES

.PHONY: check functionals

all:
	@ for i in $(COMPONENTS); do \
		echo "===================================="; \
		echo "= Compilling tests/$$i";               \
		echo "===================================="; \
		$(MAKE) -C $${i} all || exit;                \
	done

install:

functionals:
	@ $(MAKE) -C functionals all || exit

######## OPTIONS

check:
	@ $(MAKE) -C check check

clean:
	@ for i in $(COMPONENTS); do    \
		$(MAKE) -C $${i} clean; \
	done
	@ $(MAKE) -C functionals clean

