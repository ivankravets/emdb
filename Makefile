# You can put your build options here
-include config.mk

CFLAGS=-I./src -Wall -g -march=native
LDFLAGS=-g -flto -L. -lemdb

SRCS=src/emdb.c

OBJS=obj/emdb.o
TEST=test/test.o


all: build

ifdef LARGE
CFLAGS += -DLARGE
endif

ifdef MEDIUM
CFLAGS += -DMEDIUM
endif

ifdef SMALL
CFLAGS += -DSMALL
endif

ifdef TINY
CFLAGS += -DTINY
endif

ifdef EMDB_MEMORY_STORAGE
CFLAGS += -DEMDB_MEMORY_STORAGE

obj/storage/memory.o: src/storage/memory.c src/storage/memory.h src/emdb.h
	$(CC) -c $(CFLAGS) src/storage/memory.c -o obj/storage/memory.o

test/memory.o: test/memory.c src/storage/memory.c src/storage/memory.h
	$(CC) -c $(CFLAGS) test/memory.c -o test/memory.o

OBJS += obj/storage/memory.o
TEST += test/memory.o

endif

ifdef EMDB_QUERY
CFLAGS += -DEMDB_QUERY

OBJS += obj/query.o
TEST += test/query.o

obj/query.o: src/query.h
	$(CC) -c $(CFLAGS) src/query.c -o obj/query.o

test/query.o: test/query.c
	$(CC) -c $(CFLAGS) test/query.c -o test/query.o

endif

ifdef EMDB_JSON
CFLAGS += -DEMDB_JSON -I./jsmn

OBJS += obj/json.o jsmn/jsmn.o
TEST += test/json.o

obj/json.o: src/json.h
	$(CC) -c $(CFLAGS) src/json.c -o obj/json.o

test/json.o: test/json.c
	$(CC) -c $(CFLAGS) test/json.c -o test/json.o

jsmn/jsmn.o: jsmn/jsmn.c
	$(CC) -c $(CFLAGS) jsmn/jsmn.c -o jsmn/jsmn.o

endif


doc:
	@doxygen Doxyfile



test/test.o: test/test.c
	$(CC) -c $(CFLAGS) test/test.c -o test/test.o



obj/emdb.o: src/emdb.c src/emdb.h
	$(CC) -c $(CFLAGS) src/emdb.c -o obj/emdb.o

build: $(OBJS) $(TEST)
	$(AR) -cvq libemdb.a $(OBJS)
	$(CC) $(TEST) $(LDFLAGS) -o test_runner

.PHONY: clean clean_src

test: build
	./test_runner --spec

clean: clean_src clean_test clean_lib

clean_src:
	@rm -f obj/*.o
	@rm -f obj/storage/*.o

clean_test:
	@rm -f test/*.o
	@rm -f test/test
	@rm -f ./test_runner

clean_lib:
	@rm -f libemdb.a
