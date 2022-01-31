#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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

int main (int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	const int WIDTH = 800;
	const int HEIGHT = 600;

	SDL_Window* window = SDL_CreateWindow("sgl demo", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32,
			SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	// uint32_t* pixels = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(pixels));
	// memset(pixels, 0, WIDTH * HEIGHT * sizeof(uint32_t));

	sglBuffer* buf = sglCreateNewBuffer(WIDTH, HEIGHT, SGL_PIXELFORMAT_ABGR32);

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

	sglSetPixel(buf,
		0, 0,
		7, 7, 3, 4);
	uint8_t r, g, b, a;
	sglGetPixel(buf, 0, 0, &r, &g, &b, &a);
	SGL_DEBUG_PRINT("%#010x\n", sglGetPixelRaw(buf, 0, 0));
	SGL_DEBUG_PRINT("r %d\n", r);
	SGL_DEBUG_PRINT("g %d\n", g);
	SGL_DEBUG_PRINT("b %d\n", b);
	SGL_DEBUG_PRINT("a %d\n", a);
	SGL_DEBUG_PRINT("\n");

	sglRect A = {.x = 0, .y = 0, .w = 5, .h = 5};
	sglRect B = {.x = 3, .y = -2, .w = 4, .h = 4};
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
	while(alive) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				alive = false;
			}
			switch(event.type) {
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					break;
			}
			uint32_t buttons = SDL_GetMouseState(&m.x, &m.y);
			m.left  = (buttons & SDL_BUTTON_LMASK) != 0;
			m.right = (buttons & SDL_BUTTON_RMASK) != 0;
		}
		if (m.left) { }
		if (m.right) { }

		// clear pixel buffer
		sglClear(buf, WIDTH, HEIGHT);

		for (int x = 0; x < buf->width; x++) {
			for (int y = 0; y < buf->height; y++) {
				// if ((x + y) % 2) continue;

				int i = x % 256;
				int j = y % 256;

				sglSetPixel(buf, x, y, j, i, 255-(i/2+j/2), 255);
			}
		}

		SDL_UpdateTexture(texture, NULL, buf->pixels, WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	sglDestroyBuffer(buf);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	
	return 0;
}

