#ifndef EAR_EARDBD_API_H
#define EAR_EARDBD_API_H

#include <database_daemon/eardbd.h>

int eardbd_start(char *host, int protocol);
int eardbd_send_application(void *app);
int eardbd_send_periodic_metric(void *met);
int eardbd_dispose();

#endif //EAR_EARDBD_API_H