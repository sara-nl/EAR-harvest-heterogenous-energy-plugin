all:
	#gcc -I .. -c ../common/types/application.c
	#gcc -I .. -c ../common/types/periodic_metric.c
	gcc -I .. -c client_api/eardbd_api.c
	gcc -I .. -o client_api_test client_api_test.c eardbd_api.o
	gcc -I .. -o eardbd eardbd.c
