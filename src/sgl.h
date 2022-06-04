/**
 * @file sgl.h
 * @brief Main header for sgl
 */

#ifndef SGL_H
#define SGL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

#ifdef SGL_FAST_MATH
#define SGL_FAST_MATH
#endif

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

#define sgl_member_size(type, member) sizeof(((type*)0)->member)

#define sglGetKern(font, _char_x, _char_y, side) \
	font->kern[_char_x * 2 + _char_y * font->cols*2 + side]
#define sglLeftKern 0
#define sglRightKern 1
// TODO: extract this to font level
#define sglTextSpacing 1

#define sglExpandColor32(color, pf) \
	(color & pf->rmask) >> pf->rshift, (color & pf->gmask) >> pf->gshift, (color & pf->bmask) >> pf->bshift, (color & pf->amask) >> pf->ashift

// #define _sglAlphaBlendColor(a, b, pf) buffer->alphaBlendingEnabled ? 

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
 * @param A First rectangle
 * @param B second rectangle
 * @return True if rectangle A and B intersect
 */
bool sglHasIntersection(const sglRect* A, const sglRect* B);
/**
 * @brief Checks if two rectangles intersect
 * @param A First rectangle
 * @param B second rectangle
 * @param result Intersecting rectangle of rectangles A and B
 * @return True if rectangle A and B intersect
 */
bool sglIntersectRect(const sglRect* A, const sglRect* B, sglRect* result);
/**
 * @brief Swap start and end x cooridnate by inverting the width
 */
void sglInvertRectWidth(sglRect* rect);
/**
 * @brief Swap start and end y cooridnate by inverting the height
 */
void sglInvertRectHeight(sglRect* rect);
/**
 * @brief Swap start and end coordinates by inverting width and height
 */
void sglInvertRect(sglRect* rect);
/**
 * @brief Fixes negative width and height, in the end the rectnagle's
 * coordinates stay the same
 * @param rect The rectangle to fix
 * @return True if rectangle got fixed
 */
bool sglFixRect(sglRect* rect);
/**
 * @brief Test if a point is inside a rectangle
 * @param rect The rectangle to test the point against
 * @param x The x cooridnate of the point
 * @param y The y cooridnate of the point
 * @return True if the point is inside the rectangle
 */
bool sglIsPointInRect(const sglRect* rect, int x, int y);


/**
 * @brief A simple rectangle struct
 */
typedef struct sglPoint {
	int x;
	int y;
} sglPoint;

/**
 * @brief A simple rectangle struct
 */
typedef struct sglFPoint {
	float x;
	float y;
} sglFPoint;




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
	bool alphaBlendingEnabled;
} sglBuffer;

/**
 * @brief Creates an sgl buffer based on a pixel buffer, call sglFreeBuffer() to free!
 * @param pixels The pixel buffer that will be used to draw on
 * @param width Width of the buffer
 * @param height Height of the buffer
 * @param format Pixel format of the pixel buffer
 */
sglBuffer* sglCreateBuffer(void* pixels, uint32_t width, uint32_t height,
	sglPixelFormatEnum format);
/**
 * @brief Frees an sgl buffer, call this only with sglCreateBuffer()!
 * @param buffer The buffer to free
 * This freeing does NOT free the pixels of the buffer
 */
void sglFreeBuffer(sglBuffer* buffer);

void sglEnableAlphaBlending(sglBuffer* buffer);
void sglDisableAlphaBlending(sglBuffer* buffer);

/**
 * @brief Sets the clipping rectangle of the buffer, meaning that you won't
 * be able to draw outside the clipping rectangle
 *
 * The clipping rectangle will be a result of the intersection of the buffer
 * size and the rect parameter.
 *
 * @param buffer The buffer
 * @param rect The clipping rectangle that should be used by the buffer
 */
bool sglSetClipRect(sglBuffer* buffer, const sglRect* rect);
/**
 * @brief Resets the clipping rectangle of the buffer to the full buffer size
 * @param buffer The buffer
 */
void sglResetClipRect(sglBuffer* buffer);

/**
 * @brief Enum of image formats
 */
typedef enum {
	SGL_BITMAPFORMAT_PNG,
	SGL_BITMAPFORMAT_BMP,
	SGL_BITMAPFORMAT_JPG,
	SGL_BITMAPFORMAT_TGA,
} sglBitmapFormatEnum;

/**
 * @brief Load a bitmap from an image file, call sglFreeBitmap() to free!
 * @param path Path to the image file
 * @param format The pixel format of the bitmap
 * @return Bitmap struct in the pixel format that was given or NULL if the
 * file is not found
 */
sglBuffer* sglLoadBitmap(const char* path, sglPixelFormatEnum format);

/**
 * @brief Frees an sgl bitmap buffer, call this only with sglLoadBuffer()!
 * @param buffer The buffer to free
 * This freeing also frees the pixels of the buffer
 */
void sglFreeBitmap(sglBuffer* buffer);

/**
 * @brief Save a bitmap as an image file
 * @param buffer The buffer to save
 * @param filename The name and path to the image file
 * @param bitmapFormat The image format to save the file in (png, bmp, jpg, tga)
 * @return True if successful
 */
bool sglSaveBufferToFile(const sglBuffer* buffer, const char* filename,
	sglBitmapFormatEnum bitmapFormat);
extern int sgl_jpg_quality;

typedef struct sglFont {
	/* const */ sglBuffer* fontSheet;
	int kern[512]; // TODO: you don't need this many, maybe only 16*2*16
	int fontWidth;
	int fontHeight;
	int8_t cols;
	int8_t rows;
} sglFont;

sglFont* sglCreateFont(const char* pathToFontBitmap, int fontWidth, int fontHeight,
	bool useKerning);
void sglFreeFont(sglFont* font);
// TODO:
// getLetter()?

/*****************************************************************************
 * DRAWING FUNCTIONS                                                         *
 *****************************************************************************/

// General structure of drawing functions:
// drawSomething(bufferToDrawOn, contentToDraw, whereOrHowToDraw)

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
uint32_t sglGetPixelRaw(const sglBuffer* buffer, int x, int y);

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
void sglGetPixel(const sglBuffer* buffer, uint8_t* r, uint8_t* g, uint8_t* b,
	uint8_t* a, int x, int y);

/**
 * @brief Draw a line on buffer
 * @param buffer Buffer to draw on
 * @param color Color of the line
 * @param startX x coordinate of the start point
 * @param startY y coordinate of the start point
 * @param endX x coordinate of the end point
 * @param endY y coordinate of the end point
 */
void sglDrawLine(sglBuffer* buffer, uint32_t color, int startX, int startY,
	int endX, int endY);

/**
 * @brief Draw a rectangle on the buffer
 * @param buffer Buffer to draw on
 * @param color Stroke color of the rectangle
 * @param startX x coordinate of the rectangle
 * @param startY y coordinate of the rectangle
 * @param w Width of the rectangle
 * @param h Height of the rectangle
 */
void sglDrawRectangle(
	sglBuffer* buffer, uint32_t color, int x, int y, int w, int h);

/**
 * @brief Draw a filled rectangle on the buffer
 * @param buffer Buffer to draw on
 * @param color Fill color of the rectangle
 * @param startX x coordinate of the rectangle
 * @param startY y coordinate of the rectangle
 * @param w Width of the rectangle
 * @param h Height of the rectangle
 */
void sglFillRectangle(
	sglBuffer* buffer, uint32_t color, int startX, int startY, int w, int h);

/**
 * @brief Draw a cricle on the buffer
 * @param buffer Buffer to draw on
 * @param color Stroke color of the circle
 * @param ctrX x coordinate of the center of the cirlce
 * @param ctrY y coordinate of the center of the cirlce
 * @param radius radius of the circle
 */
void sglDrawCircle(sglBuffer* buffer, uint32_t color,
	int cntrX, int cntrY, int radius);

/**
 * @brief Draw a filled cricle on the buffer
 * @param buffer Buffer to draw on
 * @param color Fill color of the circle
 * @param ctrX x coordinate of the center of the cirlce
 * @param ctrY y coordinate of the center of the cirlce
 * @param radius radius of the circle
 */
void sglFillCircle(
	sglBuffer* buffer, uint32_t color, int cntrX, int cntrY, int radius);

/**
 * @brief Draw a filled cricle on the buffer
 * @param buffer Buffer to draw on
 * @param color Fill color of the circle
 * @param ctrX x coordinate of the center of the cirlce
 * @param ctrY y coordinate of the center of the cirlce
 * @param radius radius of the circle
 */
void sglDrawArc(sglBuffer* buffer, uint32_t color, int cntrX, int cntrY,
	int radius, float startAngle, float endAngle);

/**
 * @brief Draw a filled cricle on the buffer
 * @param buffer Buffer to draw on
 * @param color Fill color of the circle
 * @param ctrX x coordinate of the center of the cirlce
 * @param ctrY y coordinate of the center of the cirlce
 * @param radius radius of the circle
 */
void sglFillArc(sglBuffer* buffer, uint32_t color, int cntrX, int cntrY,
	int radius, float startAngle, float endAngle);

/**
 * @brief Draw a triangle
 * @param buffer the buffer to draw on
 * @param color stroke color of triangle
 * @param x1 x coordinate of first point
 * @param y1 y coordinate of first point
 * @param x2 x coordinate of second point
 * @param y2 y coordinate of second point
 * @param x3 x coordinate of thrid point
 * @param y3 y coordinate of thrid point
 */
void sglDrawTriangle(sglBuffer* buffer, uint32_t color, int x1, int y1, int x2,
	int y2, int x3, int y3);

/**
 * @brief Draw a filled triangle
 * @param buffer the buffer to draw on
 * @param color fill color of the triangle
 * @param x1 x coordinate of first point
 * @param y1 y coordinate of first point
 * @param x2 x coordinate of second point
 * @param y2 y coordinate of second point
 * @param x3 x coordinate of thrid point
 * @param y3 y coordinate of thrid point
 */
void sglFillTriangle(sglBuffer* buffer, uint32_t color, int x1, int y1, int x2,
	int y2, int x3, int y3);

/**
 * @brief Draw a filled triangle
 * @param buffer the buffer to draw on
 * @param x1 x coordinate of first point
 * @param y1 y coordinate of first point
 * @param x2 x coordinate of second point
 * @param y2 y coordinate of second point
 * @param x3 x coordinate of thrid point
 * @param y3 y coordinate of thrid point
 * @param c1 color of the first point
 * @param c2 color of the first point
 * @param c3 color of the first point
 */
void sglDrawColorInterpolatedTriangle(sglBuffer* buffer, int x1, int y1, int x2,
	int y2, int x3, int y3, uint32_t c1, uint32_t c2, uint32_t c3);

/**
 * @brief Draw a filled triangle (the texture coordinates should be in range of 0-1)
 * @param buffer the buffer to draw on
 * @param texture the texture to draw (the pixel format as to be same as of the buffers)
 * @param x1 x coordinate of first point
 * @param y1 y coordinate of first point
 * @param x2 x coordinate of second point
 * @param y2 y coordinate of second point
 * @param x3 x coordinate of thrid point
 * @param y3 y coordinate of thrid point
 * @param tx1 x coordinate of first point in the texture
 * @param ty1 y coordinate of first point in the texture
 * @param tx2 x coordinate of second point in the texture
 * @param ty2 y coordinate of second point in the texture
 * @param tx3 x coordinate of thrid point in the texture
 * @param ty3 y coordinate of thrid point in the texture
 */
void sglTextureTriangle(sglBuffer* buffer, sglBuffer* texture,
	int x1, int y1, int x2, int y2, int x3, int y3,
	float tx1, float ty1, float tx2, float ty2, float tx3, float ty3);

/**
 * @brief Draw one buffer on another buffer
 * @param buffer destination buffer - the buffer that will be drawn on
 * @param src source buffer - the buffer that will be used to draw on the other
 * @param srcRect source rectangle
 * @param dstRect destination rectangle
 */
void sglDrawBuffer(sglBuffer* buffer, const sglBuffer* src,
	const sglRect* dstRect, const sglRect* srcRect);

/**
 * @brief Draw text on a buffer using a font
 * @param buffer the buffer to draw on
 * @param text text to draw
 * @param x x location of text
 * @param y y location of text
 * @param font the font to use
 */
void sglDrawText(sglBuffer* buffer, const char* text, uint32_t color,
	 int x, int y, const sglFont* font);




/*****************************************************************************
 * UTILITY FUNCTIONS                                                         *
 *****************************************************************************/

/**
 * @brief Linear interpolation using float
 * @param a First value
 * @param b Second value
 * @param t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
float sglLerpf(float a, float b, float t);
/**
 * @brief Linear interpolation using double
 * @param a First value
 * @param b Second value
 * @param t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
double sglLerpd(double a, double b, double t);
/**
 * @brief Linear interpolation using int
 * @param a First value
 * @param b Second value
 * @param t Value between 0 and 1 that mixes a and b
 * @return Mix between a and b using t
 */
int sglLerpi(int a, int b, int t);

#define sglLerp(a, b, t) ({float retval; retval = a + t * (b - a); retval;})

/**
 * @brief Get length of a vector
 * @param x x coordinate
 * @param y y coordinate
 * @return length
 */
float sglGetLength(float x, float y);

/**
 * @brief Gets the distance between two points
 * @param a first point
 * @param b second point
 * @return distance between point a and point b
 */
float sglGetDistancePoint(sglPoint a, sglPoint b);
/**
 * @brief Gets the distance between two points
 * @param a first point
 * @param b second point
 * @return distance between point a and point b
 */
float sglGetDistance(float a_x, float a_y, float b_x, float b_y);

/**
 * @brief normalizes any angle to a range between 0 and 2π
 * @param angle the angle
 * @return new angle that is between 0 and 2π
 */
#define sglNormalizeAngle(angle) \
	fmod(fmod((angle), M_PI * 2) + M_PI * 2, M_PI * 2)

/**
 * @breif clip a line in a given rectangle
 * @param clipRect the rectangle the lines should be cliped to
 * @param startX x coordinate of the start point of the line
 * @param startY y coordinate of the start point of the line
 * @param endX x coordinate of the end point of the line
 * @param endY y coordinate of the end point of the line
 * @param[out] cstartX x coordinate of the start point of the clipped line
 * @param[out] cstartY y coordinate of the start point of the clipped line
 * @param[out] cendX x coordinate of the end point of the clipped line
 * @param[out] cendY y coordinate of the end point of the clipped line
 */
bool sglClipLine(const sglRect* clipRect, int startX, int startY, int endX,
	int endY, int* cstartX, int* cstartY, int* cendX, int* cendY);

typedef enum {
	SGL_TEXT_ALIGNMENT_LEFT   = 0,
	SGL_TEXT_ALIGNMENT_TOP    = 0,
	SGL_TEXT_ALIGNMENT_CENTER = 1,
	SGL_TEXT_ALIGNMENT_RIGHT  = 2,
	SGL_TEXT_ALIGNMENT_BOTTOM = 2,
} sglTextAlignment;

/**
 * @brief Calculates horizontal offset for given text
 * @return offset value for alignment
 */
int sglOffsetTextH(const char* text, sglTextAlignment alignment, const sglFont* font);

/**
 * @brief Calculates vertical offset for given text
 * @return offset value for alignment
 */
int sglOffsetTextV(const char* text, sglTextAlignment alignment, const sglFont* font);


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
 * @brief Looks if the pixel format has an alpha channel
 * @return True if alpha channel is present
 */
uint32_t sglHasAlphaChannel(sglPixelFormatEnum format);

void sglAlphaBlendRGBAlpha(float alpha,
		uint8_t r_a, uint8_t g_a, uint8_t b_a,
		uint8_t r_b, uint8_t g_b, uint8_t b_b,
		uint8_t* r, uint8_t* g, uint8_t* b);

void sglAlphaBlendRGBA(
		uint8_t r_a, uint8_t g_a, uint8_t b_a, uint8_t a_a,
		uint8_t r_b, uint8_t g_b, uint8_t b_b, uint8_t a_b,
		uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

uint32_t sglAlphaBlendColor(uint32_t a, uint32_t b,
		const sglPixelFormat* pf);

/**
 * @brief Get last error that occurred when calling an sgl function
 */
const char* sglGetError(void);

#endif // SGL_H
