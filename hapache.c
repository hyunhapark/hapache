#include "hapache.h"
#include "http.h"
#include "list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

static char *webroot;
static struct sockaddr_in listen_addr;

struct list cbuf_list; // TODO : use hash queue instead of list

void
server_init () {
	webroot = WEBROOT_DIR;
	chroot (webroot);
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(PORT);
	memset (&listen_addr.sin_zero, 0, (size_t) 8);
	list_init (&cbuf_list);
}

static void
server_check_clients (int listenfd, struct pool *p) {
	int s, n;
	for ( s = 3; s < p->maxfd+1 && p->nready > 0; s++ ) {
		if (s == listenfd) continue;
		if (FD_ISSET(s, &p->master_set) && FD_ISSET(s, &p->ready_set)) {
			FD_CLR(s, &p->ready_set);
			p->nready--;
			struct cbuf *cbuf = NULL;  /* Client buffer. */
			struct list_elem *e = NULL;
			for (e = list_begin (&cbuf_list); e != list_end (&cbuf_list); e = list_next (e)){
				if (list_entry (e, struct cbuf, elem)->fd == s){
					cbuf = list_entry (e, struct cbuf, elem);
					break;
				}
			}
			if (!cbuf) { /* If there's no buffer, create one. */
				cbuf = (struct cbuf *) calloc (sizeof(struct cbuf), 1);
				cbuf->fd = s;
				cbuf->n = 0;
				list_push_back (&cbuf_list, &cbuf->elem);
			}
			if ((n = read(s, cbuf->buf + cbuf->n, REQBUFSIZE - cbuf->n)) > 0){
				cbuf->n += n;
				if ( !http_check_end (cbuf->n, cbuf->buf) ) { /* Request is not end yet. */
					if (cbuf->n == REQBUFSIZE) {
						/* Since we only implemented the method GET,
						   just truncate the remain as most servers do for the
						   large GET request. */
						//TODO
					}
				} else {
					struct http_request *req;
					http_get_request (cbuf, req);
					http_send_response (cbuf, req);
					/* TODO : chage this send asyncronous.
					   http_send_response_async (cbuf, req); */
					FD_CLR(s, &p->master_set);
					if (s == p->maxfd) {
						do p->maxfd--;
						while (!FD_ISSET(p->maxfd, &p->master_set));
					}
				}
			} else if (n < 0) { /* Unexpected close?? or some other things? */
				fprintf(stderr,"DEBUG L%d\n", __LINE__); fflush(stderr); //XXX
				/* TODO : free buffer and close? socket. */
				/* TODO : check errorno */
			} else { /* EOF (n==0) */
				/* Free buffer (if exists) and close socket. */
				fprintf(stderr,"DEBUG L%d\n", __LINE__); fflush(stderr); //XXX
				if (cbuf!=NULL) {
					list_remove (&cbuf->elem);
					free (cbuf);
				}
				close(s);
				FD_CLR(s, &p->master_set);
				if (s == p->maxfd) {
					do p->maxfd--;
					while (!FD_ISSET(p->maxfd, &p->master_set));
				}
			}
		}
	}
}

int
server_start () {
	int listenfd=0, connfd=0, tmpfd=0, val=0;
	int yes=1;
	struct pool p;

	if ( (tmpfd = open(PIDLOG_PATH, O_CREAT | O_TRUNC | O_RDWR,
			00644)) == -1 ) {
		fprintf(stderr, "Failed to log pid.\n");
		exit(1);
	}
	pid_t pid = getpid();
	char spid[32];
	sprintf(spid, "%d", (int) pid);
	write(tmpfd, spid, strlen(spid));

	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		perror("Failed creating socket.");
		exit(1);
	}

	if ( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1 ) {
		perror("Reusing address failed.");
		exit(1);
	}

	if ( bind(listenfd, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1 ) {
		perror("Bind failed.");
		exit(1);
	}

	if ( listen(listenfd, BACKLOG_CNT) == -1 ) {
		perror("Listen failed.");
		exit(1);
	}

	p.maxfd = listenfd;
	FD_ZERO(&p.master_set);
	FD_SET(listenfd, &p.master_set);

	while (1) {
		p.ready_set = p.master_set;
		p.nready = select (p.maxfd+1, &p.ready_set, NULL, NULL, NULL);
		if (FD_ISSET(listenfd, &p.ready_set)) {
			int caddrlen=0;
			struct sockaddr_in caddr;
			connfd = accept (listenfd, (struct sockaddr *)&caddr,
					&caddrlen);
			FD_SET(connfd, &p.master_set);
			if (connfd > p.maxfd) p.maxfd = connfd;
			FD_CLR(listenfd, &p.ready_set);
			p.nready--;
		}
		server_check_clients (listenfd, &p);
	}

	return 0;
}


int
main (int argc, char *argv[])
{
	server_init ();
	server_start ();
	return 0;
}
