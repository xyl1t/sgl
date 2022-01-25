#ifndef SGL_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define sgl_member_size(type, member) sizeof(((type *)0)->member)

/* data structures */

/**
 * \brief A buffer is a thing that sgl uses to draw
 */
typedef struct sglBuffer {
	uint32_t* data;
	size_t width;
	size_t height;
} sglBuffer;

sglBuffer* sglCreateBuffer(int width, int height);
void sglDestroyBuffer(sglBuffer* buffer);


/* graphics functions */

/**
 * \brief clear buffer
 * @param buffer Buffer to clear
 * @param width Width of the buffer
 * @param height Height of the buffer
 */
void sglClear(uint32_t* buffer, int width, int height);

/**
 * \brief set pixel in buffer
 * @param buffer Buffer to clear
 * @param bufferWidth width of buffer
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param color 32 bit color of pixel
 */
void sglSetPixel(uint32_t* buffer, int x, int y, size_t bufferWidth, uint32_t color);


/* utility functions */

#endif // !SGL_H
