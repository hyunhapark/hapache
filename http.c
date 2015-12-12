/**
  * HTTP Protocal Parsing Module.
  * + URL Parsing??
  *
  * @Author: Hyunha Park
  */
#include "http.h"
#include "hapache.h"
#include "mime.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#define strcats(s1, s2, cnt)     strcat((s1)+(cnt),(s2));strcat((s1),CRLF);(cnt)+=(strlen(s2)+2)
#define MIN(a,b)   ((a)<(b)?(a):(b))

/* Get http_request object from client buffer cbuf. */
void
http_get_request (struct cbuf *cbuf, struct http_request **_req) {
	
	char *s=cbuf->buf;
	int i, cnt=0, n=cbuf->n;

	struct http_request *req = (struct http_request *) calloc (sizeof(struct http_request), 1);

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
	req->request_uri[i]=0;
	
	s += cnt+1;
	for (cnt=1;cnt<n;cnt++)
		if (s[cnt-1]==CR && s[cnt]==LF) break;

	/*TODO*/
	req->http_version = strcpy (malloc(1), "");
	req->host = strcpy (malloc(1), "");
	*_req = req;
}

void
http_send_response (struct cbuf *cbuf, struct http_request *req) {
	int fd = cbuf->fd;
	list_remove (&cbuf->elem);
	int cnt = 0;
	char res[RESBUFSIZE];
	res[0] = 0;
	struct stat st;
	//TODO : index file, autoindex, ...
	if (stat(req->request_uri, &st)!=-1 && st.st_ino && (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) ) {
		off_t len = st.st_size;
		int file = open (req->request_uri, O_RDONLY);
		char *fpath = strcpy (malloc(strlen(req->request_uri)+1), req->request_uri);
		char *p = strtok(fpath, ".");
		char ext[128]={0};
		if (p==NULL) {
			strcpy(ext, "*");
		} else {
			char *old_p = p;
			while ((p=strtok (NULL, ".")) != NULL) old_p=p;
			p=old_p;
			if ((p=strtok(p, "/"))==NULL) {
				strcpy (ext, old_p);
			} else {
				old_p = p;
				while((p=strtok(NULL, "/"))!=NULL) old_p=p;
				strcpy (ext, old_p);
			}
		}
		const char *mime = get_mime_from_ext(ext);
		char *ctype = strcat (strcpy (malloc(128), "Content-Type: "), mime);

		// TODO handle error.
		strcats (res, "HTTP/1.1 200 OK", cnt); //FIXME
		strcats (res, "Server: Hapache/0.0.1", cnt);
		strcats (res, "Connection: close", cnt); /* Force close. */
		strcats (res, ctype, cnt); //FIXME
		free (ctype);
		strcat (res, "Content-Length: "); cnt += strlen("Content-Length: ");
		char tmp[512]={0};
		snprintf (tmp, 500, "%d", (int) len);
		strcats (res, tmp, cnt);
		strcats (res, "", cnt);
		int n=0;
		while ( (n=read(file, res+cnt, RESBUFSIZE-cnt)) > 0 ) {
			cnt += n;
			write (fd, res, cnt);
			cnt=0;
		}
		close (file);
	} else {
		//TODO : check errno. something like permission error(return 403).
		strcat (res, "HTTP/1.1 404 Not Found"); //FIXME
		strcat (res, CRLF);
		strcat (res, "Server: Hapache/0.0.1");
		strcat (res, CRLF);
		strcat (res, "Connection: close"); /* Force close. */
		strcat (res, CRLF);
		strcat (res, CRLF);
		char tmp[8192];
		snprintf(tmp, 8191, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\n<p>The requested URL %s was not found on this server.</p>\n<hr>\n<address>%s Server at %s Port %d</address>\n</body></html>\n", req->request_uri, SERVER_NAME, req->host, PORT);
		strcat (res, tmp);
		write (fd, res, strlen(res));
	}
	free (req->request_uri);
	free (req->http_version);
	free (req->host);
	free (req);
	if (!close (fd)){
		list_remove (&cbuf->elem);
		free (cbuf);
	}
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

