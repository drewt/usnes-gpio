usnes-gpio - RPi SNES controller driver
==============================================

Copyright (C) 2014 Drew Thoreson

About
-----

usnes is a driver for the Super Nintendo controller, connected over GPIO on the
Raspberry Pi.  It uses the Linux uinput interface to emulate a USB input device
from user space.   usnes will work with an unmodified SNES controller, provided
you are able to connect it to Raspberry Pi's GPIO pins.

Pinout
------

Below is the pinout for the SNES controller (copied from
http:/www.gamefaqs.com/snes/916396-super-nintendo/faqs/5395).  You will need to
connect the clock, latch and data pins on the controller to GPIO pins on the
Pi, and tell the daemon about the mapping.  This can be done through command
line options, or by editing the values for DEFAULT\_\*\_PIN in config.h.

    ,------------------------------.
    | (1) (2) (3) (4) | (5) (6) (7) )
    '------------------------------`

    Pin    Description    Color of wire in cable
    ---    -----------    ----------------------
    1      +5v            White
    2      Data Clock     Yellow
    3      Data Latch     Orange
    4      Serial Data    Red
    5      ?              No wire
    6      ?              No wire
    7      Ground         Brown

NOTE: I have had success driving controllers at 3.3v, but as always YMMV.

Do not connect pin 1 to 5v unless you know what you are doing.  You may break
your Pi.  I take no responsibility for what happens.

Configuration
-------------

By default, usnes uses the following button/key mapping:

    SNES BUTTON    KEY
    -----------    ---
    A              a
    B              b
    X              x
    Y              y
    L              l
    R              r
    D-Pad Up       Up
    D-Pad Down     Down
    D-Pad Left     Left
    D-Pad Right    Right
    Start          Enter
    Select         Escape

This can be changed by editing the file config.h prior to compiling.

Build/Install
-------------

The bcm2835 library is a dependency.  It should be installed prior to building.

    $ make
    # make install

Running
-------

Using the default pin mapping:

    # usnes

Configuring the pins with command line options (note that usnes uses the
BCM2835 pin numbering):

    # usnes --clock 21 --latch 20 --data 16

Git Repository
--------------

https://github.com/drewt/usnes-gpio

    $ git clone https://github.com/drewt/usnes-gpio
