Roadmap
=======

0.1
---

### Mathematics

- [x] `var_in_range`
- [x] `fract_iec559`
- [x] `round_iec559`
- [ ] `round_icast` (missing tests)
- [x] `round`
- [x] `abs`
- [x] `modulo`
- [x] `remainder`
- [x] `minimum`
- [x] `maximum`

### Colour
- [x] Conversion functions
  - [x] HSV to RGB
  - [x] RGB to HSV
- [ ] Colour spaces
  - [x] Basic support for RGB colourspaces
  - [ ] Basic support for greyscale colourspaces
  - [ ] Basic support for cylinderical colourspaces

### Geometry
- [ ] Simple shapes
  - [ ] Triangle
  - [ ] Quad
  - [ ] Circle
  - [ ] Sphere
- [ ] Collisions
- [ ] Triangulation

Later
-----

### Utility
- [ ] Enumerate

### Colour
- [ ] Conversion functions
  - [ ] Wavelength to RGB
  - [ ] Temperature to RGB
- [ ] Colour spaces
  - [ ] Basic support for CIE colourspaces

### Geometry
- [ ] Simple shapes
  - [ ] Polygon
    - [ ] Convex
    - [ ] Concave
- [ ] Compond shapes

### Physical units
- [ ] Length
- [ ] Time
  - [ ] Integrate std::chrono

### Event system
- [ ] Window events from Video IO
- [ ] Network events
- [ ] Timer events
- [ ] Filesystem events
- [ ] Variable change events

### Video IO
- [ ] Basic video target
  - [ ] Example GLFW target
  - [ ] Example File target
- [ ] Basic event system
  - [ ] Keyboard
  - [ ] Mouse
  - [ ] Touch
  - [ ] Stylus

### Plotting
- [ ] Plot class

  
### Networking
- [x] UDP/TCP sockets
  - [ ] sync API (missing tests)
  - [ ] async callback API
- [ ] SSL socket wrapper
  - [ ] Certificate handling
    - [ ] Linux / BSD
    - [ ] Windows
    - [ ] OS X


Ideas to explore
================

unorthodox::opaque
------------------
used like

enum Color : unorthodox::opaque
{
    Blaa,
    Blaa2,
}

int value = Blaa; // error

unorthodox::error
-----------------
error handling wrapper that can be chosen
with compile-time-switch to throw or not?

unorthodox::file
----------------
because does someone actually use fstream?
