######## FILES

COMPONENTS = check \
             functionals

######## RULES

.PHONY: check

all:
	@ for i in $(COMPONENTS); do                         \
		echo "===================================="; \
		echo "= Compilling tests/$$i";               \
                echo "===================================="; \
		$(MAKE) -C $${i} all || exit;                \
        done

######## OPTIONS

check:
	@ $(MAKE) -C check check

clean:
	@ for i in $(COMPONENTS); do    \
                $(MAKE) -C $${i} clean; \
        done
