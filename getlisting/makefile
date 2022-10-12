
CC = gcc

CPPFLAGS += -D _GNU_SOURCE

CFLAGS += -Wall -Werror

/tmp/getlisting: /tmp/getlisting.o /tmp/json.o
	$(CC) $^ -o $@

/tmp/getlisting.o: getlisting.c json.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

/tmp/json.o: json.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

json.c json.h: json.zb
	zebu -t fileio -i $< -o json

run: /tmp/getlisting
	/tmp/getlisting
	
valrun: /tmp/getlisting
	valgrind /tmp/getlisting
