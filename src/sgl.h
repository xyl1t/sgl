/**
 * @file sgl.h
 * @brief Main header for sgl
 */

#ifndef SGL_H

// Check bounds by default in sglSetPixel()
#define SGL_CHECK_BUFFER_BOUNDS

#ifndef NDEBUG
#include <stdio.h>
#define SGL_DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define SGL_DEBUG_PRINT(...) do{ } while ( 0 )
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define sgl_member_size(type, member) sizeof(((type *)0)->member)

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
	SGL_PIXELFORMAT_ABGR32 =
		(SGL_CHANNELORDER_ABGR << 5) |
		(SGL_PIXELTYPE_32 << 2) |
		(SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_BGRA32 =
		(SGL_CHANNELORDER_BGRA << 5) |
		(SGL_PIXELTYPE_32 << 2) |
		(SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_RGBA32 =
		(SGL_CHANNELORDER_RGBA << 5) |
		(SGL_PIXELTYPE_32 << 2) |
		(SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_ARGB32 =
		(SGL_CHANNELORDER_ARGB << 5) |
		(SGL_PIXELTYPE_32 << 2) |
		(SGL_CHANNELLAYOUT_8888),

	SGL_PIXELFORMAT_ABGR4444 =
		(SGL_CHANNELORDER_ABGR << 5) |
		(SGL_PIXELTYPE_16 << 2) |
		(SGL_CHANNELLAYOUT_4444),

	SGL_PIXELFORMAT_RGB332 =
		(SGL_CHANNELORDER_RGBX << 5) |
		(SGL_PIXELTYPE_8 << 2) |
		(SGL_CHANNELLAYOUT_332),
	
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

bool sglHasIntersection(const sglRect* A, const sglRect* B);
bool sglIntersectRect(const sglRect* A, const sglRect* B, sglRect* result);


/**
 * @brief A buffer is a thing that sgl uses to draw things on
 */
typedef struct sglBuffer {
	void* pixels;
	sglPixelFormat* pf;
	int width;
	int height;
	sglRect clipRect;
} sglBuffer;

sglBuffer* sglCreateBuffer(uint32_t* pixels, uint32_t startX, uint32_t startY,
                           uint32_t width, uint32_t height,
                           sglPixelFormatEnum format);
sglBuffer* sglCreateNewBuffer(uint32_t width, uint32_t height,
                              sglPixelFormatEnum format);
void sglDestroyBuffer(sglBuffer* buffer);
bool sglSetClipRect(sglBuffer* buffer, const sglRect* rect);



/*****************************************************************************
 * GRAPHICS FUNCTIONS                                                        *
 *****************************************************************************/

// TODO: rename sgl***Pixel() to sgl***RGB() and sgl***RGBA()

/**
 * @brief clear buffer
 * @param buffer Buffer to clear
 * @param width Width of the buffer
 * @param height Height of the buffer
 */
void sglClear(sglBuffer* buffer, int width, int height);

/**
 * @brief set pixel in buffer using a 32 bit value
 * @param buffer Buffer to clear
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param color 32 bit color of pixel
 */
void sglSetPixelRaw(sglBuffer* buffer, int x, int y, uint32_t color);

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
void sglSetPixel(sglBuffer* buffer, int x, int y,
                 uint8_t r, uint8_t g, uint8_t b, uint8_t a);

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
void sglGetPixel(sglBuffer* buffer, int x, int y,
                 uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);



/*****************************************************************************
 * UTILITY FUNCTIONS                                                         *
 *****************************************************************************/

/**
 * @brief Map single rgba values to a 32 bit uint32_t using a pixel format
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 * @param pf Pixel format that should be used to map the rgba values
 */
uint32_t sglMapRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                    const sglPixelFormat* pf);
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
void sglGetRGBA(uint32_t color, const sglPixelFormat* pf,
                uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

uint32_t sglGetPixelType(sglPixelFormatEnum format);

uint32_t sglGetChannelOrder(sglPixelFormatEnum format);

uint32_t sglGetChannelLayout(sglPixelFormatEnum format);

#endif // !SGL_H

