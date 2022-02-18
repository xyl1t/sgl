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


#define DEMOS(name) void name(sglBuffer* buffer, const mouse* m, const uint8_t* k, sglPoint* cp, int ccp, uint32_t time, bool init)
typedef DEMOS(demos_f);
DEMOS(demos);

DEMOS(demo1);
DEMOS(demo2);
DEMOS(demo3);
DEMOS(demo4);
DEMOS(demo5);
DEMOS(demo6);

#endif
