#include "sgl.h"
#include <stdio.h>
#include <string.h>

void sglClear(sglBuffer* buffer, int width, int height) {
	memset(buffer->pixels, 0, width * height * sizeof(uint32_t));
}

void sglSetPixel(sglBuffer* buffer, int x, int y, size_t bufferWidth, uint32_t color) {
	buffer->pixels[x + y * bufferWidth] = color;
}

sglBuffer* sglCreateBuffer(uint32_t* pixels, int width, int height) {
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->width = width;
	b->height = height;
	b->pixels = pixels;
	return b;
}

sglBuffer* sglCreateNewBuffer(int width, int height) {
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->width = width;
	b->height = height;
	b->pixels = malloc(width * height * sizeof(uint32_t));
	return b;
}

void sglDestroyBuffer(sglBuffer* buffer) {
	free(buffer->pixels);
	free(buffer);
}
