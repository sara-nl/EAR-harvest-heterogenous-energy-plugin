INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/etc/module \
                  $(DESTDIR)/etc/slurm \
                  $(DESTDIR)/etc/scripts \
                  $(DESTDIR)/etc/scripts/examples \
                  $(DESTDIR)/etc/scripts/learning \
                  $(DESTDIR)/etc/scripts/running \
                  $(DESTDIR)/sbin

# JUST FOR US, TODO: REMOVE
INSTALL_FOLDERS += $(DESTDIR)/dbs \
                   $(DESTDIR)/coeffs \
                   $(DESTDIR)/tmp

install:
	@ mkdir -p ${INSTALL_FOLDERS}
	cp    module/ear $(DESTDIR)/etc/module/
	cp    slurm/ear.conf $(DESTDIR)/etc/slurm/
	cp    slurm/plugstack.conf $(DESTDIR)/etc/slurm/
	cp    scripts/examples/* $(DESTDIR)/etc/scripts/examples/
	cp -n scripts/running/* $(DESTDIR)/etc/scripts/running/

#cp -n scripts/learning/learning_phase_compile.sh $(DESTDIR)/etc/scripts/learning/learning_phase_compile.sh
#cp -n scripts/learning/learning_phase_execute.sh $(DESTDIR)/etc/scripts/learning/learning_phase_execute.sh
#cp -n scripts/learning/learning_phase_helper.sh $(DESTDIR)/etc/scripts/learning/learning_phase_helper.sh
