######## FILES
                  
gen_FOLDERS = $(DESTDIR)/bin \
              $(DESTDIR)/bin/kernels \
              $(DESTDIR)/bin/scripts \
              $(DESTDIR)/bin/tools \
              $(DESTDIR)/include \
              $(DESTDIR)/lib \
              $(DESTDIR)/sbin

etc_FOLDERS = $(ETCDIR) \
              $(ETCDIR)/ear \
              $(ETCDIR)/ear/coeffs \
              $(ETCDIR)/ear/dbs \
              $(ETCDIR)/module \
              $(ETCDIR)/systemd \
              $(ETCDIR)/slurm 

dev_FOLDERS = $(DESTDIR)/include/common \
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
	@ rm $(DESTDIR)/bin/scripts/tools/README.md

etc.install: ;
	@ mkdir -p $(etc_FOLDERS) 
	cp -i conf/ear.conf $(ETCDIR)/ear
	cp -i module/ear $(ETCDIR)/module/
	cp -i slurm/ear.plugstack.conf $(ETCDIR)/slurm/
	cp -i systemd/eard.service $(ETCDIR)/systemd
	cp -i systemd/eardbd.service $(ETCDIR)/systemd
	cp -i systemd/eargmd.service $(ETCDIR)/systemd

devel.install: ;
	@ mkdir -p $(dev_FOLDERS)

scripts.install: install;	
