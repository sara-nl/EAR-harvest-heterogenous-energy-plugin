######## FILES

COMPONENTS = common database_cache
ifdef CPUF_BASE
	COMPONENTS += control
	ifdef DB_BASE
    COMPONENTS += database_cache
	endif
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
	COMPONENTS += global_manager
endif

######## RULES

.PHONY: check

all: all.components

check:
	@ $(MAKE) -C tests check

install: install.components

etc.install: etc.install.components;

devel.install: devel.install.components;

%.install:
	@ $(MAKE) -C $* install || exit

clean: clean.components

######## SUBFOLDERS

%.components:
	@ for i in $(COMPONENTS); do                     \
		echo "===================================="; \
		echo "= Compilling $$i";                     \
		echo "===================================="; \
		$(MAKE) -C $${i} $* || exit;                 \
	done

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components