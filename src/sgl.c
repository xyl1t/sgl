#include "sgl.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


sglPixelFormat* sglCreatePixelFormat(sglPixelFormatEnum format) {
	printf("before\n");
	sglPixelFormat* pf = malloc(sizeof(sglPixelFormat));
	printf("after %lu\n", sizeof(sglPixelFormat));
	pf->format = format;

#define SGL_FORMAT(a, b, c, d) \
	pf->a##mask = 0xff000000; \
	pf->b##mask = 0x00ff0000; \
	pf->c##mask = 0x0000ff00; \
	pf->d##mask = 0x000000ff; \
	pf->a##shift = 24; \
	pf->b##shift = 16; \
	pf->c##shift =  8; \
	pf->d##shift =  0;

	switch (format) {
		case SGL_PIXELFORMAT_ABGR8888:
			SGL_FORMAT(r, g, b, a);
			break;

		case SGL_PIXELFORMAT_BGRA8888:
			SGL_FORMAT(a, r, g, b);
			break;

		case SGL_PIXELFORMAT_RGBA8888:
			SGL_FORMAT(a, b, g, r);
			break;

		case SGL_PIXELFORMAT_ARGB8888:
			SGL_FORMAT(b, g, r, a);
			break;
	}

#undef SGL_FORMAT

	return pf;
}

sglBuffer* sglCreateBuffer(uint32_t* pixels, int width, int height, 
		sglPixelFormatEnum format) {
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->width = width;
	b->height = height;
	b->pixels = pixels;
	b->format = sglCreatePixelFormat(format);
	return b;
}

sglBuffer* sglCreateNewBuffer(int width, int height, sglPixelFormatEnum format) {
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->width = width;
	b->height = height;
	b->pixels = malloc(width * height * sizeof(uint32_t));
	b->format = sglCreatePixelFormat(format);
	return b;
}

void sglDestroyBuffer(sglBuffer* buffer) {
	free(buffer->pixels);
	free(buffer->format);
	free(buffer);
}

void sglClear(sglBuffer* buffer, int width, int height) {
	memset(buffer->pixels, 0, width * height * sizeof(uint32_t));
}

void sglSetPixel(sglBuffer* buffer, int x, int y, uint32_t color) {
	buffer->pixels[x + y * buffer->width] = color;
}

uint32_t sglToColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a, sglPixelFormat* pf) {
	uint32_t color = 0x0;

	color = (r << pf->rshift) +
			(g << pf->gshift) +
			(b << pf->bshift) +
			(a << pf->ashift);

	return color;
}

