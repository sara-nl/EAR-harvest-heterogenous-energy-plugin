all: app_api_test
CC=icc
INSTALL_PATH=/hpc/base/ctt/packages/EAR/1808
app_api_test:app_api_test.c $(INSTALL_PATH)/include/ear.h
	$(CC) -I$(INSTALL_PATH)/include -o app_api_test app_api_test.c -L$(INSTALL_PATH)/lib -lEARapi -mkl
	
clean:
	rm app_api_test
