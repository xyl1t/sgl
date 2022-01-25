#include <stdio.h>

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

int main (int argc, char *argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	int test;

	const int WIDTH = 800;
	const int HEIGHT = 600;

	SDL_Window* window = SDL_CreateWindow("sgl demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	uint32_t* pixels = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(pixels));
	memset(pixels, 0, WIDTH * HEIGHT * sizeof(uint32_t));

	int mx = 0, my = 0;
	bool mleft = false, mright = false;

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
			uint32_t buttons = SDL_GetMouseState(&mx, &my);
			mleft  = (buttons & SDL_BUTTON_LMASK) != 0;
			mright = (buttons & SDL_BUTTON_RMASK) != 0;
		}

		// clear pixel buffer
		memset(pixels, 0, WIDTH * HEIGHT * sizeof(uint32_t));

		pixels[100 + 100 * WIDTH] = 0xff00ffff;

		SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	
	return 0;
}
