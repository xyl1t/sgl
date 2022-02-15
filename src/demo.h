#ifndef SGL_DEMO
#define SGL_DEMO

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include <math.h>
#include "sgl.h"

typedef struct mouse {
	int x;
	int y;
	bool left;
	bool right;
} mouse;


void tests(sglBuffer* buffer, mouse* m);

void test1(sglBuffer* buffer, mouse* m);
void test2(sglBuffer* buffer, mouse* m);
void test3(sglBuffer* buffer, mouse* m);
void test4(sglBuffer* buffer, mouse* m);
void test5(sglBuffer* buffer, mouse* m);
void test6(sglBuffer* buffer, mouse* m);

#endif
