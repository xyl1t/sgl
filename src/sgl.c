#include "sgl.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int sgl_max(int a, int b) { return a > b ? a : b; }
static int sgl_min(int a, int b) { return a < b ? a : b; }

const char* _sglError;
// TODO: add sglUnsported()
static void sglError(const char* error) { _sglError = error; }

// #ifndef NDEBUG
// #define SGL_FAST_MATH
// #endif

#define SGL_FLOAT float

#ifdef SGL_FAST_MATH
#define roundf(x) (x);
#undef SGL_FLOAT
#define SGL_FLOAT int
#endif


/*****************************************************************************
 * DATA STRUCTURES                                                           *
 *****************************************************************************/

sglPixelFormat* sglCreatePixelFormat(sglPixelFormatEnum format)
{
	sglPixelFormat* pf = malloc(sizeof(sglPixelFormat));
	pf->format = format;

	pf->bitsPerPixel = sglGetPixelType(format) * 8;
	pf->bytesPerPixel = sglGetPixelType(format);

#define SGL_SET_FORMAT(a, b, c, d) \
	pf->a##mask = 0xff000000;      \
	pf->b##mask = 0x00ff0000;      \
	pf->c##mask = 0x0000ff00;      \
	pf->d##mask = 0x000000ff;      \
	pf->a##shift = 24;             \
	pf->b##shift = 16;             \
	pf->c##shift = 8;              \
	pf->d##shift = 0;

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
		pf->rmask = 0xE0;
		pf->gmask = 0x1C;
		pf->bmask = 0x03;
		pf->amask = 0x00;
		pf->rshift = 5;
		pf->gshift = 2;
		pf->bshift = 0;
		pf->ashift = 0;
		break;
	}

#undef SGL_SET_FORMAT

	return pf;
}

bool sglHasIntersection(const sglRect* A, const sglRect* B)
{
	return (A && B && A->x + A->w >= B->x && A->x <= B->x + B->w
		&& A->y + A->h >= B->y && A->y <= B->y + B->h);
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

sglBuffer* sglCreateBuffer(void* pixels, uint32_t width, uint32_t height,
	sglPixelFormatEnum format)
{
	sglBuffer* b = malloc(sizeof(sglBuffer));
	b->pixels = pixels;
	b->pf = sglCreatePixelFormat(format);
	b->width = width;
	b->height = height;
	b->pitch = width * b->pf->bytesPerPixel;
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
	// don't free the pixels for the caller!
	// free(buffer->pixels);
	free(buffer->pf);
	free(buffer);
	SGL_DEBUG_PRINT("sgl buffer destroyed\n");
}

bool sglSetClipRect(sglBuffer* buffer, const sglRect* rect)
{
	bool intersects = false;

	if (buffer) {
		intersects
			= sglIntersectRect(&buffer->clipRect, rect, &buffer->clipRect);
		if (!intersects) {
			sglError("`rect` is not intersecting the buffer\n");
		}
	}

	return intersects;
}
void sglResetClipRect(sglBuffer* buffer)
{
	if (!buffer)
		return;
	buffer->clipRect = (sglRect) {
		.x = 0,
		.y = 0,
		.w = buffer->width,
		.h = buffer->height,
	};
}


/*****************************************************************************
 * GRAPHICS FUNCTIONS                                                        *
 *****************************************************************************/

#define SET_PIXEL_FAST(x, y, type, bpp, color)                       \
	*(type*)((uint8_t*)buffer->pixels + (y)*buffer->pitch + (x)*bpp) \
		= (type)color

#define SET_PIXEL_FAST_1(x, y, color) SET_PIXEL_FAST(x, y, uint8_t, 1, color)
#define SET_PIXEL_FAST_2(x, y, color) SET_PIXEL_FAST(x, y, uint16_t, 2, color)
#define SET_PIXEL_FAST_4(x, y, color) SET_PIXEL_FAST(x, y, uint32_t, 4, color)

/**
 * fast draw pixel
 */
static void setPixel(sglBuffer* buffer, uint32_t color, int x, int y)
{
	switch (buffer->pf->bytesPerPixel) {
	case 1:
		*((uint8_t*)buffer->pixels + (y * buffer->width + x)) = color;
		break;

	case 2:
		*((uint16_t*)buffer->pixels + (y * buffer->width + x)) = color;
		break;

	case 3:
		sglError(
			"Unsupported pixel format (3 bytes per pixel are not supported)");
		break;

	case 4:
		*((uint32_t*)buffer->pixels + (y * buffer->width + x)) = color;
		break;
	}
}

void sglClear(sglBuffer* buffer)
{
	// memset(buffer->pixels, 0,
	// 		buffer->width * buffer->height * buffer->pf->bytesPerPixel);
	memset(buffer->pixels, 0, buffer->pitch * buffer->height);
}
void sglClearClipRect(sglBuffer* buffer)
{
	if (!buffer)
		return;

	for (int y = buffer->clipRect.y; y < buffer->clipRect.y + buffer->clipRect.h; y++) {
		memset(buffer->pixels + buffer->clipRect.x * buffer->pf->bytesPerPixel
				+ buffer->pitch * y,
			0, buffer->clipRect.w * buffer->pf->bytesPerPixel);
	}
}

void sglDrawPixelRaw(sglBuffer* buffer, uint32_t color, int x, int y)
{
	// #ifdef SGL_CHECK_BUFFER_BOUNDS
	// 	if (x < 0 || y < 0 ||
	// 		x >= buffer->width || y >= buffer->height) return;
	// #endif

	if (!buffer)
		return;

	if (x < buffer->clipRect.x || y < buffer->clipRect.y
		|| x >= buffer->clipRect.x + buffer->clipRect.w
		|| y >= buffer->clipRect.y + buffer->clipRect.h)
		return;

	setPixel(buffer, color, x, y);
}

void sglDrawPixel(
	sglBuffer* buffer, uint8_t r, uint8_t g, uint8_t b, uint8_t a, int x, int y)
{
	sglDrawPixelRaw(buffer, sglMapRGBA(r, g, b, a, buffer->pf), x, y);
}

uint32_t sglGetPixelRaw(sglBuffer* buffer, int x, int y)
{
#ifdef SGL_CHECK_BUFFER_BOUNDS
	if (x < 0 || y < 0 || x >= buffer->width || y >= buffer->height)
		return 0;
#endif

	switch (buffer->pf->bytesPerPixel) {
	case 1:
		return *((uint8_t*)buffer->pixels + (y * buffer->width + x));
		break;

	case 2:
		return *((uint16_t*)buffer->pixels + (y * buffer->width + x));
		break;

	case 3:
		sglError(
			"Unsupported pixel format (3 bytes per pixel are not supported)");
		break;

	case 4:
		return *((uint32_t*)buffer->pixels + (y * buffer->width + x));
		break;
	}

	return 0;
}

void sglGetPixel(sglBuffer* buffer, uint8_t* r, uint8_t* g, uint8_t* b,
	uint8_t* a, int x, int y)
{

#ifdef SGL_CHECK_BUFFER_BOUNDS
	if (x < 0 || y < 0 || x >= buffer->width || y >= buffer->height)
		return;
#endif
	// uint32_t color = buffer->pixels[x + y * buffer->width];
	uint32_t color = sglGetPixelRaw(buffer, x, y);

	if (r) {
		*r = (color & buffer->pf->rmask) >> buffer->pf->rshift;
	}
	if (g) {
		*g = (color & buffer->pf->gmask) >> buffer->pf->gshift;
	}
	if (b) {
		*b = (color & buffer->pf->bmask) >> buffer->pf->bshift;
	}
	if (a) {
		*a = (color & buffer->pf->amask) >> buffer->pf->ashift;
	}
}

void sglDrawLine(sglBuffer* buffer, uint32_t color, int startX, int startY,
	int endX, int endY)
{
	if (!clipLine(&buffer->clipRect, startX, startY, endX, endY, &startX,
			&startY, &endX, &endY)) {
		return;
	}

	int dx = endX - startX;
	int dy = endY - startY;

	int largest = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

	float stepX = dx / (float)largest;
	float stepY = dy / (float)largest;

	float x = startX + 0.5f;
	float y = startY + 0.5f;
	for (int i = 0; i <= largest; i++) {
		setPixel(buffer, color, x, y);
		x += stepX;
		y += stepY;
	}
}

void sglDrawRectangle(
	sglBuffer* buffer, uint32_t color, int x, int y, int w, int h)
{
	if (!buffer)
		return;

	sglDrawLine(buffer, color, x, y, x + w, y);
	sglDrawLine(buffer, color, x, y, x, y + h);
	sglDrawLine(buffer, color, x + w, y, x + w, y + h);
	sglDrawLine(buffer, color, x, y + h, x + w, y + h);
}

void sglFillRectangle(
	sglBuffer* buffer, uint32_t color, int startX, int startY, int w, int h)
{
	if (!buffer)
		return;

	sglRect clipped = (sglRect) { .x = startX, .y = startY, .w = w, .h = h };

	// if rect is not in clipping pane of the buffer just exit
	if (!sglIntersectRect(&clipped, &buffer->clipRect, &clipped)) {
		return;
	}

#define FILL_RECT(type, bpp)                                          \
	do {                                                              \
		for (int x = clipped.x; x < clipped.x + clipped.w; x++) {     \
			for (int y = clipped.y; y < clipped.y + clipped.h; y++) { \
				SET_PIXEL_FAST(x, y, type, bpp, color);               \
			}                                                         \
		}                                                             \
	} while (0);


	switch (buffer->pf->bytesPerPixel) {
	case 1:
		FILL_RECT(uint8_t, 1);
		break;
	case 2:
		FILL_RECT(uint16_t, 2);
		break;
	case 3:
		sglError(
			"Unsupported pixel format (3 bytes per pixel are not supported)");
		break;
	case 4:
		FILL_RECT(uint32_t, 4);
		break;
	}
}

void sglDrawCircle(sglBuffer* buffer, uint32_t color,
	int cntrX, int cntrY, int radius)
{
	if (!buffer) return;
	if (radius < 1) return;
	if (radius == 1) { sglDrawPixelRaw(buffer, color, cntrX, cntrY); return; }

	float x = radius;
	float y = 0;

	while((int)x > (int)y) {
		x = sqrt(x * x - 2 * y - 1);

		sglDrawPixelRaw(buffer, color, (int) x + cntrX, (int) y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int) y + cntrY);
		sglDrawPixelRaw(buffer, color, (int) x + cntrX, (int)-y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int)-y + cntrY);
		sglDrawPixelRaw(buffer, color, (int) y + cntrX, (int) x + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int) x + cntrY);
		sglDrawPixelRaw(buffer, color, (int) y + cntrX, (int)-x + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int)-x + cntrY);

		y++;
	}
}

void sglFillCircle(sglBuffer* buffer, uint32_t color,
		int cntrX, int cntrY, int radius)
{

	if (!buffer) return;
	if (radius < 1) return;
	if (radius == 1) { sglDrawPixelRaw(buffer, color, cntrX, cntrY); return; }

	float x = radius;
	float y = 0;

	while((int) x > 0) {
		x = sqrt(x * x - 2 * y);

		int end = x + cntrX + 0.5f;
		for(int i = -x + cntrX + 0.5f; i < end; i++) {
			sglDrawPixelRaw(buffer, color, i,  y + cntrY + 0.5f);
			sglDrawPixelRaw(buffer, color, i, -y + cntrY + 0.5f);
		}

		y++;
	}
}

/*****************************************************************************
 * UTILITY FUNCTIONS                                                         *
 *****************************************************************************/

float sglLerpf(float a, float b, float t) { return a + t * (b - a); }
double sglLerpd(double a, double b, double t) { return a + t * (b - a); }
int sglLerpi(int a, int b, int t) { return a + t * (b - a); }


static int findRegion(const sglRect* r, int x, int y)
{
	if (!r)
		return -1;

	int code = 0;

	if (y >= r->y + r->h)
		code |= 1; // bottom
	else if (y < r->y)
		code |= 2; // top
	if (x >= r->x + r->w)
		code |= 4; // right
	else if (x < r->x)
		code |= 8; // left

	return (code);
}
bool clipLine(const sglRect* clipRect, int startX, int startY, int endX,
	int endY, int* cstartX, int* cstartY, int* cendX, int* cendY)
{
	int code1, code2, codeout;
	bool accept = 0, done = 0;
	code1 = findRegion(clipRect, startX, startY);
	code2 = findRegion(clipRect, endX, endY);

	do {
		if (!(code1 | code2))
			accept = done = 1;
		else if (code1 & code2)
			done = 1;

		else {
			SGL_FLOAT x, y;
#ifdef SGL_FAST_MATH
			SGL_DEBUG_PRINT("test\n");
#endif
			codeout = code1 ? code1 : code2;
			if (codeout & 1) { // bottom
				x = startX
					+ (endX - startX) * (clipRect->y + clipRect->h - 1 - startY)
						/ (float)(endY - startY);
				y = clipRect->y + clipRect->h - 1;
			} else if (codeout & 2) { // top
				x = startX
					+ (endX - startX) * (clipRect->y - startY)
						/ (float)(endY - startY);
				y = clipRect->y;
			} else if (codeout & 4) { // right
				y = startY
					+ (endY - startY) * (clipRect->x + clipRect->w - 1 - startX)
						/ (float)(endX - startX);
				x = clipRect->x + clipRect->w - 1;
			} else { // left
				y = startY
					+ (endY - startY) * (clipRect->x - startX)
						/ (float)(endX - startX);
				x = clipRect->x;
			}

			if (codeout == code1) {
				startX = roundf(x);
				startY = roundf(y);
				code1 = findRegion(clipRect, startX, startY);
			} else {
				endX = roundf(x);
				endY = roundf(y);
				code2 = findRegion(clipRect, endX, endY);
			}
		}
	} while (done == 0);

	if (accept) {
		*cstartX = startX;
		*cendX = endX;
		*cstartY = startY;
		*cendY = endY;
		return 1;
	} else {
		*cstartX = *cstartY = *cendX = *cendY = 0;
		return 0;
	}
}

uint32_t sglMapRGBA(
	uint8_t r, uint8_t g, uint8_t b, uint8_t a, const sglPixelFormat* pf)
{
	return ((r << pf->rshift) & pf->rmask)
		+ ((g << pf->gshift) & pf->gmask)
		+ ((b << pf->bshift) & pf->bmask)
		+ ((a << pf->ashift) & pf->amask);
}

void sglGetRGBA(uint32_t color, const sglPixelFormat* pf, uint8_t* r,
	uint8_t* g, uint8_t* b, uint8_t* a)
{
	if (r) {
		*r = (color & pf->rmask) >> pf->rshift;
	}
	if (g) {
		*g = (color & pf->gmask) >> pf->gshift;
	}
	if (b) {
		*b = (color & pf->bmask) >> pf->bshift;
	}
	if (a) {
		*a = (color & pf->amask) >> pf->ashift;
	}
}

uint32_t sglGetPixelType(sglPixelFormatEnum format)
{
	return (format >> 2) & 0x7;
}

uint32_t sglGetChannelOrder(sglPixelFormatEnum format)
{
	return (format >> 5) & 0x7;
}

uint32_t sglGetChannelLayout(sglPixelFormatEnum format) { return format & 0x3; }

const char* sglGetError(void) { return _sglError; }
