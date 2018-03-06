######## FILES

COMPONENTS = common \
             metrics \
             library \
             daemon \
             learning \
             tools \
             tests
ifdef SLURM_BASE
	COMPONENTS += slurm_plugin
endif

INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/etc/scripts \
                  $(DESTDIR)/etc/scripts/environment \
                  $(DESTDIR)/etc/scripts/examples \
                  $(DESTDIR)/etc/scripts/learning \
                  $(DESTDIR)/etc/scripts/running \
                  $(DESTDIR)/sbin

# JUST FOR US, TODO: REMOVE
INSTALL_FOLDERS += $(DESTDIR)/DBS \
                   $(DESTDIR)/COEFFICIENTS \
                   $(DESTDIR)/SUMMARIES \
                   $(DESTDIR)/tmp

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
	@ mkdir -p ${INSTALL_FOLDERS}
	@ for i in $(COMPONENTS); do \
		$(MAKE) -S -C $${i} install; \
	done

clean:
	@ for i in $(COMPONENTS); do \
		$(MAKE) -C $${i} clean; \
	done
