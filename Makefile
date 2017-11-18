C++ = g++
CPPFLAGS = -Wall -Wextra -pedantic -std=c++11

DEPHEADS = Connection.h Database.h Decode.h Encode.h Error.h ResponseBuilder.h Codes.h
DEPS = Connection.cpp Database.cpp Decode.cpp Encode.cpp Error.cpp ResponseBuilder.cpp main.cpp

EXES = myldap
ZIPFILES = *.cpp *.h Makefile *.md *.pdf
ZIPNAME = xberky02.tar

all: myldap

myldap: $(DEPHEADS) $(DEPS)
	$(C++) $(CPPFLAGS) -o $@ $^

.PHONY: clean zip run

clean:
	rm -f $(EXES) $(ZIPNAME)

zip:
	tar -czvf $(ZIPNAME) $(ZIPFILES)

run: myldap
	./myldap -p 38900 -f ./isa2017-ldap.csv