#include "SDL_rect.h"
#include "SDL_timer.h"
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
typedef struct point point;

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
		window, -1, SDL_RENDERER_ACCELERATED/*  | SDL_RENDERER_PRESENTVSYNC */);

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

		uint32_t tic = SDL_GetTicks();

		//////////////////////////////////////////////////////////////////////

		// clear pixel buffer
		sglClear(buffer);
		sglResetClipRect(buffer);

		bool test1 = false;
		bool test2 = false;
		bool test3 = false;
		bool test4 = false;
		bool test5 = true;

		if (test1) {
			for (int x = 0; x < buffer->width; x++) {
				for (int y = 0; y < buffer->height; y++) {
					// if ((x + y) % 2) continue;

					int i = x << (y % 256 / 32) % 256;
					int j = y << (x % 256 / 32) % 256;

					sglDrawPixel(buffer, j, i, 255 - (i / 2 + j / 2), 255, x, y);
				}
			}
		}

		if (test2) {
			SGL_DEBUG_PRINT("=== TEST 2 ===========\n");
			sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 400, .h = 401 };
			sglSetClipRect(buffer, &clip);


			// int x1, y1, x2, y2;
			// clipLine(&clip, 32, 20, 80, 80, &x1, &y1, &x2, &y2);
			// SGL_DEBUG_PRINT("x1: %d\n", x1);
			// SGL_DEBUG_PRINT("y1: %d\n", y1);
			// SGL_DEBUG_PRINT("x2: %d\n", x2);
			// SGL_DEBUG_PRINT("y2: %d\n", y2);
		}

		if (test3) {

			sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 150, .h = 150 };

			sglFillRectangle(buffer, 0x203040ff, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);

			sglDrawLine(buffer, 0xff0000ff, p1.x, p1.y, p2.x, p2.y);
			sglDrawRectangle(buffer, 0xff0000ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y)	;

			sglSetClipRect(buffer, &clip);
			sglClearClipRect(buffer);

			sglDrawLine(buffer, 0x00ff00ff, p1.x, p1.y, p2.x, p2.y);

			sglDrawRectangle(buffer, 0x00ff00ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y)	;

		}

		if (test4) {
			float radius = 16;
			float distance = 96;
			int maxCircles = 32;
			for (int circleCount = 0; circleCount < maxCircles; circleCount++) {
				int x = cos(circleCount/(float)maxCircles*2*3.14) * distance + buffer->width/2.f;
				int y = sin(circleCount/(float)maxCircles*2*3.14) * distance + buffer->width/2.f;

				if (circleCount % 2){
					sglDrawCircle(buffer, 0x00ff00ff, x, y, radius);
				} else {
					sglFillCircle(buffer, 0x00ff00ff, x, y, radius);
				}

				radius -= 0.5;
				distance -= 2;
			}
		}

		if (test5) {
			static float start_angle = 0;
			static float end_angle = 3*M_PI/2;
			int radius = 32;

			sglPoint s = {
				cos(start_angle)*radius + p2.x + 0.5f,
				sin(start_angle)*radius + p2.y + 0.5f
			};
			sglPoint e = {
				cos(end_angle)*radius + p2.x + 0.5f,
				sin(end_angle)*radius + p2.y + 0.5f,
			};

			sglDrawLine(buffer, 0x00ff00ff, p2.x, p2.y, s.x, s.y);
			sglDrawLine(buffer, 0xff0000ff, p2.x, p2.y, e.x, e.y);

			sglDrawCircle(buffer, 0x00ff00ff, s.x, s.y, 3);
			sglDrawCircle(buffer, 0xff0000ff, e.x, e.y, 3);

			sglDrawArc(buffer, 0xffffffff, p2.x, p2.y, radius + 1, start_angle, end_angle);

			float angle = atan2f(m.y - p2.y, m.x - p2.x);

			static bool sHeld = false;
			static bool eHeld = false;

			if (!eHeld && m.left && sglGetDistance(s.x, s.y, m.x, m.y) < 4 || sHeld) {
				start_angle = angle;
				sHeld = true;
			}
			if (!sHeld && m.left && sglGetDistance(e.x, e.y, m.x, m.y) < 4 || eHeld) {
				end_angle = angle;
				eHeld = true;
			}

			sHeld &= m.left;
			eHeld &= m.left;

			// sglDrawArc(buffer, 0x00ff00ff, m.x, m.y, 32,
			// 		0, 2*M_PI-0.1f);
			//
			// bool checkSlope(float x, float y, float k_s, float k_e, int q_s, int q_e);
			//
			// bool isInside = checkSlope(-1, -1, 0, 0, 0, 0);
			//
			// SGL_DEBUG_PRINT("Test: %i\n", isInside);
		}

		//////////////////////////////////////////////////////////////////////

		SDL_UpdateTexture(
			texture, NULL, buffer->pixels, CANVAS_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_Rect srcRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
		SDL_Rect dstRect = {0, 0, CANVAS_WIDTH, CANVAS_HEIGHT};
		SDL_RenderCopy(renderer, texture, &dstRect, &srcRect);
		SDL_RenderPresent(renderer);

		uint32_t toc = SDL_GetTicks();
		delta += toc - tic;

		static uint32_t acc = 0;
		acc++;
		if (delta > 1000) {
			SGL_DEBUG_PRINT("elapsed time: %.2fms\n", delta / (float)acc);
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
