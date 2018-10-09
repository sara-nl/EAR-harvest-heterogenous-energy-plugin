API
---
The Database Daemon offers a simple API (`eardbd_api.c`) to send by sockets metrics to the EARDBD daemon. The API just sends this data, which will be cached and stored in the database by a buffering technique.
