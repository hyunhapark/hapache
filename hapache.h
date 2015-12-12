/*
 */
#ifndef HAPACHE_HAPACHE_H
#define HAPACHE_HAPACHE_H

#include "list.h"
#include <sys/socket.h>

#define SERVER_NAME "Hapache/0.0.1"
#define PORT        12345
#define WEBROOT_DIR "/var/www"
#define PIDLOG_PATH "/tmp/hapache.pid"
#define BACKLOG_CNT 8
#define REQBUFSIZE  8192
#define RESBUFSIZE  8192


struct pool {
	int maxfd; // largest descriptor in master_set
	int nready; // number of ready desc. from select
	fd_set master_set; // set of all active descriptors
	fd_set ready_set; // subset of desc. ready for reading
};

struct cbuf { /* Per Client Buffer. */
	int fd;
	int n;
	char buf[REQBUFSIZE];
	struct list_elem elem;
};

#endif /* HAPACHE_HAPACHE_H */
