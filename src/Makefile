######## FILES

COMPONENTS = common
ifdef CPUF_BASE
	COMPONENTS += control
	ifdef DB_BASE
    COMPONENTS += database_cache
	endif
	ifdef PAPI_BASEa
		ifdef IPMI_BASE
			COMPONENTS += metrics \
                          daemon \
                          global_manager \
                          commands \
                          tests
		endif
		COMPONENTS += library
	endif
	ifdef GSL_BASEa
		COMPONENTS += tools
	endif
endif
ifdef SLURM_BASEa
	COMPONENTS += slurm_plugin
endif
ifdef DB_BASEa
	COMPONENTS += global_manager
endif

######## RULES

.PHONY: check

all: all.components

check:
	@ $(MAKE) -C tests check

######## OPTIONS

install: install.components

%.install:
	@ $(MAKE) -C $* install || exit

clean: clean.components

depend: depend.components

depend-clean: depend-clean.components

######## SUBFOLDERS

%.components:
	@ for i in $(COMPONENTS); do                     \
		echo "===================================="; \
		echo "= Compilling $$i";                     \
		echo "===================================="; \
		$(MAKE) -C $${i} $* || exit;                 \
	done
