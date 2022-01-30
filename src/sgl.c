#include "sgl.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static int sgl_max(int a, int b) { return a > b ? a : b; }
static int sgl_min(int a, int b) { return a < b ? a : b; }

sglPixelFormat* sglCreatePixelFormat(sglPixelFormatEnum format)
{
	sglPixelFormat* pf = malloc(sizeof(sglPixelFormat));
	pf->format = format;

	pf->bitsPerPixel = sglGetPixelType(format) * 8;
	pf->bytesPerPixel = sglGetPixelType(format);

#define SGL_SET_FORMAT(a, b, c, d) \
	pf->a##mask = 0xff000000; \
	pf->b##mask = 0x00ff0000; \
	pf->c##mask = 0x0000ff00; \
	pf->d##mask = 0x000000ff; \
	pf->a##shift = 24; \
	pf->b##shift = 16; \
	pf->c##shift =  8; \
	pf->d##shift =  0;

	switch (format) {
		case SGL_PIXELFORMAT_ABGR32:
			SGL_SET_FORMAT(r, g, b, a);
			break;

		case SGL_PIXELFORMAT_BGRA32:
			SGL_SET_FORMAT(a, r, g, b);
			break;

		case SGL_PIXELFORMAT_RGBA32:
			SGL_SET_FORMAT(a, b, g, r);
			break;

		case SGL_PIXELFORMAT_ARGB32:
			SGL_SET_FORMAT(b, g, r, a);
			break;

		case SGL_PIXELFORMAT_ABGR4444:
			pf->bitsPerPixel = 16;
			pf->bytesPerPixel = 2;
			pf->rmask = 0xf000;
			pf->gmask = 0x0f00;
			pf->bmask = 0x00f0;
			pf->amask = 0x000f;
			pf->rshift = 12;
			pf->gshift = 8;
			pf->bshift = 4;
			pf->ashift = 0;
			break;

		case SGL_PIXELFORMAT_RGB332:
			pf->bitsPerPixel = 8;
			pf->bytesPerPixel = 1;
			pf->rmask = 0x07;
			pf->gmask = 0x38;
			pf->bmask = 0xC0;
			pf->amask = 0x00;
			pf->rshift = 0;
			pf->gshift = 3;
			pf->bshift = 6;
			pf->ashift = 0;
			break;
	}

#undef SGL_SET_FORMAT

	return pf;
}

bool sglHasIntersection(const sglRect* A, const sglRect* B)
{
	if (A && B) {
		// if (B->x >= A->x &&
		// 		B->x <= A->x + A->w &&
		// 		B->y >= A->y &&
		// 		B->y <= A->y + A->h) {
		if (A->x + A->w >= B->x &&
			A->x <= B->x + B->w &&
			A->y + A->h >= B->y &&
			A->y <= B->y + B->h) {
			return true;
		}
	}

	return false;
}

bool sglIntersectRect(const sglRect* A, const sglRect* B, sglRect* result)
{
	if (sglHasIntersection(A, B)) {
		result->x = sgl_max(A->x, B->x);
		result->y = sgl_max(A->y, B->y);
		int A_max_x = A->x + A->w;
		int B_max_x = B->x + B->w;
		int A_max_y = A->y + A->h;
		int B_max_y = B->y + B->h;
		result->w = sgl_min(A_max_x, B_max_x) - result->x;
		result->h = sgl_min(A_max_y, B_max_y) - result->y;

		return true;
	}

	return false;
}

sglBuffer* sglCreateBuffer(uint32_t* pixels, uint32_t startX, uint32_t startY,
		                   uint32_t width, uint32_t height,
                           sglPixelFormatEnum format)
{
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->pixels = pixels;
	b->pf = sglCreatePixelFormat(format);
	b->width = width;
	b->height = height;
	b->clipRect = (sglRect) {
		.x = 0,
		.y = 0,
		.w = width,
		.h = height,
	};

	SGL_DEBUG_PRINT("sgl buffer initialized\n");
	return b;
}

sglBuffer* sglCreateNewBuffer(uint32_t width, uint32_t height,
                              sglPixelFormatEnum format)
{
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->pixels = malloc(width * height * sglGetPixelType(format));
	b->pf = sglCreatePixelFormat(format);
	b->width = width;
	b->height = height;
	b->clipRect = (sglRect) {
		.x = 0,
		.y = 0,
		.w = width,
		.h = height,
	};

	SGL_DEBUG_PRINT("sgl buffer initialized\n");
	return b;
}

void sglDestroyBuffer(sglBuffer* buffer)
{
	free(buffer->pixels);
	free(buffer->pf);
	free(buffer);
	SGL_DEBUG_PRINT("sgl buffer destroyed\n");
}

void sglClear(sglBuffer* buffer, int width, int height)
{
	memset(buffer->pixels, 0, width * height * sizeof(uint32_t));
}

void sglSetPixelRaw(sglBuffer* buffer, int x, int y, uint32_t color)
{
#ifdef SGL_CHECK_BUFFER_BOUNDS
	if (x < 0 || y < 0 ||
		x >= buffer->width || y >= buffer->height) return;
#endif

	switch (buffer->pf->bytesPerPixel) {
		case 1:
			*((uint8_t*)buffer->pixels + (y * buffer->width + x)) = color;
			break;

		case 2:
			*((uint16_t*)buffer->pixels + (y * buffer->width + x)) = color;
			break;

		case 4:
			*((uint32_t*)buffer->pixels + (y * buffer->width + x)) = color;
			break;
	}
}

uint32_t sglGetPixelRaw(sglBuffer* buffer, int x, int y)
{
#ifdef SGL_CHECK_BUFFER_BOUNDS
	if (x < 0 || y < 0 ||
		x >= buffer->width || y >= buffer->height) return 0;
#endif


	switch (buffer->pf->bytesPerPixel) {
		case 1:
			return *((uint8_t*)buffer->pixels + (y * buffer->width + x));
			break;

		case 2:
			return *((uint16_t*)buffer->pixels + (y * buffer->width + x));
			break;

		case 4:
			return *((uint32_t*)buffer->pixels + (y * buffer->width + x));
			break;

		default:
			return 0;
	}
}

void sglSetPixel(sglBuffer* buffer, int x, int y,
                 uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	sglSetPixelRaw(buffer, x, y, sglMapRGBA(r, g, b, a, buffer->pf));
// #ifdef SGL_CHECK_BUFFER_BOUNDS
// 	if (x < buffer->startX && y < buffer->startY &&
// 		x >= buffer->width && y >= buffer->height) return;
// #endif
//
// 	buffer->pixels[x + y * buffer->width] = sglMapRGBA(r, g, b, a, buffer->pf);
}

void sglGetPixel(sglBuffer* buffer, int x, int y,
                 uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
	
#ifdef SGL_CHECK_BUFFER_BOUNDS
	if (x < 0 || y < 0 ||
		x >= buffer->width || y >= buffer->height) return;
#endif
	// uint32_t color = buffer->pixels[x + y * buffer->width];
	uint32_t color = sglGetPixelRaw(buffer, x, y);

	if (r) { *r = (color & buffer->pf->rmask) >> buffer->pf->rshift; }
	if (g) { *g = (color & buffer->pf->gmask) >> buffer->pf->gshift; }
	if (b) { *b = (color & buffer->pf->bmask) >> buffer->pf->bshift; }
	if (a) { *a = (color & buffer->pf->amask) >> buffer->pf->ashift; }
}


uint32_t sglMapRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a,
					const sglPixelFormat* pf)
{
	return ((r << pf->rshift) & pf->rmask) +
           ((g << pf->gshift) & pf->gmask) +
           ((b << pf->bshift) & pf->bmask) +
           ((a << pf->ashift) & pf->amask);
}

void sglGetRGBA(uint32_t color, const sglPixelFormat* pf,
                uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
	if (r) { *r = (color & pf->rmask) >> pf->rshift; }
	if (g) { *g = (color & pf->gmask) >> pf->gshift; }
	if (b) { *b = (color & pf->bmask) >> pf->bshift; }
	if (a) { *a = (color & pf->amask) >> pf->ashift; }
}

uint32_t sglGetPixelType(sglPixelFormatEnum format)
{
	return (format >> 2) & 0x7;
}

uint32_t sglGetChannelOrder(sglPixelFormatEnum format)
{
	return (format >> 5) & 0x7;
}

uint32_t sglGetChannelLayout(sglPixelFormatEnum format)
{
	return format & 0x3;
}
