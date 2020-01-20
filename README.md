# This is a port of MicroPython adapted for running on Temple OS (U).

![Python on Temple OS (U)](https://raw.githubusercontent.com/tosrevive/micropython-tos/master/screenshot.png?)

Specifically I changed the port in ports/unix.  Even more specifically I changed the "minimal" version of this port.  The minimal version of this port for Temple OS contains an amount of functionality somewhere between the regular and minimal normal unix ports.  See source code for details.  Please don't try to compile this, it requires other external assembly code changes, modified C libraries, and many other toolchain specific changes to work.  I'm mainly putting it up for reference and license reasons.

The standard release Python.ISO.C will only run on [TempleOS (U)](https://github.com/tosrevive/TempleOS-Unofficial)!

There is a legacy version (Python_TOS_NO_FP.ISO.C) without floating point support that will run on vanilla [Temple OS](http://templeos.org).

### Example usage:

// Mount the Python ISO

MountFile("Python.ISO.C");

// Assuming this is the first ISO file you mounted, otherwise change M: as appropriate

Cd("M:/");

#include "LoadPy"

Python;

### Credits
- Thanks to Dr Jack Whitham for your work porting [frotz](https://github.com/jwhitham/frotz/) to Temple OS as described [here](https://www.jwhitham.org/2015/07/porting-third-party-programs-to-templeos.html).
- [MicroPython](http://www.micropython.org)
