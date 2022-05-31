# SGL

> **S**oftware **G**raphics **L**ibrary

Created with 🤕 in C!

Lots of things inspired from SDL :D

## Usage

Copy and paste `sgl.h` into your project and do this:

    #define SGL_IMPLEMENTATION

in *one* C/C++ file before you include the `cidr.hpp` file.

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
	* [ ] Textured triangles ⌨️
* [x] Images (using stb_image.h)
	* [x] Nearest neighbor sampling
	* [x] Save to disk
	* [ ] Linear sampling
	* [ ] Wrapping options
* [x] Text
	* [x] Simple text rendering
	* [x] Kerning
	* [x] Alignment
	* [ ] Size
	* [ ] Shadow
* [ ] Shaders

### Utility

* [x] Alpha blending
* [ ] Gamma correction
* [ ] HSV
* [ ] HSL
* [ ] (ordered) Dithering (as a shader? depends on pixel format?)

### Other

* [x] implement `sglGetError()` <https://wiki.libsdl.org/SDL\_GetError>
* [x] live demo code update
* [x] generalize `sglBuffer`'s and `sglBitmap`'s `getPixel()` function to one
* [x] make the library header only
* [ ] add more pixel formats
* [ ] allow drawing extended characters
* [ ] text shadows
* [ ] *optimize*
	* [ ] circle drawing
	* [ ] arc drawing
	* [ ] triangles
