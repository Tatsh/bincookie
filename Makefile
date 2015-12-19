LIBNAME := libbinarycookies
LIBVERSION = 0.0.1
LIBVERSION_SHORT = 0
CFLAGS := -std=c11 -pedantic -Wall
CC := gcc
DESTDIR := ./out

OBJECTS = bincookie.o

all: $(LIBNAME).so

install: $(LIBNAME).so
	mkdir -p $(DESTDIR)
	cp $(LIBNAME).so $(LIBNAME).so.$(LIBVERSION_SHORT) $(LIBNAME).so.$(LIBVERSION) $(DESTDIR)/

$(LIBNAME).so: $(LIBNAME).so.$(LIBVERSION_SHORT)
	ln -s $(LIBNAME).so.$(LIBVERSION_SHORT) $(LIBNAME).so

$(LIBNAME).so.$(LIBVERSION_SHORT): $(LIBNAME).so.$(LIBVERSION)
	ln -s $(LIBNAME).so.$(LIBVERSION) $(LIBNAME).so.$(LIBVERSION_SHORT)

$(LIBNAME).so.$(LIBVERSION): $(OBJECTS)
	$(CC) -shared -Wl,-soname,$(LIBNAME).so.$(LIBVERSION_SHORT) -o $(LIBNAME).so.$(LIBVERSION) $(OBJECTS)

$(OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

test: readbincook

readbincook: $(LIBNAME).so
	$(CC) -I. $(CFLAGS) -L. -o readbincook test/main.c -lbinarycookies

clean:
	rm -f $(LIBNAME).so* readbincook $(DESTDIR)/$(LIBNAME).so*
