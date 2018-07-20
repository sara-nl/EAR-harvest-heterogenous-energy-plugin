######## FILES

COMPONENTS = common database_cache
ifdef CPUF_BASE
	COMPONENTS += control
	ifdef PAPI_BASE
		ifdef IPMI_BASE
			COMPONENTS += metrics \
                          daemon \
                          global_manager \
                          commands \
                          tests
		endif
		COMPONENTS += library
	endif
	ifdef GSL_BASE
		COMPONENTS += tools
	endif
endif
ifdef SLURM_BASE
	COMPONENTS += slurm_plugin
endif
ifdef DB_BASE
	COMPONENTS += global_manager \
				  database_cache
endif

######## RULES

.PHONY: check

all: all.components

check:
	@ $(MAKE) -C tests check

######## OPTIONS

install: install.components

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
