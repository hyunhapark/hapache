/**
  * HTTP Protocal Parsing Module.
  * + URL Parsing??
  *
  * @Author: Hyunha Park
  */
#include "http.h"
#include "hapache.h"
#include <stdlib.h>
#include <string.h>

/* Get http_request object from client buffer cbuf. */
void
http_get_request (struct cbuf *cbuf, struct http_request *req) {
	//TODO
	char *s=cbuf->buf;
	int cnt=0, n=cbuf->n;

	for (;cnt<n;cnt++)
		if (s[cnt]==SP) break;
	char *s_method = (char *) malloc (cnt+1);
	for (i=0;i<cnt;i++)
		s_method[i]=s[i];
	s_method[i]=0;
	if (!strcmp(s_method, "GET")) {
		req->method = HTTP_METHOD_GET;
	} else {  /* Currently we only supports GET. */
		req->method = HTTP_METHOD_INVALID;
	}
	free (s_method);

	s += cnt+1;
	for (cnt=0;cnt<n;cnt++)
		if (s[cnt]==SP) break;
	req->request_uri = (char *) malloc (cnt+1);
	for (i=0;i<cnt;i++)
		req->request_uri[i] = s[i];
	
	s += cnt+1;
	for (cnt=1;cnt<n;cnt++)
		if (s[cnt-1]==CR && s[cnt]==LF) break;

	/*TODO*/
	req->http_version = strcpy (malloc(1), "");
	req->host = strcpy (malloc(1), "");
}

void
http_send_response (struct cbuf *cbuf, struct http_request *req) {
	int fd = cbuf->fd;
	list_remove (&cbuf->elem);
	free (cbuf);
	int cnt = 0;
	char *res = (char *) calloc (512, 1);
	int file = open(req->request_uri, O_RDONLY);
	if (file<0) {
		//TODO : check errno. something like permission error.
		strcat (res, "HTTP/1.1 404 Not Found"); //FIXME
		strcat (res, CRLF);
		strcat (res, "Server: Hapache/0.0.1");
		strcat (res, CRLF);
		strcat (res, "Connection: close"); /* Force close. */
		strcat (res, CRLF);
		strcat (res, CRLF);
	} else {
		char buf[RESBUFSIZE];
		struct stat f_stat;
		fstat(file, &f_stat);
		off_t len = f_stat.st_size;

		strcat (res, "HTTP/1.1 200 OK"); //FIXME
		strcat (res, CRLF);
		strcat (res, "Server: Hapache/0.0.1");
		strcat (res, CRLF);
		strcat (res, "Connection: close"); /* Force close. */
		strcat (res, CRLF);
		strcat (res, "Content-Type: text/html"); //FIXME
		strcat (res, CRLF);
		strcat (res, "Content-Length: "); //FIXME TODO XXX-hard coded number
		char tmp[512];
		sprintf(tmp, "%d", len);
		strcat (res, CRLF);
		strcat (res, CRLF);
		strcat (res, "<h1>Hello server!!</h1>");
		strcat (res, CRLF);
	}
	write (fd, res, strlen(res));
	free (res);
	free (req->request_uri);
	free (req->http_version);
	free (req->host);
	free (req);
	close (fd);
}

int
http_check_end(int n, char *buf){
	int i;

	if(n<4) return 0;

	for (i=3;i<n;i++) {
		//check for double CRLF
		if (buf[i-3]==CR && buf[i-2]==LF && buf[i-1]==CR
				&& buf[i]==LF){
			return i+1;
		}
	}
	return 0;
}

void http_url_parse(char *url){
}

