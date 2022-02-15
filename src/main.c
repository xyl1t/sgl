#include "SDL_keyboard.h"
#include "SDL_rect.h"
#include "SDL_timer.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "sgl.h"
#include "demo.h"

const uint8_t* keyboard;
static mouse m;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	const int WINDOW_WIDTH = 512;
	const int WINDOW_HEIGHT = 512;

	const int CANVAS_WIDTH = 256;
	const int CANVAS_HEIGHT = 256;

	SDL_Window* window = SDL_CreateWindow("sgl demo", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED /*  | SDL_RENDERER_PRESENTVSYNC */);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32,
		SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	uint32_t* pixels
		= (uint32_t*)malloc(CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(pixels));
	memset(pixels, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(uint32_t));

	sglBuffer* buffer = sglCreateBuffer(
		pixels, CANVAS_WIDTH, CANVAS_HEIGHT, SGL_PIXELFORMAT_ABGR32);

	// SGL_DEBUG_PRINT("SGL_PIXELFORMAT_ABGR32 %#010x\n",
	// 		sglGetChannelLayout(SGL_PIXELFORMAT_ABGR32));
	// SGL_DEBUG_PRINT("SGL_PIXELFORMAT_BGRA32 %#010x\n",
	// 		sglGetChannelLayout(SGL_PIXELFORMAT_BGRA32));
	// SGL_DEBUG_PRINT("SGL_PIXELFORMAT_RGBA32 %#010x\n",
	// 		sglGetChannelLayout(SGL_PIXELFORMAT_RGBA32));
	// SGL_DEBUG_PRINT("SGL_PIXELFORMAT_ARGB32 %#010x\n",
	// 		sglGetChannelLayout(SGL_PIXELFORMAT_ARGB32));
	// SGL_DEBUG_PRINT("SGL_PIXELFORMAT_RGB332 %#010x\n",
	// 		sglGetChannelLayout(SGL_PIXELFORMAT_RGB332));

	sglDrawPixel(buffer, 0, 0, 7, 7, 3, 4);
	uint8_t r, g, b, a;
	sglGetPixel(buffer, &r, &g, &b, &a, 0, 0);
	SGL_DEBUG_PRINT("%#010x\n", sglGetPixelRaw(buffer, 0, 0));
	SGL_DEBUG_PRINT("r %d\n", r);
	SGL_DEBUG_PRINT("g %d\n", g);
	SGL_DEBUG_PRINT("b %d\n", b);
	SGL_DEBUG_PRINT("a %d\n", a);
	SGL_DEBUG_PRINT("\n");

	sglRect A = { .x = 0, .y = 0, .w = 5, .h = 5 };
	sglRect B = { .x = 3, .y = -2, .w = 4, .h = 4 };
	sglRect result;

	SGL_DEBUG_PRINT("intersect %d\n", sglIntersectRect(&A, &B, &result));
	SGL_DEBUG_PRINT("r.x = %d\n", result.x);
	SGL_DEBUG_PRINT("r.y = %d\n", result.y);
	SGL_DEBUG_PRINT("r.w = %d\n", result.w);
	SGL_DEBUG_PRINT("r.h = %d\n", result.h);
	SGL_DEBUG_PRINT("\n");

	// sglRect clipRect = {
	// 	.x = -25,
	// 	.y = -25,
	// 	.w = 100,
	// 	.h = 100
	// };
	// SGL_DEBUG_PRINT("clip %d\n", sglSetClipRect(buf, &clipRect));
	//
	// SGL_DEBUG_PRINT("clip.x = %d\n", clipRect.x);
	// SGL_DEBUG_PRINT("clip.y = %d\n", clipRect.y);
	// SGL_DEBUG_PRINT("clip.w = %d\n", clipRect.w);
	// SGL_DEBUG_PRINT("clip.h = %d\n", clipRect.h);

	// return 0;


	sglPoint p1;
	sglPoint p2;

	p2.x = 64;
	p2.y = 64;

	SDL_Event event;
	bool alive = true;
	uint32_t delta = 0;

	while (alive) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				alive = false;
			}
			switch (event.type) {
			case SDL_KEYDOWN:
				break;
			case SDL_KEYUP:
				break;
			}
			uint32_t buttons = SDL_GetMouseState(&m.x, &m.y);
			m.x /= WINDOW_WIDTH / (float)CANVAS_WIDTH;
			m.y /= WINDOW_HEIGHT / (float)CANVAS_HEIGHT;
			m.left = (buttons & SDL_BUTTON_LMASK) != 0;
			m.right = (buttons & SDL_BUTTON_RMASK) != 0;

			keyboard = SDL_GetKeyboardState(NULL);
		}
		if (m.left) {
			p1.x = m.x;
			p1.y = m.y;
		}
		if (m.right) {
			p2.x = m.x;
			p2.y = m.y;
		}

		uint32_t tic = SDL_GetTicks();

		//////////////////////////////////////////////////////////////////////

		// clear pixel buffer
		sglClear(buffer);
		sglResetClipRect(buffer);

		tests(buffer, &m);

		//////////////////////////////////////////////////////////////////////

		SDL_UpdateTexture(
			texture, NULL, buffer->pixels, CANVAS_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_Rect srcRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		SDL_Rect dstRect = { 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT };
		SDL_RenderCopy(renderer, texture, &dstRect, &srcRect);
		SDL_RenderPresent(renderer);

		uint32_t toc = SDL_GetTicks();
		delta += toc - tic;

		static uint32_t acc = 0;
		acc++;
		if (delta > 1000) {
			SGL_DEBUG_PRINT("elapsed time: %.2fms\n", delta / (float)acc);
			// SGL_DEBUG_PRINT("x: %d, y: %d\n", m.x, m.y);
			delta = 0;
			acc = 0;
		}
	}

	sglDestroyBuffer(buffer);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

