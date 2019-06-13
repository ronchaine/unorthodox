Roadmap
=======

1.0
---

### Mathematics
#### Functions required for constexpring 

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
  - [ ] Basic support for RGB colourspaces
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

### Video IO
- [ ] Basic video target
  - [ ] GLFW target
  - [ ] File target
- [ ] Basic event system
  - [ ] Keyboard
  - [ ] Mouse
  
Later
-----
### Colour
- [ ] Conversion functions
  - [ ] Wavelength to RGB
- [ ] Colour spaces
  - [ ] Basic support for CIE colourspaces

### Geometry
- [ ] Simple shapes
  - [ ] Polygon
    - [ ] Convex
    - [ ] Concave
- [ ] Compond shapes

### Video IO
- [ ] Basic event system
  - [ ] Touch
  - [ ] Stylus

### Physical units
- [ ] Length
- [ ] Time
  - [ ] Integrate std::chrono
