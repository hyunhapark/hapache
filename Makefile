all:
	gcc -o hapache hapache.c http.c list.c mime.c -g -Wall -Wextra -Wno-unused
tags:
	ctags `find . -name '*.[ch]'`
clean:
	rm -f hapache tags
