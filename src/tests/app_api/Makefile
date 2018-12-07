all: app_api_test
app_api_test:app_api_test.c $(DESTDIR)/include/ear.h
	$(CC) -I$(DESTDIR)/include -o app_api_test app_api_test.c -L$(DESTDIR)/lib -lEARapi -mkl
	
clean:
	rm app_api_test
