######## FILES

COMPONENTS = common
ifdef CPUF_BASE
	COMPONENTS += control
	ifdef PAPI_BASE
		ifdef IPMI_BASE
			COMPONENTS += metrics \
                          database_daemon \
                          daemon \
                          commands \
                          tests
		endif
	endif
	ifdef GSL_BASE
		COMPONENTS += tools
	endif
endif
ifdef SLURM_BASE
	COMPONENTS += slurm_plugin
endif
ifdef DB_BASE
	COMPONENTS += global_manager
endif

######## RULES

.PHONY: check

all:
	@ for i in $(COMPONENTS); do                     \
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
