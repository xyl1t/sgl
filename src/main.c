#include "SDL_keyboard.h"
#include "SDL_rect.h"
#include "SDL_timer.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "sgl.h"
#include "demo.h"

const uint8_t* keyboard;
static mouse m;

const char* demoLibPath = "demo.so";

time_t getFileCreationTime(const char *path) {
	struct stat attr;
	stat(path, &attr);
	return attr.st_mtime;
}

demos_f* reloadDemos(void)
{
	static void* libDemoHandle = NULL;

	if (libDemoHandle) {
		dlclose(libDemoHandle);
		libDemoHandle = NULL;
	}

	libDemoHandle = dlopen(demoLibPath, RTLD_NOW);
	demos_f* dyDemos = NULL;

	if (libDemoHandle) {
		dyDemos = dlsym(libDemoHandle, "demos");
		char* result = dlerror();
		if (result) {
			printf("Cannot find demos() in %s: %s\n", demoLibPath, result);
		}
	} else {
		printf("Cannot load %s: %s\n", demoLibPath, dlerror());
	}

	if (!dyDemos) {
		dyDemos = demos;
		printf("Error: demo.so not aviable\n");
	}

	return dyDemos;
}

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


	// NOTE: 0xF(=16) demos and every demo has 0xF(=16) control points
	sglPoint controlPoints[0xFF] = { };
	int currentControlPoint = -1;

	demos_f* dyDemos = reloadDemos();
	dyDemos(buffer, &m, keyboard, controlPoints, currentControlPoint, 0, true);
	time_t demoLibCreationTime = 0;

	SDL_Event event;
	bool alive = true;
	uint32_t delta = 0;

	while (alive) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				alive = false;
			}
			// TODO: reload demos on window focus
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_r) {
					SGL_DEBUG_PRINT("Reloading %s...\n", demoLibPath);
					dyDemos = reloadDemos();
					dyDemos(buffer, &m, keyboard, controlPoints, currentControlPoint, 0, true);
				}
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
			for (int i = 0; i < sizeof(controlPoints) / sizeof(controlPoints[0]) && currentControlPoint == -1; i++) {
				if (sglGetDistance(controlPoints[i].x, controlPoints[i].y, m.x, m.y) < 6) {
					currentControlPoint = i;
				}
			}
			controlPoints[currentControlPoint].x = m.x;
			controlPoints[currentControlPoint].y = m.y;
		} else {
			currentControlPoint = -1;
		}

		uint32_t tic = SDL_GetTicks();

		time_t now = getFileCreationTime(demoLibPath);
		if (now > demoLibCreationTime) {
			SGL_DEBUG_PRINT("Reloading %s...\n", demoLibPath);
			dyDemos = reloadDemos();
			demoLibCreationTime = now;
		}


		//////////////////////////////////////////////////////////////////////

		// clear pixel buffer
		sglClear(buffer);
		sglResetClipRect(buffer);

		dyDemos(buffer, &m, keyboard, controlPoints, currentControlPoint, tic, false);

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

// nnoremap <leader>b <cmd>wa<cr><cmd>!pushd build/ && ./build.sh; popd<cr>
