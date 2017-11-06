C++ = g++
CPPFLAGS = -Wall -Wextra -pedantic -std=c++11

DEPHEADS = Connection.h Database.h Decode.h Encode.h Error.h ResponseBuilder.h Codes.h
DEPS = Connection.cpp Database.cpp Decode.cpp Encode.cpp Error.cpp ResponseBuilder.cpp main.cpp

EXES = server
ZIPFILES = *.cpp *.h *.md Makefile
ZIPNAME = xberky02.zip

all: server

server: $(DEPHEADS) $(DEPS)
	$(C++) $(CPPFLAGS) -o $@ $^

.PHONY: clean zip run

clean:
	rm -f $(EXES) $(ZIPNAME)

zip:
	zip $(ZIPNAME) $(ZIPFILES)

run: server
	./server -p 38900 -f ./isa2017-ldap.csv