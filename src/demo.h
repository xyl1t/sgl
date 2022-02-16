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


#define DEMOS(name) void name(sglBuffer* buffer, mouse* m)
typedef DEMOS(demos_f);
void demos(sglBuffer* buffer, mouse* m);

void demo1(sglBuffer* buffer, mouse* m);
void demo2(sglBuffer* buffer, mouse* m);
void demo3(sglBuffer* buffer, mouse* m);
void demo4(sglBuffer* buffer, mouse* m);
void demo5(sglBuffer* buffer, mouse* m);
void demo6(sglBuffer* buffer, mouse* m);

#endif
