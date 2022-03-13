#include "demo.h"
#include "sgl.h"

DEMOS(demos) {
	// TODO: make adding demos more easy,
	// maybe make an array of funciton pointers to the demos?
	// demo1(buffer, m, k, cp + 0x00, ccp - 0x00, time, init);
	// demo2(buffer, m, k, cp + 0x20, ccp - 0x20, time, init);
	// demo3(buffer, m, k, cp + 0x30, ccp - 0x30, time, init);
	// demo4(buffer, m, k, cp + 0x40, ccp - 0x40, time, init);
	// demo5(buffer, m, k, cp + 0x50, ccp - 0x50, time, init);
	demo6(buffer, m, k, cp + 0x60, ccp - 0x60, time, init);
	// printf("hallo\n");
}


#define drawControlPoint(cp, color) \
	sglDrawCircle(buffer, color, cp.x, cp.y, 3);

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
	if (init) {
		cp[1].x = 10;
		cp[1].y = 10;
		cp[2].x = 60;
		cp[2].y = 60;
		return;
	}

	sglRect clip = (sglRect) { .x = 32, .y = 32, .w = 150, .h = 150 };

	sglFillRectangle(
			buffer, 0x203040ff, 0, 0, buffer->width, buffer->height);

	sglDrawLine(buffer, 0xff0000ff, cp[1].x, cp[1].y, cp[2].x, cp[2].y);
	sglDrawRectangle(
			buffer, 0xff0000ff, cp[1].x, cp[1].y, cp[2].x - cp[1].x, cp[2].y - cp[1].y);

	sglSetClipRect(buffer, &clip);
	sglClearClipRect(buffer);

	sglDrawLine(buffer, 0x00ff00ff, cp[1].x, cp[1].y, cp[2].x, cp[2].y);

	sglDrawRectangle(
			buffer, 0x00ff00ff, cp[1].x, cp[1].y, cp[2].x - cp[1].x, cp[2].y - cp[1].y);

	sglResetClipRect(buffer);
	drawControlPoint(cp[1], 0xffffffff);
	drawControlPoint(cp[2], 0xffffffff);

}

DEMOS(demo3)
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

DEMOS(demo4)
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

	static float radius = 24;

	float agl = atan2f(cp[1].y - cp[0].y, cp[1].x - cp[0].x);

	cp[3].x = cos(agl) * radius + cp[0].x;
	cp[3].y = sin(agl) * radius + cp[0].y;

	// float radius = sglGetDistancePoint(cp[0], cp[3]);

	sglFillArc(
			buffer, 0xffffffff, cp[0].x, cp[0].y, radius, start_angle, end_angle);
	sglDrawArc(
			buffer, 0x3377ffff, cp[0].x, cp[0].y, radius, start_angle, end_angle);

	sglDrawLine(buffer, 0x00ff00ff, cp[0].x, cp[0].y, cp[1].x, cp[1].y);
	sglDrawLine(buffer, 0xff0000ff, cp[0].x, cp[0].y, cp[2].x, cp[2].y);

	drawControlPoint(cp[0], 0x3377ffff);
	drawControlPoint(cp[1], 0x00ff00ff);
	drawControlPoint(cp[2], 0xff0000ff);
	sglFillCircle(buffer, 0xffff00ff, cp[3].x, cp[3].y, 3);

	float angle = atan2f(m->y - cp[0].y, m->x - cp[0].x);

	if (m->left && ccp == 3) {
		radius = sglGetDistance(m->x, m->y, cp[0].x, cp[0].y);
	}
	if (m->left && ccp == 1) {
		start_angle = angle;
	}
	if (m->left && ccp == 2) {
		end_angle = angle;
	}

}

DEMOS(demo5)
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

	drawControlPoint(cp[0], 0xff0000ff);
	drawControlPoint(cp[1], 0x00ff00ff);
	drawControlPoint(cp[2], 0x0000ffff);

	drawControlPoint(cp[3], 0xffffffff);
	drawControlPoint(cp[4], 0xffffffff);
	drawControlPoint(cp[5], 0xffffffff);
}

DEMOS(demo6)
{
	static sglBitmap* bmp = NULL;
	static sglRect previewRect;

	if (init || !bmp) {
		sglFreeBitmap(bmp);
		// bmp = sglLoadBitmap("../res/cidr.png", SGL_PIXELFORMAT_ABGR32);
		bmp = sglLoadBitmap("../res/cidr.png", SGL_PIXELFORMAT_ABGR32);
		SGL_DEBUG_PRINT("init bmp: %p\n", bmp);

		previewRect.w = 48;
		previewRect.h = bmp->height / (float)bmp->width * previewRect.w;
		previewRect.x = buffer->width - previewRect.w - 8;
		previewRect.y = 8;

		// result
		cp[0] = (sglPoint){ .x = 16, .y = 16 };
		cp[1] = (sglPoint){ .x = bmp->width-16, .y = bmp->height-16 };

		// preview
		cp[2] = (sglPoint){ .x = previewRect.x, .y = previewRect.y };
		cp[3] = (sglPoint){ .x = previewRect.x + previewRect.w, .y = previewRect.y + previewRect.h };

		return;
	}

	sglRect src;
	src.x = (cp[2].x - previewRect.x) / (float)previewRect.w * bmp->width;
	src.y = (cp[2].y - previewRect.y) / (float)previewRect.h * bmp->height;
	src.w = (cp[3].x - cp[2].x) / (float)previewRect.w * bmp->width;;
	src.h = (cp[3].y - cp[2].y) / (float)previewRect.h * bmp->height;

	sglRect dst = (sglRect) {
		.x = cp[0].x,
		.y = cp[0].y,
		.w = cp[1].x - cp[0].x,
		.h = cp[1].y - cp[0].y,
	};

	sglDrawBitmap(buffer, bmp, &src, &dst);

	// draw little preview
	sglDrawBitmap(buffer, bmp, NULL, &previewRect);

	drawControlPoint(cp[0], 0xaf7fefff);
	drawControlPoint(cp[1], 0xaf7fefff);
	drawControlPoint(cp[2], 0xafff7fff);
	drawControlPoint(cp[3], 0xafff7fff);

	// TOOD: make rectangle translucent when alpha compositing is added
	sglDrawRectangle(buffer, 0xafff7fff, cp[2].x, cp[2].y, cp[3].x - cp[2].x, cp[3].y - cp[2].y);
	sglDrawRectangle(buffer, 0xaf7fefff, cp[0].x, cp[0].y, cp[1].x - cp[0].x, cp[1].y - cp[0].y);

}

