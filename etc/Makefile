INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/etc/scripts \
                  $(DESTDIR)/sbin

# JUST FOR US, TODO: REMOVE
INSTALL_FOLDERS += $(DESTDIR)/DBS \
                   $(DESTDIR)/COEFFICIENTS \
                   $(DESTDIR)/SUMMARIES \
                   $(DESTDIR)/tmp

install:
	@ mkdir -p ${INSTALL_FOLDERS}
	cp scripts/environment/lib_vars.sh $(DESTDIR)/etc/scripts/environment/lib_vars.sh
	cp -n scripts/environment/ear_vars.sh $(DESTDIR)/etc/scripts/environment/ear_vars.sh
	cp -rn scripts/learning $(DESTDIR)/etc/scripts/
	cp -rn scripts/examples $(DESTDIR)/etc/scripts/
	cp -rn scripts/running $(DESTDIR)/etc/scripts/
	cp -rn slurm $(DESTDIR)/etc/slurm
