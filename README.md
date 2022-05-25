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
* [x] Triangles
	* [x] Stroke
	* [x] Fill
	* [x] Color interpolation
	* [ ] Textured triangles
* [x] Images (using stb_image.h)
	* [x] Nearest neighbor sampling
	* [x] Saving
	* [ ] Linear sampling
	* [ ] Wrapping options
* [x] Text ⌨️
	* [x] Simple text rendering
	* [ ] Kerning
	* [ ] Alignment
	* [ ] Size
	* [ ] Shadow
* [ ] Shaders

### Utility

* [ ] HSV
* [ ] HSL
* [ ] Alpha blending
* [ ] Gamma correction
* [ ] (ordered) Dithering

### Other

* [x] implement `sglGetError()` <https://wiki.libsdl.org/SDL\_GetError>
* [x] live demo code update
* [ ] add more pixel formats
* [ ] make the library header only
* [x] generalize `sglBuffer`'s and `sglBitmap`'s `getPixel()` function to one
* [ ] allow drawing extended characters
* [ ] *optimize*
	* [ ] circle drawing
	* [ ] arc drawing
	* [ ] triangles

