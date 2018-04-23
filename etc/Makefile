######## FILES

INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/lib \
                  $(DESTDIR)/etc \
                  $(DESTDIR)/sbin
                  
ETC_FOLDERS = $(ETCDIR)/module \
              $(ETCDIR)/slurm \
              $(ETCDIR)/scripts \
              $(ETCDIR)/scripts/examples \
              $(ETCDIR)/scripts/learning \
              $(ETCDIR)/scripts/running \
              $(ETCDIR)/dbs \
              $(ETCDIR)/coeffs \
              $(ETCDIR)/misc

######## OPTIONS

install:
	@ mkdir -p ${INSTALL_FOLDERS}
	@ mkdir -p ${ETC_FOLDERS}
	cp    module/ear $(ETCDIR)/module/
	cp -n slurm/ear.conf $(ETCDIR)/slurm/
	cp -n slurm/ear.link $(ETCDIR)/slurm/
	cp -n slurm/plugstack.conf $(ETCDIR)/slurm/
	cp -r scripts/* $(ETCDIR)/scripts/
	@ rm  $(ETCDIR)/scripts/README.md $(ETCDIR)/scripts/learning/README.md
	
	#cp   scripts/running/mpi_exec.sh $(DESTDIR)/bin/mpi_exec.sh
	#cp   scripts/running/ssh_daemon_exit.sh $(DESTDIR)/sbin
	#cp   scripts/running/ssh_daemon_start.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_compile.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_execute.sh $(DESTDIR)/sbin
	#cp   scripts/learning/learning_phase_helper.sh $(DESTDIR)/sbin

