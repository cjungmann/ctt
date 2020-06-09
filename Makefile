BASEFLAGS = -Wall -Werror -m64
LIB_CFLAGS = ${BASEFLAGS} -I. -fPIC -shared -fvisibility=hidden
LIB_DBFLAGS = ${LIB_CFLAGS} -ggdb
LOCAL_LINK = -Wl,-R -Wl,. libctt.so

BASEFLAGS = -Wall -Werror -ggdb
# MODULES = procopts.o line_reader.o key_reader.o picker.o
MODULES = ctt.o cusses.o procopts.o line_reader.o key_reader.o

CC = gcc

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

cusses.o : cusses.h cusses.c
	$(CC) ${LIB_CFLAGS} -c -o cusses.o cusses.c

line_reader.o : line_reader.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o line_reader.o line_reader.c

key_reader.o : key_reader.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o key_reader.o key_reader.c

procopts.o : procopts.c ctt.h
	$(CC) ${LIB_CFLAGS} -c -o procopts.o procopts.c

install :
	install -D --mode=755 libctt.so /usr/lib
	install -D --mode=755 ctt.h     /usr/local/include

uninstall :
	rm -f /usr/lib/libctt.so
	rm -f /usr/local/include/ctt.h

clean :
	rm -f $(MODULES)
	rm -f libctt.so
	rm -f ctt cusses procopts key_reader line_reader test

