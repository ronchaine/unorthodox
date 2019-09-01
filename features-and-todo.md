Roadmap
=======

legend
------
x = done
! = done, but untested
W = work in progress (branch after name)
  = unstarted

sooner
------

### Mathematics

- [x] `var_in_range`
- [x] `fract_iec559`
- [x] `round_iec559`
- [!] `round_icast`
- [x] `round`
- [x] `abs`
- [x] `modulo`
- [x] `remainder`
- [x] `minimum`
- [x] `maximum`

- [ ] `fixed_point`
  - [x] Basic functionality
  - [ ] cmath functions
    - [!] abs() // untested
    - [ ] fmod()
    - [ ] remainder()
    - [ ] max()
    - [ ] min()

    - [ ] exponents & logarithms
      - [ ] exp()
      - [ ] exp2()
      - [ ] expm1()
      - [ ] log()
      - [ ] log10()
      - [ ] log2()
      - [ ] log1p()
      - [ ] pow()
      - [ ] sqrt()
      - [ ] cbrt()

    - [ ] trigonometry
      - [ ] hypot()
      - [ ] sin()
      - [ ] cos()
      - [ ] tan()
      - [ ] asin()
      - [ ] acos()
      - [ ] atan()
      - [ ] atan2()
      - [ ] sinh()
      - [ ] cosh()
      - [ ] tanh()
      - [ ] asinh()
      - [ ] acosh()
      - [ ] atanh()

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

later
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
  - [!] sync API
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
