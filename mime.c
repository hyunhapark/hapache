#include "mime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *mimes[512] = {

	"html:text/html",
	"htm:text/html",
	"shtml:text/html",
	"css:text/css",
	"xml:text/xml",
	"rss:text/xml"
	"gif:image/gif",
	"jpeg:image/jpeg",
	"jpg:image/jpeg",
	"js:application/x-javascript",
	"atom:application/atom+xml",

	"mml:text/mathml",
	"txt:text/plain",
	"jad:text/vnd.sun.j2me.app-descriptor",
	"wml:text/vnd.wap.wml",
	"htc:text/x-component",

	"png:image/png",
	"tif:image/tiff",
	"tiff:image/tiff",
	"wbmp:image/vnd.wap.wbmp",
	"ico:image/x-icon",
	"jng:image/x-jng",
	"bmp:image/x-ms-bmp",
	"svg:image/svg+xml",
	"svgz:image/svg+xml",

	"jar:application/java-archive",
	"war:application/java-archive",
	"ear:application/java-archive",
	"json:application/json",
	"hqx:application/mac-binhex40",
	"doc:application/msword",
	"pdf:application/pdf",
	"ps:application/postscript",
	"eps:application/postscript",
	"ai:application/postscript",
	"rtf:application/rtf",
	"xls:application/vnd.ms-excel",
	"ppt:application/vnd.ms-powerpoint",
	"wmlc:application/vnd.wap.wmlc",
	"kml:application/vnd.google-earth.kml+xml",
	"kmz:application/vnd.google-earth.kmz",
	"7z:application/x-7z-compressed",
	"cco:application/x-cocoa",
	"jardiff:application/x-java-archive-diff",
	"jnlp:application/x-java-jnlp-file",
	"run:application/x-makeself",
	"pl:application/x-perl",
	"pm:application/x-perl",
	"prc:application/x-pilot",
	"pdb:application/x-pilot",
	"rar:application/x-rar-compressed",
	"rpm:application/x-redhat-package-manager",
	"sea:application/x-sea",
	"swf:application/x-shockwave-flash",
	"sit:application/x-stuffit",
	"tcl:application/x-tcl",
	"tk:application/x-tcl",
	"der:application/x-x509-ca-cert",
	"pem:application/x-x509-ca-cert",
	"crt:application/x-x509-ca-cert",
	"xpi:application/x-xpinstall",
	"xhtml:application/xhtml+xml",


	"*:application/octet-stream",  /* IMPORTANT!! This should be the last one.
									  New ones should be added above this line. */
};

const char *
get_mime_from_ext (char *ext) {
	const char *mime=NULL;
	//TODO ext -> tolower.
	int i = 0, n = strlen(ext);
	for (i=0; i<n; i++)
		if(ext[i]>'A' && ext[i]<'Z') ext[i] += 32;

	int len = strlen(ext);
	char *find = strcpy(malloc(len+2), ext); //TODO : malloc -> handle error.
	find[len] = ':'; find[len+1] = 0;

	i=0;
	while (mimes[i]) {
		if (strstr(mimes[i], find)==mimes[i]){
			int j;
			len = strlen(mimes[i]);
			for (j=0;j<len;j++){
				if (mimes[i][j]==':'){
					mime = mimes[i]+j+1;
					break;
				}
			}
			break;
		}
		i++;
	}

	free (find);

	if (!mime) mime="application/octet-stream";

	return mime;
}
