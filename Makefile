AR=ar
CC=g++
CFLAGS=-Wall -Werror -g
#CFLAGS=-Wall -Werror -O3
RANLIB=ranlib

all: demo libhll.a

demo: demo.o libhll.a -lssl -lcrypto
	$(CC) -o demo demo.o libhll.a

libhll.a: bitwise_ops.o hll.o
	$(AR) rcs libhll.a bitwise_ops.o hll.o
	$(RANLIB) libhll.a

.cc.o:
	$(CC) $(CFLAGS) -c $<

.PHONY:
clean:
	-rm -f demo *.o libhll.a *~ .*~

.PHONY:
count:
	wc -l *.h *.cc
