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


/**
 * @brief demo function template
 * @param currDemo current demo index
 * @param buffer buffere to draw on
 * @param m mouse info (coordinates, button click)
 * @param k keyboard info
 * @param cp list of control points for current demo (beginning at 0 for all demos)
 * @param ccp current control point index
 * @param time elapsed time between frames in ms
 * @param init if the demo should initialize
 * 
 * the function should have this structure:
 * DEMOS(demoName) {
 *     if (init) {
 *         // init stuff
 *        return;
 *     }
 *
 *     // demo code
 * }
 */
#define DEMOS(name) void name(int currDemo, sglBuffer* buffer, const mouse* m, const uint8_t* k, sglPoint* cp, int ccp, uint32_t time, bool init)
typedef DEMOS(demos_f);
DEMOS(demos);

#define DEMOS_COUNT 8
#define CONTROL_POINTS_PER_DEMO_COUNT 0x20
demos_f* demoArr[DEMOS_COUNT];

#endif
