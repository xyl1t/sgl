#include "sgl.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

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

int sgl_jpg_quality = 100;

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

void sglInvertRectWidth(sglRect* rect)
{
	int tmp = rect->x;
	rect->x = rect->x + rect->w;
	rect->w = tmp - rect->x;
}
void sglInvertRectHeight(sglRect* rect)
{
	int tmp = rect->y;
	rect->y = rect->y + rect->h;
	rect->h = tmp - rect->y;
}
void sglInvertRect(sglRect* rect)
{
	sglInvertRectWidth(rect);
	sglInvertRectHeight(rect);
}
bool sglFixRect(sglRect* rect) {
	if (!rect) return false;

	bool fixed = false;

	if (rect->w < 0) {
		int tmp = rect->x;
		rect->x = rect->x + rect->w;
		rect->w = tmp - rect->x;
		fixed = true;
	}

	if (rect->h < 0) {
		int tmp = rect->y;
		rect->y = rect->y + rect->h;
		rect->h = tmp - rect->y;
		fixed = true;
	}

	return fixed;
}

bool sglIsPointInRect(const sglRect* rect, int x, int y)
{
	if (!rect) return false;

	sglRect fixed = *rect;
	sglFixRect(&fixed);

	return x >= fixed.x && y >= fixed.y && x < fixed.x + fixed.w && y < fixed.y + fixed.h;
}

sglBuffer* sglCreateBuffer(
	void* pixels, uint32_t width, uint32_t height, sglPixelFormatEnum format)
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

void sglFreeBuffer(sglBuffer* buffer)
{
	if (!buffer) return;
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
		intersects = sglIntersectRect(&buffer->clipRect, rect, &buffer->clipRect);
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


// NOTE: maybe the clip rect should be set to the size of the buffer
sglBuffer* sglLoadBitmap(const char* path, sglPixelFormatEnum format)
{
	sglBuffer* bmp = malloc(sizeof(sglBuffer));

	int imgChannels;
	uint8_t* data = stbi_load(path, &bmp->width, &bmp->height, &imgChannels, 0);

	if (!data) {
		free(bmp);
		return NULL;
	}

	bmp->pf = sglCreatePixelFormat(format);
	// FIXME: mallocing pixel data but not freeing!!
	bmp->pixels = malloc(bmp->width * bmp->height * bmp->pf->bytesPerPixel);
	bmp->pitch = bmp->width * bmp->pf->bytesPerPixel;
	bmp->clipRect = (sglRect){
		.x = 0, .y = 0,
		.w = bmp->width, .h = bmp->height
	};

	for (int x = 0; x < bmp->width; x++) {
		for (int y = 0; y < bmp->height; y++) {
			uint8_t r = data[(x + y * bmp->width) * imgChannels + 0];
			uint8_t g = data[(x + y * bmp->width) * imgChannels + 1];
			uint8_t b = data[(x + y * bmp->width) * imgChannels + 2];
			uint8_t a = imgChannels == 4 ? data[(x + y * bmp->width) * imgChannels + 3] : 0xff;

			((uint32_t*)bmp->pixels)[x + y * bmp->width] = sglMapRGBA(r, g, b, a, bmp->pf);
		}
	}

	stbi_image_free(data);

	return bmp;
}

bool sglSaveBufferToFile(const sglBuffer* bmp, const char* filename, sglBitmapFormatEnum bitmapFormat)
{
	uint32_t* data = malloc(bmp->width * bmp->height * sizeof(uint32_t));
	sglBuffer* dataBuffer = sglCreateBuffer(data, bmp->width, bmp->height, SGL_PIXELFORMAT_RGBA32);
	sglDrawBuffer(dataBuffer, bmp, NULL, NULL);
	sglFreeBuffer(dataBuffer);

	switch (bitmapFormat) {
		case SGL_BITMAPFORMAT_PNG:
			return stbi_write_png(filename,
				bmp->width,
				bmp->height,
				4,
				data,
				bmp->pitch);
			break;
		case SGL_BITMAPFORMAT_BMP:
			return stbi_write_bmp(filename,
				bmp->width,
				bmp->height,
				4,
				data);
			break;
		case SGL_BITMAPFORMAT_JPG:
			return stbi_write_jpg(filename,
				bmp->width,
				bmp->height,
				4,
				data,
				sgl_jpg_quality);
			break;
		case SGL_BITMAPFORMAT_TGA:
			return stbi_write_tga(filename,
				bmp->width,
				bmp->height,
				4,
				data);
			break;
	}

	return false;
}

sglFont* sglCreateFont(const char* pathToFontBitmap, int fontWidth, int fontHeight,
	bool useKerning)
{
	sglBuffer* fontSheet = sglLoadBitmap(pathToFontBitmap, SGL_PIXELFORMAT_ABGR32);
	if (!fontSheet) return NULL;

	sglFont* font = malloc(sizeof(sglFont));

	font->fontSheet = fontSheet;
	font->fontWidth = fontWidth;
	font->fontHeight = fontHeight;

	if (useKerning) {
		int characterCols = fontSheet->width / fontWidth;
		int characterRows = fontSheet->height / fontHeight;

		for (int letterX = 0; letterX < characterCols; letterX++) {
			for (int letterY = 0; letterY < characterRows; letterY++) {

				bool wasLeft = false, wasRight = false;
				for (int subXLeft = 0, subXRight = fontWidth - 1;
						subXLeft < fontWidth;
						subXLeft++, subXRight--) {
					for (int subY = 0; subY < fontHeight; subY++) {

						// initialize
						int minLeftVal = fontWidth;
						int maxRightVal = 0;

						uint8_t isLetterHitLeft;
						sglGetPixel(fontSheet,
								&isLetterHitLeft, NULL, NULL, NULL,
								letterX * fontWidth + subXLeft,
								letterY * fontHeight + subY);

						uint8_t isLetterHitRight;
						sglGetPixel(fontSheet,
								&isLetterHitRight, NULL, NULL, NULL,
								letterX * fontWidth + subXRight,
								letterY * fontHeight + subY);

						// NOTE: also the drawing is not effecitve... put it in the demo
						// LEFT KERN
						if (isLetterHitLeft && !wasLeft) {
							wasLeft = true;
							sglGetKern(font, letterX, letterY, sglLeftKern) = subXLeft;
							sglDrawPixel(font->fontSheet,
									0xff, 0, 0, 0xff,
									letterX * fontWidth + subXLeft,
									letterY * fontHeight + subY);
						}

						// RIGHT KERN
						if (isLetterHitRight && !wasRight) {
							wasRight = true;
							sglGetKern(font, letterX, letterY, sglRightKern) = subXRight;
							sglDrawPixel(font->fontSheet,
									0, 0xff, 0, 0xff,
									letterX * fontWidth + subXRight,
									letterY * fontHeight + subY);
						}


					}
				}
			}
		}
	}

	return font;
}

void sglFreeFont(sglFont* font)
{
	if(!font) return;

	sglFreeBuffer((sglBuffer*)font->fontSheet);
	free(font);
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

#define GET_PIXEL_FAST(data, pitch, x, y, type, bpp) \
	*(type*)((uint8_t*)data + (y)*pitch + (x)*bpp)
	
#define GET_PIXEL_FAST_1(data, pitch, x, y) GET_PIXEL_FAST(data, pitch, x, y, uint8_t, 1)
#define GET_PIXEL_FAST_2(data, pitch, x, y) GET_PIXEL_FAST(data, pitch, x, y, uint16_t, 2)
#define GET_PIXEL_FAST_4(data, pitch, x, y) GET_PIXEL_FAST(data, pitch, x, y, uint32_t, 4)

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

	for (int y = buffer->clipRect.y;
		 y < buffer->clipRect.y + buffer->clipRect.h; y++) {
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

uint32_t sglGetPixelRaw(const sglBuffer* buffer, int x, int y)
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

void sglGetPixel(const sglBuffer* buffer, uint8_t* r, uint8_t* g, uint8_t* b,
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
	if (!sglClipLine(&buffer->clipRect, startX, startY, endX, endY, &startX,
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


void sglDrawCircle(
	sglBuffer* buffer, uint32_t color, int cntrX, int cntrY, int radius)
{
	if (!buffer)
		return;
	if (radius < 1)
		return;
	if (radius == 1) {
		sglDrawPixelRaw(buffer, color, cntrX, cntrY);
		return;
	}

	float x = radius;
	float y = 0;

	while ((int)x > (int)y) {
		x = sqrt(x * x - 2 * y - 1);

		sglDrawPixelRaw(buffer, color, (int)x + cntrX, (int)y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int)y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)x + cntrX, (int)-y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int)-y + cntrY);
		sglDrawPixelRaw(buffer, color, (int)y + cntrX, (int)x + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int)x + cntrY);
		sglDrawPixelRaw(buffer, color, (int)y + cntrX, (int)-x + cntrY);
		sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int)-x + cntrY);

		y++;
	}
}

void sglFillCircle(
	sglBuffer* buffer, uint32_t color, int cntrX, int cntrY, int radius)
{

	if (!buffer)
		return;
	if (radius < 1)
		return;
	if (radius == 1) {
		sglDrawPixelRaw(buffer, color, cntrX, cntrY);
		return;
	}

	float x = radius;
	float y = 0;

	while ((int)x > 0) {
		x = sqrt(x * x - 2 * y);

		int end = x + cntrX + 0.5f;
		for (int i = -x + cntrX + 1.5f; i < end; i++) {
			sglDrawPixelRaw(buffer, color, i, y + cntrY + 0.5f);
			sglDrawPixelRaw(buffer, color, i, -y + cntrY + 0.5f);
		}

		y++;
	}
}

/**
 * @brief check if point is in range of two slopes
 * @param x x coordinate of point
 * @param y y coordinate of point
 * @param k_s slope of start
 * @param k_e slope of end
 * @param isPositiveX_s if the start is on the positive side of the x axis
 * @param isPositiveX_e if the end is on the positive side of the x axis
 * @param isConcave is the angle greater than 180°
 */
static bool checkSlope(float x, float y, float k_s, float k_e,
	int isPositiveX_s, int isPositiveX_e, bool isConcave)
{
	float expected_y_s = k_s * x;
	float expected_y_e = k_e * x;

	bool s = (isPositiveX_s && y >= expected_y_s) ||
		(!isPositiveX_s && y <= expected_y_s);
	bool e = (isPositiveX_e && y <= expected_y_e) ||
		(!isPositiveX_e && y >= expected_y_e);

	if (isConcave)
		return s || e;
	else
		return s && e;
}

static int getQuadrant(float angle) { return angle / M_PI_2; }

// https://excalidraw.com/#json=kAaT2aqfqX3a18n5nrNBV,JXiyMP0jQeniTYq6XpTp_g

void sglDrawArc(sglBuffer* buffer, uint32_t color, int cntrX, int cntrY,
	int radius, float startAngle, float endAngle)
{
	if (!buffer) return;
	if (radius < 1) return;
	if (radius == 1) {
		sglDrawPixelRaw(buffer, color, cntrX, cntrY);
		return;
	}
	if (startAngle == 0 && endAngle == M_PI * 2)
		sglDrawCircle(buffer, color, cntrX, cntrY, radius);

	float k_s = tanf(startAngle);
	float k_e = tanf(endAngle);

	int isOnPositiveXSide_s = !(getQuadrant(startAngle) % 3);
	int isOnPositiveXSide_e = !(getQuadrant(endAngle) % 3);

	startAngle = sglNormalizeAngle(startAngle);
	endAngle = sglNormalizeAngle(endAngle);

	bool isConcave =
		startAngle < endAngle && fabsf(endAngle - startAngle) > M_PI ||
		startAngle > endAngle && fabsf(endAngle - startAngle) < M_PI;

	float x = radius;
	float y = 0;

	while ((int)x > (int)y) {
		x = sqrt(x * x - 2 * y - 1);

		if (checkSlope(x, y, k_s, k_e, isOnPositiveXSide_s, isOnPositiveXSide_e,
				isConcave))
			sglDrawPixelRaw(buffer, color, (int)x + cntrX, (int)y + cntrY);
		if (checkSlope(-x, y, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int)y + cntrY);
		if (checkSlope(x, -y, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)x + cntrX, (int)-y + cntrY);
		if (checkSlope(-x, -y, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)-x + cntrX, (int)-y + cntrY);
		if (checkSlope(y, x, k_s, k_e, isOnPositiveXSide_s, isOnPositiveXSide_e,
				isConcave))
			sglDrawPixelRaw(buffer, color, (int)y + cntrX, (int)x + cntrY);
		if (checkSlope(-y, x, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int)x + cntrY);
		if (checkSlope(y, -x, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)y + cntrX, (int)-x + cntrY);
		if (checkSlope(-y, -x, k_s, k_e, isOnPositiveXSide_s,
				isOnPositiveXSide_e, isConcave))
			sglDrawPixelRaw(buffer, color, (int)-y + cntrX, (int)-x + cntrY);

		y++;
	}
}

void sglFillArc(sglBuffer* buffer, uint32_t color, int cntrX, int cntrY,
	int radius, float startAngle, float endAngle)
{
	if (!buffer) return;
	if (radius < 1) return;
	if (radius == 1) {
		sglDrawPixelRaw(buffer, color, cntrX, cntrY);
		return;
	}
	if (startAngle == 0 && endAngle == M_PI * 2) {
		sglDrawCircle(buffer, color, cntrX, cntrY, radius);
		return;
	}

	float k_s = tanf(startAngle);
	float k_e = tanf(endAngle);

	int isOnPositiveXSide_s = !(getQuadrant(startAngle) % 3);
	int isOnPositiveXSide_e = !(getQuadrant(endAngle) % 3);

	startAngle = sglNormalizeAngle(startAngle);
	endAngle = sglNormalizeAngle(endAngle);

	bool isConcave =
		startAngle < endAngle && fabsf(endAngle - startAngle) > M_PI ||
		startAngle > endAngle && fabsf(endAngle - startAngle) < M_PI;

	float x = radius;
	float y = 0;

	while ((int)x > 0) {
		x = sqrt(x * x - 2 * y);

		int end = x + cntrX + 0.5f;
		for (int i = -x + cntrX + 1.5f; i < end; i++) {
			if (checkSlope(i - cntrX, y, k_s, k_e, isOnPositiveXSide_s,
					isOnPositiveXSide_e, isConcave)) {
				sglDrawPixelRaw(buffer, color, i, y + cntrY + 0.5f);
			}
			if (checkSlope(i - cntrX, -y, k_s, k_e, isOnPositiveXSide_s,
					isOnPositiveXSide_e, isConcave)) {
				sglDrawPixelRaw(buffer, color, i, -y + cntrY + 0.5f);
			}
		}

		y++;
	}
}

void sglDrawTriangle(sglBuffer* buffer, uint32_t color, int x1, int y1, int x2,
	int y2, int x3, int y3)
{
	sglDrawLine(buffer, color, x1, y1, x2, y2);
	sglDrawLine(buffer, color, x2, y2, x3, y3);
	sglDrawLine(buffer, color, x1, y1, x3, y3);
}

void sglFillTriangle(sglBuffer* buffer, uint32_t color, int x1, int y1, int x2,
	int y2, int x3, int y3)
{
	if (!buffer) return;

	// swap so that y1 is first, y2 second, y3 thrid
	int temp;
	if (y1 > y3) {
		temp = y3; y3 = y1; y1 = temp;
		temp = x3; x3 = x1; x1 = temp;
	}
	if (y2 > y3) {
		temp = y3; y3 = y2; y2 = temp;
		temp = x3; x3 = x2; x2 = temp;
	}
	if (y2 < y1) {
		temp = y1; y1 = y2; y2 = temp;
		temp = x1; x1 = x2; x2 = temp;
	}

	float t = (y2 - y1) / (float)(y3 - y1);
	float betwix = sglLerpf(x1, x3, t);

#define drawTrianglePart(sy, ey, sx, ex, ax, bx)      \
	do {                                              \
		for (int y = sy; y < ey; y++) {               \
			t = (y - sy) / (float)(ey - sy);          \
			int startX = sglLerpf(sx, ex, t) + 0.5f;  \
			int endX = sglLerpf(ax, bx, t) + 0.5f;    \
			if (endX < startX) {                      \
				temp = startX;                        \
				startX = endX;                        \
				endX = temp;                          \
			}                                         \
			for (int x = startX; x < endX; x++) {     \
				sglDrawPixelRaw(buffer, color, x, y); \
			}                                         \
		}                                             \
	} while (0);

	// top
	drawTrianglePart(y1, y2, x1, x2, x1, betwix);
	// bottom
	drawTrianglePart(y2, y3, x2, x3, betwix, x3);

#undef drawTrianglePart
}

void sglDrawColorInterpolatedTriangle(sglBuffer* buffer, int x1, int y1, int x2,
	int y2, int x3, int y3, uint32_t c1, uint32_t c2, uint32_t c3)
{
	if (!buffer) return;

	int temp;
	if (y1 > y3) {
		temp = y3; y3 = y1; y1 = temp;
		temp = x3; x3 = x1; x1 = temp;
		temp = c3; c3 = c1; c1 = temp;
	}
	if (y2 > y3) {
		temp = y3; y3 = y2; y2 = temp;
		temp = x3; x3 = x2; x2 = temp;
		temp = c3; c3 = c2; c2 = temp;
	}
	if (y2 < y1) {
		temp = y1; y1 = y2; y2 = temp;
		temp = x1; x1 = x2; x2 = temp;
		temp = c1; c1 = c2; c2 = temp;
	}

	uint8_t r1, g1, b1, a1;
	sglGetRGBA(c1, buffer->pf, &r1, &g1, &b1, &a1);
	uint8_t r2, g2, b2, a2;
	sglGetRGBA(c2, buffer->pf, &r2, &g2, &b2, &a2);
	uint8_t r3, g3, b3, a3;
	sglGetRGBA(c3, buffer->pf, &r3, &g3, &b3, &a3);

	float t = (y2 - y1) / (float)(y3 - y1);
	float betwix = sglLerpf(x1, x3, t);
	float betwir = sglLerpf(r1, r3, t);
	float betwig = sglLerpf(g1, g3, t);
	float betwib = sglLerpf(b1, b3, t);
	float betwia = sglLerpf(a1, a3, t);

	for (int y = y1; y < y2; y++) {
		t = (y - y1) / (float)(y2 - y1);
		int startX = sglLerpf(x1, x2, t) + 0.5f;
		int endX = sglLerpf(x1, betwix, t) + 0.5f;

		uint8_t s_r = sglLerpf(r1, r2, t) + 0.5f;
		uint8_t s_g = sglLerpf(g1, g2, t) + 0.5f;
		uint8_t s_b = sglLerpf(b1, b2, t) + 0.5f;
		uint8_t s_a = sglLerpf(a1, a2, t) + 0.5f;

		uint8_t e_r = sglLerpf(r1, betwir, t) + 0.5f;
		uint8_t e_g = sglLerpf(g1, betwig, t) + 0.5f;
		uint8_t e_b = sglLerpf(b1, betwib, t) + 0.5f;
		uint8_t e_a = sglLerpf(a1, betwia, t) + 0.5f;

		if (endX < startX) {
			temp = startX; startX = endX; endX = temp;
			temp = s_r; s_r = e_r; e_r = temp;
			temp = s_g; s_g = e_g; e_g = temp;
			temp = s_b; s_b = e_b; e_b = temp;
			temp = s_a; s_a = e_a; e_a = temp;
		}

		for (int x = startX; x < endX; x++) {
			t = (x - startX) / (float)(endX - startX);
			uint32_t color = sglMapRGBA(
					sglLerpf(s_r, e_r, t),
					sglLerpf(s_g, e_g, t),
					sglLerpf(s_b, e_b, t),
					sglLerpf(s_a, e_a, t),
					buffer->pf);
			sglDrawPixelRaw(buffer, color, x, y);
		}
	}
	
	for (int y = y2; y < y3; y++) {
		t = (y - y2) / (float)(y3 - y2);
		int startX = sglLerpf(x2, x3, t) + 0.5f;
		int endX = sglLerpf(betwix, x3, t) + 0.5f;

		uint8_t s_r = sglLerpf(r2, r3, t) + 0.5f;
		uint8_t s_g = sglLerpf(g2, g3, t) + 0.5f;
		uint8_t s_b = sglLerpf(b2, b3, t) + 0.5f;
		uint8_t s_a = sglLerpf(a2, a3, t) + 0.5f;

		uint8_t e_r = sglLerpf(betwir, r3, t) + 0.5f;
		uint8_t e_g = sglLerpf(betwig, g3, t) + 0.5f;
		uint8_t e_b = sglLerpf(betwib, b3, t) + 0.5f;
		uint8_t e_a = sglLerpf(betwia, a3, t) + 0.5f;

		if (endX < startX) {
			temp = startX; startX = endX; endX = temp;
			temp = s_r; s_r = e_r; e_r = temp;
			temp = s_g; s_g = e_g; e_g = temp;
			temp = s_b; s_b = e_b; e_b = temp;
			temp = s_a; s_a = e_a; e_a = temp;
		}

		for (int x = startX; x < endX; x++) {
			t = (x - startX) / (float)(endX - startX);
			uint32_t color = sglMapRGBA(
					sglLerpf(s_r, e_r, t),
					sglLerpf(s_g, e_g, t),
					sglLerpf(s_b, e_b, t),
					sglLerpf(s_a, e_a, t),
					buffer->pf);
			sglDrawPixelRaw(buffer, color, x, y);
		}
	}
}

void sglDrawBuffer(sglBuffer* buffer, const sglBuffer* bmp,
		const sglRect* srcRect, const sglRect* dstRect)
{
	sglRect bmpClipRect = { 0, 0, buffer->width, buffer->height };
	sglRect clippedSrcRect;
	sglRect clippedDstRect;
	sglRect bmpRect = { 0, 0, bmp->width, bmp->height };

	if (dstRect) {
		sglIntersectRect(&buffer->clipRect, dstRect, &clippedDstRect);
	} else {
		clippedDstRect = (sglRect){
			.x = buffer->clipRect.x,
				.y = buffer->clipRect.y,
				.w = buffer->clipRect.w,
				.h = buffer->clipRect.h
		};
	}

	if (srcRect) {
		// sglIntersectRect(&bmpClipRect, srcRect, &clippedSrcRect);
		clippedSrcRect = *srcRect;
	} else {
		clippedSrcRect = (sglRect){
			.x = 0,
			.y = 0,
			.w = bmp->width,
			.h = bmp->height
		};
	}

	if (clippedDstRect.w < 0) {
		sglInvertRectWidth(&clippedDstRect);
		sglInvertRectWidth(&clippedSrcRect);
	}
	if (clippedDstRect.h < 0) {
		sglInvertRectHeight(&clippedDstRect);
		sglInvertRectHeight(&clippedSrcRect);
	}

	for (int bufX = clippedDstRect.x; bufX < clippedDstRect.x + clippedDstRect.w; bufX++) {
		for (int bufY = clippedDstRect.y; bufY < clippedDstRect.y + clippedDstRect.h; bufY++) {
			int bmpX = (bufX - clippedDstRect.x) / (float)clippedDstRect.w * clippedSrcRect.w + clippedSrcRect.x;
			int bmpY = (bufY - clippedDstRect.y) / (float)clippedDstRect.h * clippedSrcRect.h + clippedSrcRect.y;

			if (!sglIsPointInRect(&bmpRect, bmpX, bmpY)) continue;

			uint8_t r, g, b, a;
			sglGetRGBA(sglGetPixelRaw(bmp, bmpX, bmpY), bmp->pf, &r, &g, &b, &a);

			sglDrawPixel(buffer, r, g, b, a, bufX, bufY);
		}
	}
}

void sglDrawText(sglBuffer* buffer, const char* text, int x, int y,
		const sglFont* font)
{
	if (!font) return;

	int charRows = font->fontSheet->width / font->fontWidth;
	int charCols = font->fontSheet->height / font->fontHeight;

	int cursorRow = 0;
	int cursorCol = 0;

	for (int charIdx = 0; text[charIdx] != '\0'; charIdx++) {
		char currentChar = text[charIdx];
		int letterBmpX = currentChar % charCols;
		int letterBmpY = currentChar / charRows;

		if (currentChar == '\n') {
			cursorCol = 0;
			cursorRow++;
			continue;
		}

		if (currentChar == '\t') {
			// NOTE: the 4 is the tab size
			cursorCol += 4 - (cursorCol) % 4;
			continue;
		}

		for (int fontPixelX = 0; fontPixelX < font->fontWidth; fontPixelX++) {
			for (int fontPixelY = 0; fontPixelY < font->fontHeight; fontPixelY++) {
				uint8_t r, g, b, a;
				sglGetPixel(font->fontSheet, &r, &g, &b, &a,
					letterBmpX * font->fontWidth + fontPixelX,
					letterBmpY * font->fontHeight + fontPixelY);

					if (r != 0) {
					sglDrawPixel(buffer, r, g, b, a,
						x + fontPixelX + cursorCol * font->fontWidth,
						y + fontPixelY + cursorRow * font->fontHeight);
				}
			}
		}

		cursorCol++;
	}
}



/*****************************************************************************
 *  UTILITY FUNCTIONS                                                        *
 *****************************************************************************/

float sglLerpf(float a, float b, float t) { return a + t * (b - a); }
double sglLerpd(double a, double b, double t) { return a + t * (b - a); }
int sglLerpi(int a, int b, int t) { return a + t * (b - a); }

float sglGetLength(float x, float y) {
	return sqrtf(x * x + y * y);
}

#define SGL_DISTANCE(a_x, a_y, b_x, b_y) \
	sqrtf(powf((b_x) - (a_x), 2) + powf((b_y) - (a_y), 2))

float sglGetDistancePoint(sglPoint a, sglPoint b)
{
	return SGL_DISTANCE(a.x, a.y, b.x, b.y);
}
float sglGetDistance(float a_x, float a_y, float b_x, float b_y)
{
	return SGL_DISTANCE(a_x, a_y, b_x, b_y);
}


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
bool sglClipLine(const sglRect* clipRect, int startX, int startY, int endX,
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

sglRect sglCalculateTextBoundingBox(const char* text, const sglFont* font) {
	int cursorRow = 0;
	int cursorCol = 0;
	int maxCursorCol = 0;

	for (int charIdx = 0; text[charIdx] != '\0'; charIdx++) {
		char currentChar = text[charIdx];

		if (currentChar == '\n') {
			cursorCol = 0;
			cursorRow++;
		} else if (currentChar == '\t') {
			// NOTE: the 4 is the tab size
			cursorCol += 4 - (cursorCol) % 4;
		} else {
			cursorCol++;
		}

		if (maxCursorCol < cursorCol) {
			maxCursorCol = cursorCol;
		}
	}

	return (sglRect){
		.x = 0,
		.y = 0,
		.w = maxCursorCol * font->fontWidth,
		.h = (cursorRow+1) * font->fontHeight
	};
}

int sglOffsetTextH(const char* text, sglTextAlignment alignment, const sglFont* font)
{
	if (!text) return 0;

	switch (alignment) {
		case SGL_TEXT_ALIGNMENT_LEFT:
			return 0;
			break;
		case SGL_TEXT_ALIGNMENT_CENTER:
			return -sglCalculateTextBoundingBox(text, font).w/2;
			break;
		case SGL_TEXT_ALIGNMENT_RIGHT:
			return -sglCalculateTextBoundingBox(text, font).w;
			break;
		default:
			return 0;
	}
}

int sglOffsetTextV(const char* text, sglTextAlignment alignment, const sglFont* font)
{
	if (!text || !font) return 0;

	switch (alignment) {
		case SGL_TEXT_ALIGNMENT_TOP:
			return 0;
			break;
		case SGL_TEXT_ALIGNMENT_CENTER:
			return -sglCalculateTextBoundingBox(text, font).h/2;
			break;
		case SGL_TEXT_ALIGNMENT_BOTTOM:
			return -sglCalculateTextBoundingBox(text, font).h;
			break;
		default:
			return 0;
	}
}

uint32_t sglMapRGBA(
	uint8_t r, uint8_t g, uint8_t b, uint8_t a, const sglPixelFormat* pf)
{
	return ((r << pf->rshift) & pf->rmask) + ((g << pf->gshift) & pf->gmask)
		+ ((b << pf->bshift) & pf->bmask) + ((a << pf->ashift) & pf->amask);
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

uint32_t sglGetChannelLayout(sglPixelFormatEnum format)
{
	return format & 0x3;
}

uint32_t sglHasAlphaChannel(sglPixelFormatEnum format)
{
	return sglGetChannelLayout(format) < 3;
}

const char* sglGetError(void) { return _sglError; }
