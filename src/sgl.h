#ifndef SGL_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define sgl_member_size(type, member) sizeof(((type *)0)->member)

/* data structures */

// TODO: add more formats
typedef enum sglPixelFormatEnum {
	SGL_PIXELFORMAT_ABGR8888,
	SGL_PIXELFORMAT_BGRA8888,
	SGL_PIXELFORMAT_RGBA8888,
	SGL_PIXELFORMAT_ARGB8888,
} sglPixelFormatEnum;

typedef struct sglPixelFormat {
	int rmask;
	int gmask;
	int bmask;
	int amask;
	int rshift;
	int gshift;
	int bshift;
	int ashift;
	uint32_t format; // typeof sglPixelFormat
} sglPixelFormat;

sglPixelFormat* sglCreatePixelFormat(sglPixelFormatEnum pf);

/**
 * \brief A buffer is a thing that sgl uses to draw
 */
typedef struct sglBuffer {
	uint32_t* pixels;
	uint32_t width;
	uint32_t height;
	sglPixelFormat* format;
} sglBuffer;

sglBuffer* sglCreateBuffer(uint32_t* pixels, int width, int height, sglPixelFormatEnum format);
sglBuffer* sglCreateNewBuffer(int width, int height, sglPixelFormatEnum format);
void sglDestroyBuffer(sglBuffer* buffer);


/* graphics functions */

/**
 * \brief clear buffer
 * @param buffer Buffer to clear
 * @param width Width of the buffer
 * @param height Height of the buffer
 */
void sglClear(sglBuffer* buffer, int width, int height);

/**
 * \brief set pixel in buffer
 * @param buffer Buffer to clear
 * @param bufferWidth width of buffer
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param color 32 bit color of pixel
 */
void sglSetPixel(sglBuffer* buffer, int x, int y, uint32_t color);


/* utility functions */

uint32_t sglToColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a, sglPixelFormat* pf);

#endif // !SGL_H
