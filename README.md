# This is a port of MicroPython adapted for running on Temple OS (U).

Specifically I changed the port in ports/unix.  Even more specifically I changed the "minimal" version of this port.  The minimal version of this port for Temple OS contains an amount of functionality somewhere between the regular and minimal normal unix ports.  See source code for details.  Please don't try to compile this, it requires other external assembly code changes, modified C libraries, and many other toolchain specific changes to work.  I'm mainly putting it up for reference and license reasons.

The standard release Python.ISO.C will only run on [TempleOS (U)](https://github.com/tosrevive/TempleOS-Unofficial)!

There is a legacy version (Python_TOS_NO_FP.ISO.C) without floating point support that will run on vanilla [Temple OS](http://templeos.org).

Example usage:

// Mount the Python ISO

MountFile("Python.ISO.C");

// Assuming this is the first ISO file you mounted, otherwise change M: as appropriate

Cd("M:/");

#include "LoadPy"

Python;

See the [MicroPython](http://www.micropython.org) project for more info, but obviously don't bother to contact them for support with this hacked up release as it is not an official port!
