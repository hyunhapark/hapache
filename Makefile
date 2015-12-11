all:
	gcc -o hapache hapache.c http.c list.c mime.c
tags:
	ctags `find . -name '*.[ch]'`
clean:
	rm -f hsh tags
