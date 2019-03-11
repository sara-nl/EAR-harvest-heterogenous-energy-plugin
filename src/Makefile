######## FILES

COMPONENTS = common control database_cache metrics daemon commands tests library tools global_manager slurm_plugin
ifdef CPUF_BASE
	COMPONENTS += control
	ifdef DB_BASE
	COMPONENTS += database_cache
	endif
	ifdef PAPI_BASE
		ifdef IPMI_BASE
			COMPONENTS += metrics \
                          daemon \
                          commands \
                          tests
		endif
		COMPONENTS += library
	endif
	ifdef GSL_BASE
		COMPONENTS += tools
	endif
endif
ifdef DB_BASE
	COMPONENTS += global_manager
endif
ifdef SLURM_BASE
	COMPONENTS += slurm_plugin
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
		echo "= Compiling $$i";                      \
		echo "===================================="; \
		$(MAKE) -C $${i} $* || exit;                 \
	done

######## ALIASES

eard.install: daemon.install;

eardbd.install: database_cache.install;

eargmd.install: global_manager.install;

earplug.install: slurm_plugin.install;

######## DEPENDENCIES

depend: depend.components

depend-clean: depend-clean.components