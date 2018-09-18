######## FILES
                  
gen_FOLDERS = $(DESTDIR)/bin \
              $(DESTDIR)/bin/kernels \
              $(DESTDIR)/bin/scripts \
              $(DESTDIR)/lib \
              $(DESTDIR)/sbin

etc_FOLDERS = $(ETCDIR) \
              $(ETCDIR)/ear \
              $(ETCDIR)/ear/coeffs \
              $(ETCDIR)/ear/dbs \
              $(ETCDIR)/module \
              $(ETCDIR)/systemd \
              $(ETCDIR)/slurm 


dev_FOLDERS = $(DESTDIR)/include \
              $(DESTDIR)/include/common \
              $(DESTDIR)/include/metrics/custom \
              $(DESTDIR)/include/metrics/ipmi \
              $(DESTDIR)/include/metrics/papi \
              $(DESTDIR)/include/metrics/power_metrics

######## OPTIONS
 
install:
	@ mkdir -p $(gen_FOLDERS) 
	cp -r scripts/* $(DESTDIR)/bin/scripts/
	@ rm $(DESTDIR)/bin/scripts/README.md
	@ rm $(DESTDIR)/bin/scripts/learning/README.md

etc.install: ;
	@ mkdir -p $(etc_FOLDERS) 
	cp conf/ear.conf $(ETCDIR)/ear
	cp module/ear $(ETCDIR)/module/
	cp slurm/ear.plugstack.conf $(ETCDIR)/slurm/
	cp systemd/eard.service $(ETCDIR)/systemd
	cp systemd/eardbd.service $(ETCDIR)/systemd
	cp systemd/eargmd.service $(ETCDIR)/systemd

devel.install: ;
	@ mkdir -p $(dev_FOLDERS)

scripts.install: install;	
