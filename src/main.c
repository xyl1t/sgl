#include "SDL_rect.h"
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

struct mouse {
	int x;
	int y;
	bool left;
	bool right;
} m;

struct point {
	int x;
	int y;
} p1, p2;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	const int WINDOW_WIDTH = 512;
	const int WINDOW_HEIGHT = 512;

	const int CANVAS_WIDTH = 256;
	const int CANVAS_HEIGHT = 256;

	SDL_Window* window = SDL_CreateWindow("sgl demo", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332,
		SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	uint8_t* pixels
		= (uint8_t*)malloc(CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(pixels));
	memset(pixels, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(uint8_t));

	sglBuffer* buf = sglCreateBuffer(
		pixels, CANVAS_WIDTH, CANVAS_HEIGHT, SGL_PIXELFORMAT_RGB332);

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

	sglDrawPixel(buf, 0, 0, 7, 7, 3, 4);
	uint8_t r, g, b, a;
	sglGetPixel(buf, &r, &g, &b, &a, 0, 0);
	SGL_DEBUG_PRINT("%#010x\n", sglGetPixelRaw(buf, 0, 0));
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

	SDL_Event event;
	bool alive = true;
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
			m.x /= WINDOW_WIDTH / CANVAS_WIDTH;
			m.y /= WINDOW_HEIGHT / CANVAS_HEIGHT;
			m.left = (buttons & SDL_BUTTON_LMASK) != 0;
			m.right = (buttons & SDL_BUTTON_RMASK) != 0;
		}
		if (m.left) {
			p1.x = m.x;
			p1.y = m.y;
		}
		if (m.right) {
			p2.x = m.x;
			p2.y = m.y;
		}

		// clear pixel buffer
		sglClear(buf);
		sglResetClipRect(buf);

		sglDrawPixel(buf, 7, 7, 3, 0, 10, 20);

		bool test1 = false;
		bool test2 = false;
		bool test3 = false;

		if (test1) {
			for (int x = 0; x < buf->width; x++) {
				for (int y = 0; y < buf->height; y++) {
					// if ((x + y) % 2) continue;

					int i = x << (y % 256 / 32) % 256;
					int j = y << (x % 256 / 32) % 256;

					sglDrawPixel(buf, j, i, 255 - (i / 2 + j / 2), 255, x, y);
				}
			}
		}

		if (test2) {
			SGL_DEBUG_PRINT("=== TEST 2 ===========\n");
			sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 400, .h = 401 };
			sglSetClipRect(buf, &clip);


			// int x1, y1, x2, y2;
			// clipLine(&clip, 32, 20, 80, 80, &x1, &y1, &x2, &y2);
			// SGL_DEBUG_PRINT("x1: %d\n", x1);
			// SGL_DEBUG_PRINT("y1: %d\n", y1);
			// SGL_DEBUG_PRINT("x2: %d\n", x2);
			// SGL_DEBUG_PRINT("y2: %d\n", y2);
		}

		if (test3) {

			sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 150, .h = 150 };

			sglFillRectangle(buf, 0x203040ff, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

			sglDrawLine(buf, 0xff0000ff, p1.x, p1.y, p2.x, p2.y);
			sglDrawRectangle(buf, 0xff0000ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y)	;

			sglSetClipRect(buf, &clip);
			sglClearClipRect(buf);

			sglDrawLine(buf, 0x00ff00ff, p1.x, p1.y, p2.x, p2.y);

			sglDrawRectangle(buf, 0x00ff00ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y)	;

		}


		// clock_t begin = clock();
		// for (int i = 0; i < 1000; i++)
		// 	sglFillRectangle(buf, 0x00ff00ff, 0, 0, buf->width, buf->height);
		// clock_t end = clock();
		// double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		// SGL_DEBUG_PRINT("time: %lf\n", time_spent);

		SDL_UpdateTexture(
			texture, NULL, buf->pixels, CANVAS_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_Rect srcRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
		SDL_Rect dstRect = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
		SDL_RenderCopy(renderer, texture, &dstRect, &srcRect);
		SDL_RenderPresent(renderer);
	}

	sglDestroyBuffer(buf);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
