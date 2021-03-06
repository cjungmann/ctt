BASEFLAGS = -Wall -Werror -m64
LIB_CFLAGS = ${BASEFLAGS} -I. -fPIC -shared -fvisibility=hidden
LIB_DBFLAGS = ${LIB_CFLAGS} -ggdb
LOCAL_LINK = -Wl,-R -Wl,. libctt.so

BASEFLAGS = -Wall -Werror -ggdb
# MODULES = process_options.o get_line.o get_keypress.o picker.o
MODULES = ctt.o cusses.o dir_iterate.o dir_walk.o get_line.o pick_from_list.o get_keypress.o process_options.o utilities.o
MANFILES = $(wildcard man/*)

CC = gcc

define install_man_pages
	cd man; gzip -k *
	cd man; mv *.gz /usr/share/man/man3
endef

define uninstall_man_pages
	rm -rf $(foreach mname, $(MANFILES), /usr/share/man/man3$(subst man,,$(mname)).gz)
endef

all : libctt.so test


test : test.c libctt.so
	$(CC) ${BASEFLAGS} -o test test.c ${LOCAL_LINK}
#
#
# Library and constituent object files:
#
libctt.so : $(MODULES)
	$(CC) ${LIB_CFLAGS} -o libctt.so $(MODULES)

ctt.o : ctt.h ctt.c
	$(CC) ${LIB_CFLAGS} -c -o ctt.o ctt.c

dir_iterate.o : dir_iterate.c
	$(CC) ${LIB_CFLAGS} -c -o dir_iterate.o dir_iterate.c

dir_walk.o : dir_walk.c
	$(CC) ${LIB_CFLAGS} -c -o dir_walk.o dir_walk.c

cusses.o : cusses.h cusses.c
	$(CC) ${LIB_CFLAGS} -c -o cusses.o cusses.c

get_line.o : get_line.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o get_line.o get_line.c

pick_from_list.o : pick_from_list.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o pick_from_list.o pick_from_list.c

get_keypress.o : get_keypress.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o get_keypress.o get_keypress.c

process_options.o : process_options.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o process_options.o process_options.c

utilities.o : utilities.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o utilities.o utilities.c

install :
	install -D --mode=755 libctt.so /usr/lib
	install -D --mode=755 ctt.h     /usr/local/include
	install -D --mode=755 cusses.h  /usr/local/include
	$(call install_man_page)

uninstall :
	rm -f /usr/lib/libctt.so
	rm -f /usr/local/include/ctt.h
	rm -f /usr/local/include/cusses.h
	$(call uninstall_man_pages)

clean :
	rm -f $(MODULES)
	rm -f libctt.so
	rm -f ctt cusses dir_iterate dir_walk get_keypress get_line pick_from_list process_options test

install_man :
	$(call install_man_pages)

uninstall_man :
	$(call uninstall_man_pages)
