This is Fragmon 0.2 

This is an old unmaintained project for talking to first person shooter
game servers like Quake 1,2,3,4 and Counterstrike. It grabs the basic
overview info from the server and formats it on the screen. Generally,
this includes a list of players logged into the server, your ping time,
current map, and so forth. Fragmon can also function as an IRC score
bot for broadcasting game scores.

To compile manually you'll need wxWidgets 2.6 and Python 2.4. 
pywrappers.cpp uses a lot of template functions so a recent compiler
may be needed. 

Edit Makefile to point to wx-config and the Python library and headers.
You may need to adjust some of the linker options, depending on your
OS.

So far Fragmon's been tested (compiled) on:
    Debian Sarge
    FreeBSD 6.0
    Windows

Hit F1 to see the basic help.

cd.rattan@gmail.com
