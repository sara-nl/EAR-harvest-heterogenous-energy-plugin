#ifndef EAR_EARDBD_API_H
#define EAR_EARDBD_API_H

#include <database_cache/eardbd.h>

int eardbd_connect(char *host, int protocol);
int eardbd_is_connected();
int eardbd_disconnect();

int eardbd_send_ping();
int eardbd_send_periodic_metric(void *met);
int eardbd_send_application(void *app);


#endif //EAR_EARDBD_API_H
