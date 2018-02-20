######## FILES

COMPONENTS = custom \
             papi

######## RULES

all:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} all || exit; \
	done

######## OPTIONS

install:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -S -C $${i} install; \
	done

clean:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} clean; \
	done