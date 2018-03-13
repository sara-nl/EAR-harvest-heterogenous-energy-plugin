INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/etc/slurm \
                  $(DESTDIR)/etc/scripts \
                  $(DESTDIR)/etc/scripts/examples \
                  $(DESTDIR)/etc/scripts/environment \
                  $(DESTDIR)/etc/scripts/learning \
                  $(DESTDIR)/etc/scripts/running \
                  $(DESTDIR)/sbin

# JUST FOR US, TODO: REMOVE
INSTALL_FOLDERS += $(DESTDIR)/DBS \
                   $(DESTDIR)/COEFFICIENTS \
                   $(DESTDIR)/SUMMARIES \
                   $(DESTDIR)/tmp

install:
	@ mkdir -p ${INSTALL_FOLDERS}
	cp    scripts/environment/lib_vars.sh $(DESTDIR)/etc/scripts/environment/lib_vars.sh
	cp -n scripts/environment/ear_vars.sh $(DESTDIR)/etc/scripts/environment/ear_vars.sh
	cp -n scripts/learning/learning_phase_compile.sh $(DESTDIR)/etc/scripts/learning/learning_phase_compile.sh
	cp -n scripts/learning/learning_phase_execute.sh $(DESTDIR)/etc/scripts/learning/learning_phase_execute.sh
	cp -n scripts/learning/learning_phase_helper.sh $(DESTDIR)/etc/scripts/learning/learning_phase_helper.sh
	cp    scripts/examples/* $(DESTDIR)/etc/scripts/examples/
	cp    scripts/running/* $(DESTDIR)/etc/scripts/running/
	cp    slurm/* $(DESTDIR)/etc/slurm/
