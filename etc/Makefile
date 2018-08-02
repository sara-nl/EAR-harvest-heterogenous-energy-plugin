######## FILES

INSTALL_FOLDERS = $(DESTDIR)/bin \
                  $(DESTDIR)/bin/kernels \
                  $(DESTDIR)/include \
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
              $(ETCDIR)/systemd

INCLUDE_FOLDERS = $(DESTDIR)/include/common

######## OPTIONS

install:
	@ mkdir -p ${INSTALL_FOLDERS}
	@ mkdir -p ${INCLUDE_FOLDERS}
	@ mkdir -p ${ETC_FOLDERS}
	cp    module/ear $(ETCDIR)/module/
	cp -n slurm/plugstack.conf $(ETCDIR)/slurm/
	cp    conf/ear.conf $(ETCDIR)
	cp    systemd/eard.service $(ETCDIR)/systemd
	cp    systemd/eardbd.service $(ETCDIR)/systemd
	cp    systemd/eargmd.service $(ETCDIR)/systemd
	cp -r scripts/* $(ETCDIR)/scripts/
	@ rm  $(ETCDIR)/scripts/README.md $(ETCDIR)/scripts/learning/README.md
