/**
 * @file sgl.h
 * @brief Main header for sgl
 */

#ifdef SGL_FAST_MATH
#define SGL_FAST_MATH
#endif

#ifndef SGL_H

// Check bounds by default in sglSetPixel()
#define SGL_CHECK_BUFFER_BOUNDS

#ifndef NDEBUG
#include <stdio.h>
#define SGL_DEBUG_PRINT(...)          \
	do {                              \
		fprintf(stderr, __VA_ARGS__); \
	} while (0)
#else
#define SGL_DEBUG_PRINT(...) \
	do {                     \
	} while (0)
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define sgl_member_size(type, member) sizeof(((type*)0)->member)

/*****************************************************************************
 * STATE VARIABLES                                                           *
 *****************************************************************************/

/*****************************************************************************
 * DATA STRUCTURES                                                           *
 *****************************************************************************/

// TODO: add more formats

typedef enum {
	SGL_PIXELTYPE_8 = 1,
	SGL_PIXELTYPE_16 = 2,
	SGL_PIXELTYPE_32 = 4,
} SGL_PixelType;

typedef enum {
	SGL_CHANNELORDER_ABGR,
	SGL_CHANNELORDER_BGRA,
	SGL_CHANNELORDER_RGBA,
	SGL_CHANNELORDER_ARGB,

	SGL_CHANNELORDER_XBGR,
	SGL_CHANNELORDER_BGRX,
	SGL_CHANNELORDER_RGBX,
	SGL_CHANNELORDER_XRGB
} SGL_ChannelOrder;

typedef enum {
	SGL_CHANNELLAYOUT_8888,
	SGL_CHANNELLAYOUT_4444,
	SGL_CHANNELLAYOUT_565,
	SGL_CHANNELLAYOUT_332,
} SGL_ChannelLayout;


/**
 * @brief Pixel formats
 */
typedef enum {
	SGL_PIXELFORMAT_ABGR32 = (SGL_CHANNELORDER_ABGR << 5)
		| (SGL_PIXELTYPE_32 << 2) | (SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_BGRA32 = (SGL_CHANNELORDER_BGRA << 5)
		| (SGL_PIXELTYPE_32 << 2) | (SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_RGBA32 = (SGL_CHANNELORDER_RGBA << 5)
		| (SGL_PIXELTYPE_32 << 2) | (SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_ARGB32 = (SGL_CHANNELORDER_ARGB << 5)
		| (SGL_PIXELTYPE_32 << 2) | (SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_ABGR4444 = (SGL_CHANNELORDER_ABGR << 5)
		| (SGL_PIXELTYPE_16 << 2) | (SGL_CHANNELLAYOUT_4444),

	SGL_PIXELFORMAT_RGB332 = (SGL_CHANNELORDER_RGBX << 5)
		| (SGL_PIXELTYPE_8 << 2) | (SGL_CHANNELLAYOUT_332),

	// SGL_PIXELFORMAT_BGRX8888, // X, r, g, b
	// SGL_PIXELFORMAT_RGBX8888, // X, b, g, r
} sglPixelFormatEnum;

/**
 * @brief Struct containing information about the format of pixel
 */
typedef struct sglPixelFormat {
	uint32_t format; /**< Pixel format with type of sglPixelFormat */
	uint8_t bitsPerPixel;
	uint8_t bytesPerPixel;
	uint32_t rmask;
	uint32_t gmask;
	uint32_t bmask;
	uint32_t amask;
	uint8_t rshift;
	uint8_t gshift;
	uint8_t bshift;
	uint8_t ashift;
} sglPixelFormat;

sglPixelFormat* sglCreatePixelFormat(sglPixelFormatEnum pf);

/**
 * @brief A simple rectangle struct
 */
typedef struct sglRect {
	int x;
	int y;
	int w;
	int h;
} sglRect;

/**
 * @brief Checks if two rectangles intersect
 * @parameter A First rectangle
 * @parameter B second rectangle
 * @return True if rectangle A and B intersect
 */
bool sglHasIntersection(const sglRect* A, const sglRect* B);
/**
 * @brief Checks if two rectangles intersect
 * @parameter A First rectangle
 * @parameter B second rectangle
 * @parameter result Intersecting rectangle of rectangles A and B
 * @return True if rectangle A and B intersect
 */
bool sglIntersectRect(const sglRect* A, const sglRect* B, sglRect* result);


/**
 * @brief A buffer is a thing that sgl uses to draw things on
 */
typedef struct sglBuffer {
	void* pixels;
	sglPixelFormat* pf;
	int width;
	int height;
	int pitch;
	sglRect clipRect;
} sglBuffer;

/**
 * @brief Creates an sgl buffer based on a pixel buffer
 * @parameter pixels The pixel buffer that will be used to draw on
 * @parameter width Width of the buffer
 * @parameter height Height of the buffer
 * @parameter format Pixel format of the pixel buffer
 */
sglBuffer* sglCreateBuffer(void* pixels, uint32_t width, uint32_t height,
	sglPixelFormatEnum format);
/**
 * @brief Destroys an sgl buffer
 * @parameter buffer The buffer to delete
 */
void sglDestroyBuffer(sglBuffer* buffer);

/**
 * @brief Sets the clipping rectangle of the buffer, meaning that you won't
 * be able to draw outside the clipping rectangle
 *
 * The clipping rectangle will be a result of the intersection of the buffer
 * size and the rect parameter.
 *
 * @parameter buffer The buffer
 * @parameter rect The clipping rectangle that should be used by the buffer
 */
bool sglSetClipRect(sglBuffer* buffer, const sglRect* rect);
/**
 * @brief Resets the clipping rectangle of the buffer to the full buffer size
 * @parameter buffer The buffer
 */
void sglResetClipRect(sglBuffer* buffer);



/*****************************************************************************
 * GRAPHICS FUNCTIONS                                                        *
 *****************************************************************************/

// TODO: rename sgl***Pixel() to sgl***RGB() and sgl***RGBA()

/**
 * @brief clear the whole buffer
 * @param buffer Buffer to clear
 * @param width Width of the buffer
 * @param height Height of the buffer
 */
void sglClear(sglBuffer* buffer);

/**
 * @brief clear the clipping rectangle part of the buffer
 * @param buffer Buffer to clear
 * @param width Width of the buffer
 * @param height Height of the buffer
 */
void sglClearClipRect(sglBuffer* buffer);


/**
 * @brief set pixel in buffer using a 32 bit value
 * @param buffer Buffer to clear
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param color 32 bit color of pixel
 */
void sglDrawPixelRaw(sglBuffer* buffer, uint32_t color, int x, int y);

/**
 * @brief set pixel in buffer
 * @param buffer Buffer to clear
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void sglDrawPixel(sglBuffer* buffer, uint8_t r, uint8_t g, uint8_t b, uint8_t a,
	int x, int y);

/**
 * @brief get pixel in buffer
 * @param buffer Buffer to clear
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 */
uint32_t sglGetPixelRaw(sglBuffer* buffer, int x, int y);

/**
 * @brief get pixel in buffer
 * @param buffer Buffer to clear
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param[out] r Pointer to red component
 * @param[out] g Pointer to green component
 * @param[out] b Pointer to blue component
 * @param[out] a Pointer to alpha component
 */
void sglGetPixel(sglBuffer* buffer, uint8_t* r, uint8_t* g, uint8_t* b,
	uint8_t* a, int x, int y);

/**
 * @brief Draw a line on buffer
 * @parameter buffer Buffer to draw on
 * @parameter color Color of the line
 * @parameter startX x coordinate of the start point
 * @parameter startY y coordinate of the start point
 * @parameter endX x coordinate of the end point
 * @parameter endY y coordinate of the end point
 */
void sglDrawLine(sglBuffer* buffer, uint32_t color, int startX, int startY,
	int endX, int endY);

/**
 * @brief Draw a filled rectangle on the buffer
 * @parameter buffer Buffer to draw on
 * @parameter color Color of the rectangle
 * @parameter startX x coordinate of the rectangle
 * @parameter startY y coordinate of the rectangle
 * @parameter w Width of the rectangle
 * @parameter h Height of the rectangle
 */
void sglFillRectangle(
	sglBuffer* buffer, uint32_t color, int startX, int startY, int w, int h);

/**
 * @brief Draw a rectangle on the buffer
 * @parameter buffer Buffer to draw on
 * @parameter color Color of the rectangle
 * @parameter startX x coordinate of the rectangle
 * @parameter startY y coordinate of the rectangle
 * @parameter w Width of the rectangle
 * @parameter h Height of the rectangle
 */
void sglDrawRectangle(
	sglBuffer* buffer, uint32_t color, int x, int y, int w, int h);


/*****************************************************************************
 * UTILITY FUNCTIONS                                                         *
 *****************************************************************************/

/**
 * @brief Linear interpolation using float
 * @parameter a First value
 * @parameter b Second value
 * @parameter t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
float sglLerpf(float a, float b, float t);
/**
 * @brief Linear interpolation using double
 * @parameter a First value
 * @parameter b Second value
 * @parameter t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
double sglLerpd(double a, double b, double t);
/**
 * @brief Linear interpolation using int
 * @parameter a First value
 * @parameter b Second value
 * @parameter t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
int sglLerpi(int a, int b, int t);


bool clipLine(const sglRect* clipRect, int startX, int startY, int endX,
	int endY, int* cstartX, int* cstartY, int* cendX, int* cendY);

/**
 * @brief Map single rgba values to a 32 bit uint32_t using a pixel format
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 * @param pf Pixel format that should be used to map the rgba values
 */
uint32_t sglMapRGBA(
	uint8_t r, uint8_t g, uint8_t b, uint8_t a, const sglPixelFormat* pf);
/**
 * @brief Get separate components of a 32 bit color using a pixel format
 * Pass NULL for components that you don't want to get.
 * @param color The 32 bit color that is to be converted
 * @param pf Pixel format that should be used to convert the color
 * @param[out] r Pointer to red component
 * @param[out] g Pointer to green component
 * @param[out] b Pointer to blue component
 * @param[out] a Pointer to alpha component
 */
void sglGetRGBA(uint32_t color, const sglPixelFormat* pf, uint8_t* r,
	uint8_t* g, uint8_t* b, uint8_t* a);

/**
 * @brief Get pixel type from the an sglPixelFormatEnum
 */
uint32_t sglGetPixelType(sglPixelFormatEnum format);

/**
 * @brief Get the channel order from the an sglPixelFormatEnum
 */
uint32_t sglGetChannelOrder(sglPixelFormatEnum format);

/**
 * @brief Get the channel layout from the an sglPixelFormatEnum
 */
uint32_t sglGetChannelLayout(sglPixelFormatEnum format);

/**
 * @brief Get last error that occurred when calling an sgl function
 */
const char* sglGetError(void);

#endif // !SGL_H
