CXX=clang++
CC=clang
RM=rm -f
CPPFLAGS:=-g3 -ggdb -fsanitize=address,fuzzer -fprofile-instr-generate -fcoverage-mapping -O1 $(CPPFLAGS)
LDFLAGS:=-g3 -ggdb -fsanitize=address,fuzzer -fprofile-instr-generate -fcoverage-mapping -O1 $(LDFLAGS)

PREFIX ?= /usr/local

SRCS=src/fuzz_test.cpp

all: libfuzz_test.a

libfuzz_test.a: src/fuzz_test.o
	$(AR) r libfuzz_test.a src/fuzz_test.o
	ranlib libfuzz_test.a

src/fuzz_test.o: src/fuzz_test.cpp include/fuzz_test.h include/fuzzer/FuzzedDataProvider.h
	$(CXX) $(LDFLAGS) -I./include -c -o src/fuzz_test.o src/fuzz_test.cpp

clean:
	$(RM) src/fuzz_test.o

distclean: clean
	$(RM) libfuzz_test.a

install: libfuzz_test.a
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libfuzz_test.a $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 include/fuzz_test.h $(DESTDIR)$(PREFIX)/include/
	install -m 644 include/fuzzer/FuzzedDataProvider.h $(DESTDIR)$(PREFIX)/include/
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 bin/fuzz_test $(DESTDIR)$(PREFIX)/bin/
