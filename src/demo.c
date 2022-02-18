#include "demo.h"

DEMOS(demos) {
	// demo1(buffer, m, k, cp + 0x00, ccp - 0x00, time, init);
	// demo2(buffer, m, k, cp + 0x10, ccp - 0x10, time, init);
	// demo3(buffer, m, k, cp + 0x20, ccp - 0x20, time, init);
	// demo4(buffer, m, k, cp + 0x30, ccp - 0x30, time, init);
	demo5(buffer, m, k, cp + 0x40, ccp - 0x40, time, init);
	// demo6(buffer, m, k, cp + 0x50, ccp - 0x50, time, init);
}

DEMOS(demo1)
{
	if (init) { return; }

	for (int x = 0; x < buffer->width; x++) {
		for (int y = 0; y < buffer->height; y++) {
			// if ((x + y) % 2) continue;

			int i = x << (y % 256 / 32) % 256;
			int j = y << (x % 256 / 32) % 256;

			sglDrawPixel(buffer, j, i, 255 - (i/2 + j/2), 255, x, y);
		}
	}
}

DEMOS(demo2)
{
	if (init) { return; }

	sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 400, .h = 401 };
	sglSetClipRect(buffer, &clip);
}

DEMOS(demo3)
{
	if (init) { return; }

	static sglPoint p1;
	static sglPoint p2;

	sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 150, .h = 150 };

	sglFillRectangle(
			buffer, 0x203040ff, 0, 0, buffer->width, buffer->height);

	sglDrawLine(buffer, 0xff0000ff, p1.x, p1.y, p2.x, p2.y);
	sglDrawRectangle(
			buffer, 0xff0000ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

	sglSetClipRect(buffer, &clip);
	sglClearClipRect(buffer);

	sglDrawLine(buffer, 0x00ff00ff, p1.x, p1.y, p2.x, p2.y);

	sglDrawRectangle(
			buffer, 0x00ff00ff, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
}

DEMOS(demo4)
{
	if (init) { return; }

	float radius = 16;
	float distance = 96;
	int maxCircles = 32;
	for (int circleCount = 0; circleCount < maxCircles; circleCount++) {
		int x = cos(circleCount / (float)maxCircles * 2 * 3.14) * distance + buffer->width / 2.f;
		int y = sin(circleCount / (float)maxCircles * 2 * 3.14) * distance + buffer->width / 2.f;

		if (circleCount % 2) {
			sglDrawCircle(buffer, 0x00ff00ff, x, y, radius);
		} else {
			sglFillCircle(buffer, 0x00ff00ff, x, y, radius);
		}

		radius -= 0.5;
		distance -= 2;
	}
}

DEMOS(demo5)
{
	if (init) {
		cp[0].x = 128;
		cp[0].y = 128;
		cp[1].x = 120;
		cp[1].y = 110;
		cp[2].x = 130;
		cp[2].y = 150;
		cp[3].x = 128;
		cp[3].y = 152;
		return;
	}

	float start_angle = atan2f(cp[1].y - cp[0].y, cp[1].x - cp[0].x);
	float end_angle = atan2f(cp[2].y - cp[0].y, cp[2].x - cp[0].x);

	float radius = sglGetDistancePoint(cp[0], cp[3]);

	sglFillArc(
			buffer, 0xffffffff, cp[0].x, cp[0].y, radius, start_angle, end_angle);
	sglDrawArc(
			buffer, 0x3377ffff, cp[0].x, cp[0].y, radius, start_angle, end_angle);

	sglDrawLine(buffer, 0x00ff00ff, cp[0].x, cp[0].y, cp[1].x, cp[1].y);
	sglDrawLine(buffer, 0xff0000ff, cp[0].x, cp[0].y, cp[2].x, cp[2].y);

	sglDrawCircle(buffer, 0x3377ffff, cp[0].x, cp[0].y, 3);
	sglDrawCircle(buffer, 0x00ff00ff, cp[1].x, cp[1].y, 3);
	sglDrawCircle(buffer, 0xff0000ff, cp[2].x, cp[2].y, 3);
	sglDrawCircle(buffer, 0x3377ffff, cp[3].x, cp[3].y, 3);

	float angle = atan2f(m->y - cp[0].y, m->x - cp[0].x);

	if (m->left && ccp == 1) {
		start_angle = angle;
	}
	if (m->left && ccp == 2) {
		end_angle = angle;
	}

}

DEMOS(demo6)
{
	if (init) {
		cp[0] = (sglPoint){ .x = 128, .y = 32 };
		cp[1] = (sglPoint){ .x = 200, .y = 64 };
		cp[2] = (sglPoint){ .x = 100, .y = 96 };
		cp[3] = (sglPoint){ .x =  64, .y = 32 };
		cp[4] = (sglPoint){ .x = 100, .y = 64 };
		cp[5] = (sglPoint){ .x =  16, .y = 98 };
		return;
	}

	sglFillTriangle(buffer, 0xffffffff,
			cp[3].x, cp[3].y, cp[4].x, cp[4].y, cp[5].x, cp[5].y);
	sglDrawTriangle(buffer, 0x3366EEff,
			cp[3].x, cp[3].y, cp[4].x, cp[4].y, cp[5].x, cp[5].y);
	sglDrawColorInterpolatedTriangle(buffer,
			cp[0].x, cp[0].y, cp[1].x, cp[1].y, cp[2].x, cp[2].y,
			0xff0000ff, 0x00ff00ff, 0x0000ffff);
	sglDrawTriangle(buffer, 0x3366EEff,
			cp[0].x, cp[0].y, cp[1].x, cp[1].y, cp[2].x, cp[2].y);

	sglDrawCircle(buffer, 0xff0000ff, cp[0].x, cp[0].y, 3);
	sglDrawCircle(buffer, 0x00ff00ff, cp[1].x, cp[1].y, 3);
	sglDrawCircle(buffer, 0x0000ffff, cp[2].x, cp[2].y, 3);

	sglDrawCircle(buffer, 0xffffffff, cp[3].x, cp[3].y, 3);
	sglDrawCircle(buffer, 0xffffffff, cp[4].x, cp[4].y, 3);
	sglDrawCircle(buffer, 0xffffffff, cp[5].x, cp[5].y, 3);
}

