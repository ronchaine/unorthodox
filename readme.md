work in progress
================

Requires C++20 concepts to compile, will use other C++20 features as well once compiler support improves


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
extras in the top of it!)  The libraries should keep themselves somewhat lightweight.

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

