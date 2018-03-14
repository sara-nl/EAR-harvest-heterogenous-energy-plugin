######## FILES

COMPONENTS = common \
             metrics \
             control \
             library \
             daemon \
             learning \
             tools \
             tests
ifdef SLURM_BASE
	COMPONENTS += slurm_plugin
endif

######## RULES

.PHONY: check

all:
	@ for i in $(COMPONENTS); do                         \
		echo "===================================="; \
		echo "= Compilling $$i";                     \
		echo "===================================="; \
		$(MAKE) -C $${i} all || exit;                \
	done

check:
	@ $(MAKE) -C tests check

functionals:
	@ $(MAKE) -C tests functionals || exit

######## OPTIONS

install:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -S -C $${i} install; \
	done

clean:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} clean; \
	done
