/*
 * This is a simple mmodule for HTTP Protocol implementation.
 * Check the RFC2616 document at link below for details.
 * (http://www.w3.org/Protocols/rfc2616/rfc2616.html)
 * 
 * @Author: Hyunha Park
 */
#ifndef HAPACHE_HTTP_H
#define HAPACHE_HTTP_H

#include "hapache.h"

#define SP         ' '
#define CR         '\r'
#define LF         '\n'
#define CRLF       "\r\n"
#define LINE_BRK   CRLF

#define HTTP_METHOD_INVALID 0
#define HTTP_METHOD_OPTIONS 1
#define HTTP_METHOD_GET     2
#define HTTP_METHOD_HEAD    3
#define HTTP_METHOD_POST    4
#define HTTP_METHOD_PUT     5
#define HTTP_METHOD_DELETE  6
#define HTTP_METHOD_TRACE   7
#define HTTP_METHOD_CONNECT 8
#define HTTP_METHOD_EXT     9

#define MAX_URI_LENGTH      8000 /* Will cause 414 Error if longer than this. */

struct http_request {
	/* Request-Line. (Request-Line = Method SP Request-URI SP HTTP-Version CRLF) */
	int method;
	char *request_uri;
	char *http_version;

	/* general-header. */

	/* request-header. */
	char *host;

	/* entitiy-header. */

};

struct http_response {
	/* Status-Line. (Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF) */
	char *http_version;
	
};

void http_get_request (struct cbuf *, struct http_request **);
void http_send_response (struct cbuf *, struct http_request *);
int http_check_end(int, char *);

#endif
