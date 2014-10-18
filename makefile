prefix  = /usr/local
bindir  = $(prefix)/bin
man1dir = $(prefix)/share/man/man1

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wno-unused-parameter -O2
LD      = gcc
LDFLAGS =
INSTALL = install

objects = daemon.o snes.o uinput.o
target = usnes

all: $(target)

daemon.o: config.h gpio.h snes.h
snes.o: config.h gpio.h snes.h
uinput.o: config.h uinput.h

$(target): $(objects)
	$(LD) $(LDFLAGS) -o $@ $^ -lbcm2835

install:
	$(INSTALL) -m755 -t $(bindir) $(target)
	$(INSTALL) -m644 -t $(man1dir) usnes.1

clean:
	rm -f $(objects) $(target)
