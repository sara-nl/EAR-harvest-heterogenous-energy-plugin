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
	cp -n slurm/ear.conf $(DESTDIR)/etc/slurm/
	cp -n slurm/ear.link $(DESTDIR)/etc/slurm/
	cp -n slurm/plugstack.conf $(DESTDIR)/etc/slurm/
	cp    scripts/examples/* $(DESTDIR)/etc/scripts/examples/
	cp    scripts/running/* $(DESTDIR)/etc/scripts/running/
	cp    scripts/learning/learning_phase_compile.sh $(DESTDIR)/etc/scripts/learning/learning_phase_compile.sh
	cp    scripts/learning/learning_phase_execute.sh $(DESTDIR)/etc/scripts/learning/learning_phase_execute.sh
	cp    scripts/learning/learning_phase_helper.sh $(DESTDIR)/etc/scripts/learning/learning_phase_helper.sh
	cp    scripts/running/mpi_exec.sh $(DESTDIR)/bin/mpi_exec.sh
	#cp   scripts/running/ssh_daemon_exit.sh $(DESTDIR)/sbin
	#cp   scripts/running/ssh_daemon_start.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_compile.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_execute.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_helper.sh $(DESTDIR)/sbin

