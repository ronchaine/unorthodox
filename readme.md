work in progress C++20 utility library
======================================
Unorthodox (as opposed to standard) is stuff that I often need, but have no good
go-to solution to use.  It shouldn't be a full-scale replacement for any specific
library, but to give reasonable default that can be used if more heavyweight
libraries are undesirable.

Goals
=====
* Similar style to the standard library
* Ease of use
* Cross-platform (should work at least on Windows, Linux with glibc/musl and FreeBSD)
* Usable by anybody with C++20-enabled compiler

Scope
=====
Unorthodox should consist of

* Mathematics
* Graphics
* Audio
* Networking
* Utility tools

All of which provide high-level interface to commom functionality, they do not need to replace
libraries specifically meant for those purposes.  (We are not making another Eigen and add some
extras in the top of it!)

Mathematics
-----------
- Fixed point type
- Geometry
- Vectors, Matrices and Quaternions

Graphics
--------
- Colourspaces
- Simple 2D/3D rendering API
- API for opening window and reading input
  - Reference implementation (using SDL2 or GLFW?)
- Plotting

Audio
-----
- Basic 3D audio playing

Networking
----------
- Raw sockets
- UDP/TCP
- SSL sockets (using OpenSSL or LibreSSL(?))
- Websockets
- Various RFCs (probably sometime way later)

Utility
-------
- Enumerate (unless it gets into standard first)
- Physical units
- Common event system

Compiling
=========
The codebase uses some C++20 features, such as Concepts, and will use more of them,
especially <=> and modules as they become available in clang. So currently a custom-
built version of clang is required to compile.

Later on, plan is to support at least all of the big three compilers
