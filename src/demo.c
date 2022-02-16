#include "demo.h"

void demos(sglBuffer* buffer, mouse* m) {
	// demo1(buffer, m);
	// demo2(buffer, m);
	// demo3(buffer, m);
	// demo4(buffer, m);
	// demo5(buffer, m);
	demo6(buffer, m);
}

void demo1(sglBuffer* buffer, mouse* m)
{
	for (int x = 0; x < buffer->width; x++) {
		for (int y = 0; y < buffer->height; y++) {
			// if ((x + y) % 2) continue;

			int i = x << (y % 256 / 32) % 256;
			int j = y << (x % 256 / 32) % 256;

			sglDrawPixel(buffer, j, i, 255 - (i/2 + j/2), 255, x, y);
		}
	}
}

void demo2(sglBuffer* buffer, mouse* m)
{
	sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 400, .h = 401 };
	sglSetClipRect(buffer, &clip);
}

void demo3(sglBuffer* buffer, mouse* m)
{
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

void demo4(sglBuffer* buffer, mouse* m)
{
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

void demo5(sglBuffer* buffer, mouse* m)
{
	static sglPoint p1;
	static sglPoint p2;

	static float start_angle = 0;
	static float end_angle = 3 * M_PI / 2;
	// int radius = 32;
	// int radius = 24 + (sin(SDL_GetTicks() / 1000.f) / 2.f + 0.5) * 8;
	int radius = 24;

	sglPoint s = {
		cos(start_angle) * radius + p2.x + 0.5f,
		sin(start_angle) * radius + p2.y + 0.5f
	};
	sglPoint e = {
		cos(end_angle) * radius + p2.x + 0.5f,
		sin(end_angle) * radius + p2.y + 0.5f,
	};

	sglDrawCircle(buffer, 0x00ff00ff, s.x, s.y, 3);
	sglDrawCircle(buffer, 0xff0000ff, e.x, e.y, 3);

	sglFillArc(
			buffer, 0xffffffff, p2.x, p2.y, radius, start_angle, end_angle);
	sglDrawArc(
			buffer, 0x3377ffff, p2.x, p2.y, radius, start_angle, end_angle);

	sglDrawLine(buffer, 0x00ff00ff, p2.x, p2.y, s.x, s.y);
	sglDrawLine(buffer, 0xff0000ff, p2.x, p2.y, e.x, e.y);

	float angle = atan2f(m->y - p2.y, m->x - p2.x);

	static bool sHeld = false;
	static bool eHeld = false;

	if (!eHeld && m->left && sglGetDistance(s.x, s.y, m->x, m->y) < 6 || sHeld) {
		start_angle = angle;
		sHeld = true;
	}
	if (!sHeld && m->left && sglGetDistance(e.x, e.y, m->x, m->y) < 6 || eHeld) {
		end_angle = angle;
		eHeld = true;
	}

	sHeld &= m->left;
	eHeld &= m->left;
}

void demo6(sglBuffer* buffer, mouse* m)
{
	static sglPoint t[] = {
		{ .x = 128, .y = 32 },
		{ .x = 200, .y = 64 },
		{ .x = 100, .y = 96 },

		{ .x = 64, .y = 32 },
		{ .x = 100, .y = 64 },
		{ .x = 16, .y = 98 },
	};
	static int current = -1;

	if (m->left) {
		for (int i = 0; i < sizeof(t) && current == -1; i++) {
			if (sglGetDistance(t[i].x, t[i].y, m->x, m->y) < 6) {
				current = i;
			}
		}
		t[current].x = m->x;
		t[current].y = m->y;
	} else {
		current = -1;
	}

	sglFillTriangle(buffer, 0xffffffff,
			t[3].x, t[3].y, t[4].x, t[4].y, t[5].x, t[5].y);
	sglDrawTriangle(buffer, 0x3366EEff,
			t[3].x, t[3].y, t[4].x, t[4].y, t[5].x, t[5].y);
	sglDrawColorInterpolatedTriangle(buffer,
			t[0].x, t[0].y, t[1].x, t[1].y, t[2].x, t[2].y,
			0xff0000ff, 0x00ff00ff, 0x0000ffff);
	sglDrawTriangle(buffer, 0x3366EEff,
			t[0].x, t[0].y, t[1].x, t[1].y, t[2].x, t[2].y);

	sglDrawCircle(buffer, 0xff0000ff, t[0].x, t[0].y, 3);
	sglDrawCircle(buffer, 0x00ff00ff, t[1].x, t[1].y, 3);
	sglDrawCircle(buffer, 0x0000ffff, t[2].x, t[2].y, 3);

	sglDrawCircle(buffer, 0xffffffff, t[3].x, t[3].y, 3);
	sglDrawCircle(buffer, 0xffffffff, t[4].x, t[4].y, 3);
	sglDrawCircle(buffer, 0xffffffff, t[5].x, t[5].y, 3);
}

