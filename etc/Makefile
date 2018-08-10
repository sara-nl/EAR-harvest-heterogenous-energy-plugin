######## FILES

                  
ETCEARDIR = $(ETCDIR)/ear
ETC_FOLDERS = $(ETCDIR)/module \
			  $(ETCDIR)/systemd \
			  $(ETCDIR)/slurm \
			  $(ETCEARDIR) \
              $(ETCEARDIR)/coeffs \
              $(ETCEARDIR)/dbs \
              $(ETCDIR)/scripts \
              $(ETCDIR)/scripts/examples \
              $(ETCDIR)/scripts/learning \
              $(ETCDIR)/scripts/running 


######## OPTIONS

install:
	@ mkdir -p ${ETC_FOLDERS}
	cp    module/ear $(ETCDIR)/module/
	cp -n slurm/plugstack.conf $(ETCDIR)/slurm/
	cp    conf/ear.conf $(ETCEARDIR)
	cp    systemd/eard.service $(ETCDIR)/systemd
	cp    systemd/eardbd.service $(ETCDIR)/systemd
	cp    systemd/eargmd.service $(ETCDIR)/systemd
	cp -r scripts/* $(ETCDIR)/scripts/
	@ rm  $(ETCDIR)/scripts/README.md $(ETCDIR)/scripts/learning/README.md
