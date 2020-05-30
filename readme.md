work in progress C++20 utility library
======================================
Unorthodox (as opposed to standard) is stuff that I often need, but have no good
go-to solution to use.  It shouldn't be a full-scale replacement for any specific
library, but to give reasonable default that can be used if more heavyweight
libraries are undesirable.

Dependencies
============
* A compiler that understands C++20
* tl::expected (header provided in-repo, will be replaced with std::expected when/if possible)
* optionally libcppevents

Goals
=====
* Similar style to the standard library
* Ease of use
* Cross-platform (should work at least on Windows, Linux with glibc/musl, FreeBSD and OpenBSD)
* Usable by anybody with C++20-enabled compiler

Scope
=====
Unorthodox should consist of

* Graphics
* Networking
* Mathematics

All of which provide high-level interface to common functionality, they do not need to replace
libraries specifically meant for those purposes.  (We are not making another Eigen and add some
extras in the top of it!)

Mathematics
-----------
- Fixed point type
- Geometry
- Vectors, Matrices and Quaternions
- Header-only

Graphics
--------
- Colourspaces
- Simple 2D/3D rendering API
- API for opening window and reading input
  - Reference implementation (using SDL2 or GLFW?)
- Plotting

Networking
----------
- Raw sockets
- UDP/TCP
- SSL sockets (using OpenSSL or LibreSSL(?)
- Websockets
- Various RFCs (probably sometime way later)

Event system
------------
- moved to its own project: https://github.com/ronchaine/libcppevents

Compiling
=========
The codebase uses some C++20 features, such as Concepts, and will use more of them,
especially <=> (and probably modules) as they become available in clang.

Later on, plan is to support at least all of the big three compilers

Currently the project uses meson build system to create the library files, cmake
support is probably a thing I want to look at, but it's not a priority, most of
Unorthodox can be used by just including the headers
