######## FILES

COMPONENTS = common
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

INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/include \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/sbin
    

INCLUDE_FOLDERS = $(DESTDIR)/include/common


######## RULES

.PHONY: check

all: all.components

check:
	@ $(MAKE) -C tests check

######## OPTIONS

create_folders:
	@ mkdir -p ${INSTALL_FOLDERS}
	@ mkdir -p ${INCLUDE_FOLDERS}

install: create_folders install.components

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
