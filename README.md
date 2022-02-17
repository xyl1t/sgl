# SGL

> **S**oftware **G**raphics **L**ibrary

Created with 🤕 in C!

Lots of things inspired from SDL :D

## Build the demo (Linux / Mac)

#### Dependencies

* A c99 compatible compiler
* SDL2
* CMake

#### Building

Clone the repo and execute these commands from the `sgl/` directory

```bash
mkdir build
cd build
cmake ..
make
./sgl
```

## TODO

### Core

Implement basic drawing functions

* [x] Point
* [x] Rectangles
* [x] Line
* [x] Circles
* [x] Arcs
* [ ] Triangles
	* [x] Stroke
	* [x] Fill
	* [x] Color interpolation
	* [ ] Textured triangles
* [ ] Images (using stb_image.h)
	* [ ] Linear sampling
	* [ ] Wrapping
* [ ] Text
* [ ] Shaders

### Utility

* [ ] HSV
* [ ] HSL
* [ ] Alpha blending
* [ ] Gamma correction
* [ ] (ordered) Dithering

### Other

* [x] implement `sglGetError()` <https://wiki.libsdl.org/SDL\_GetError>
* [ ] add more pixel formats
* [ ] make the library header only
* [x] live demo code update
* [ ] optimize
	* [ ] circle drawing
	* [ ] arc drawing
	* [ ] triangles

