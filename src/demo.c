#include "demo.h"
#include "sgl.h"

static sglFont* font;

DEMOS(demo0);
DEMOS(demo1);
DEMOS(demo2);
DEMOS(demo3);
DEMOS(demo4);
DEMOS(demo5);
DEMOS(demo6);

DEMOS(demos) {
	// TODO: make adding demos more easy, maybe make an array of
	// funciton pointers to the demos? - partially done
	// TODO: switching between demos (using numbers?) - already done
	
	if (init) {
		SGL_DEBUG_PRINT("Initializing demos\n");
		sglFreeFont(font);
		font = sglCreateFont("../res/xterm7x14.png", 7, 14, true);

#define initDemoArr(demoNum) \
	demoArr[demoNum] = demo##demoNum; \
	demoArr[demoNum](0, buffer, m, k, cp + 0x10 * demoNum, ccp - 0x10 * demoNum, time, init);

		initDemoArr(0);
		initDemoArr(1);
		initDemoArr(2);
		initDemoArr(3);
		initDemoArr(4);
		initDemoArr(5);
		initDemoArr(6);

#undef initDemoArr
		return;
	}

	demoArr[currDemo](currDemo, buffer, m, k, cp + 0x10 * currDemo, ccp - 0x10 * currDemo, time, init);
}


#define drawControlPoint(cp, color) \
	sglDrawCircle(buffer, color, cp.x, cp.y, 3);

// normlized versions
#define sin_n(_angle) ((1+sin(_angle))/2.f)
#define cos_n(_angle) ((1+cos(_angle))/2.f)

DEMOS(demo0)
{

	// static sglFont* font = NULL;
	static float text_x = 0;
	static float text_y = 0;

	if (init) {
		// sglFreeFont(font);
		// font = sglCreateFont("../res/xterm7x14.png", 7, 14, true);
		return;
	}
	
	const char* text = "sgl alpha demo";
	sglDrawText(buffer, text, text_x, text_y, font);
	
	float offset = sglOffsetTextH(text, SGL_TEXT_ALIGNMENT_CENTER, font);
	
	float radius = 150;

	text_x = sin_n(time/829.f+18.29)*radius + offset + (256-radius)/2.f;
	text_y = cos_n(time/989.f)*radius + (256-radius)/2.f;


	for (int i = 0; i < buffer->width; i++) {
		for (int j = 0; j < buffer->height; j++) {
			// if ((x + y) % 2) continue;

			uint8_t x = i % 256;
			uint8_t y = j % 256;

			// int r = x << (y % 256 / 32) % 256;
			// int g = y << (x % 256 / 32) % 256;
			// int b = 255 - (r/2 + g/2);

			// // int r = (y & (x^~y)) >> (~x&~y) | ((y & (x^~y)));
			// // int g = (x & (x^y) ) >> (~x& y) | ((x & (x^y) ));
			// // int b = (y & (x^y) ) >> ( x&~y) | ((y & (x^y) ));

			uint8_t baseR = y & (x^y);
			uint8_t baseG = x & (x^y);
			uint8_t baseB = y & (~x^y);

			uint8_t r = (baseR >> (~x& y)) | baseR;//(256) >> (~x&y) & (x&y);
			uint8_t g = (baseG >> ( x&~y)) | baseG;//(256) >> (x&~y) & (x&y);
			uint8_t b = (baseB >> ( x& y)) | baseB;//(256) >> (x&y ) & (x&y);
			uint8_t a = x|y;//(256) >> (x&y ) & (x&y);

			// uint8_t b = y & (x^~y);
			// uint8_t g = ~x & (x^y) ;
			// uint8_t r = ~y & (x^y) ;

			sglDrawPixel(buffer, r, g, b, a, i, j);
		}
	}
}

DEMOS(demo1)
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

DEMOS(demo2)
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

DEMOS(demo3)
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

DEMOS(demo4)
{
	if (init) {
		cp[0] = (sglPoint){ .x = 128, .y = 32 };
		cp[1] = (sglPoint){ .x = 200, .y = 64 };
		cp[2] = (sglPoint){ .x = 100, .y = 96 };

		for(int i = 1; i < 5; i++) {
			cp[0 + i*3] = (sglPoint){ .x =  64 + rand()%3-1, .y = 32 + rand()%3-1};
			cp[1 + i*3] = (sglPoint){ .x = 100 + rand()%3-1, .y = 64 + rand()%3-1};
			cp[2 + i*3] = (sglPoint){ .x =  16 + rand()%3-1, .y = 98 + rand()%3-1};
		}

		return;
	}

	sglDrawColorInterpolatedTriangle(buffer,
			cp[0].x, cp[0].y, cp[1].x, cp[1].y, cp[2].x, cp[2].y,
			0xff0000ff, 0x00ff00ff, 0x0000ffff);


	sglFillTriangle(buffer, 0xffffffff,
			cp[3].x, cp[3].y, cp[4].x, cp[4].y, cp[5].x, cp[5].y);
	sglDrawTriangle(buffer, 0x3366EEff,
			cp[3].x, cp[3].y, cp[4].x, cp[4].y, cp[5].x, cp[5].y);

	// TODO: maybe add another set of triangles that have other colors but without alpha
	sglDrawColorInterpolatedTriangle(buffer,
			cp[6].x, cp[6].y, cp[7].x, cp[7].y, cp[8].x, cp[8].y,
			0, 0, 0x0000ffff);
	sglDrawColorInterpolatedTriangle(buffer,
			cp[9].x, cp[9].y, cp[10].x, cp[10].y, cp[11].x, cp[11].y,
			0xff0000ff, 0, 0);
	sglDrawColorInterpolatedTriangle(buffer,
			cp[12].x, cp[12].y, cp[13].x, cp[13].y, cp[14].x, cp[14].y,
			0, 0x00ff00ff, 0);

	for (int i = 0; i < 5; i++) {
		sglDrawTriangle(buffer, 0x3366EEff,
			cp[0 + i*3].x, cp[0 + i*3].y, cp[1 + i*3].x, cp[1 + i*3].y, cp[2 + i*3].x, cp[2 + i*3].y);
	}

	for(int i = 0; i < 5; i++) {
		if (i != 1) {
			drawControlPoint(cp[0 + i * 3], 0xff0000ff);
			drawControlPoint(cp[1 + i * 3], 0x00ff00ff);
			drawControlPoint(cp[2 + i * 3], 0x0000ffff);
		} else {
			drawControlPoint(cp[3], 0xffffffff);
			drawControlPoint(cp[4], 0xffffffff);
			drawControlPoint(cp[5], 0xffffffff);
		}
	}

}

DEMOS(demo5)
{
	static sglBuffer* bmp = NULL;
	static sglRect previewRect;
	// static sglFont* font;

	if (init || !bmp) {
		sglFreeBuffer(bmp);
		bmp = sglLoadBitmap("../res/cidr.png", SGL_PIXELFORMAT_ABGR32);

		// sglFreeFont(font);
		// font = sglCreateFont("../res/xterm7x14.png", 7, 14, true);

		previewRect.w = 48;
		previewRect.h = bmp->height / (float)bmp->width * previewRect.w;
		previewRect.x = buffer->width - previewRect.w - 8;
		previewRect.y = 8;

		// result
		cp[0] = (sglPoint){ .x = 8, .y = 8 };
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

	sglDrawBuffer(buffer, bmp, &dst, &src);

	// draw little preview
	sglDrawBuffer(buffer, bmp, &previewRect, NULL);

	const char* text = "Preview";
	sglDrawText(buffer, text,
		previewRect.x + previewRect.w / 2 + sglOffsetTextH(text, SGL_TEXT_ALIGNMENT_CENTER, font),
		previewRect.y + previewRect.h + 2,
		font);

	drawControlPoint(cp[0], 0xaf7fefff);
	drawControlPoint(cp[1], 0xaf7fefff);
	drawControlPoint(cp[2], 0xafff7fff);
	drawControlPoint(cp[3], 0xafff7fff);

	// TOOD: make rectangle translucent when alpha compositing is added
	sglDrawRectangle(buffer, 0xafff7fff, cp[2].x, cp[2].y, cp[3].x - cp[2].x, cp[3].y - cp[2].y);
	sglDrawRectangle(buffer, 0xaf7fefff, cp[0].x, cp[0].y, cp[1].x - cp[0].x, cp[1].y - cp[0].y);

	if (k[SDL_SCANCODE_S]) {
		printf("saving bitmap...\n");
		sglSaveBufferToFile(bmp, "bitmap.png", SGL_BITMAPFORMAT_PNG);
	}

}

DEMOS(demo6)
{
	// static sglFont* font = NULL;
	static sglBuffer* bmp = NULL;
	static sglPoint prev[4];
	static sglFont* fontWithoutKern;

	if (init) {
		// sglFreeFont(font);
		// font = sglCreateFont("../res/xterm7x14.png", 7, 14, true);
		sglFreeBuffer(bmp);
		bmp = sglLoadBitmap("../res/gradient.png", SGL_PIXELFORMAT_ABGR32);
		sglFreeFont(fontWithoutKern);
		fontWithoutKern = sglCreateFont("../res/xterm7x14.png", 7, 14, false);

		prev[0].x = cp[0].x = 0;
		prev[0].y = cp[0].y = 0;
		prev[1].x = cp[1].x = bmp->width;
		prev[1].y = cp[1].y = 0;
		prev[2].x = cp[2].x = bmp->width;
		prev[2].y = cp[2].y = bmp->height;
		prev[3].x = cp[3].x = 0;
		prev[3].y = cp[3].y = bmp->height;

		return;
	}
	
	if (ccp >= 0)
	for (int i = 0; i < 4; i++) {
		
		if (i == ccp) continue;

		prev[i].x = cp[i].x;
		prev[i].y = cp[i].y;

		cp[i].x += cp[ccp].x - prev[ccp].x;
		cp[i].y += cp[ccp].y - prev[ccp].y;
	}

	prev[ccp].x = cp[ccp].x;
	prev[ccp].y = cp[ccp].y;

	sglRect r = {
		8, 8,
		font->fontSheet->width,
		font->fontSheet->height,
	};

	sglDrawBuffer(buffer, font->fontSheet, &r, NULL);

	for (int x = 0; x < font->cols; x++) {
		for (int y = 0; y < font->rows; y++) {
			int leftKern = sglGetKern(font, x, y, sglLeftKern);
			int rightKern = sglGetKern(font, x, y, sglRightKern);
			// SGL_DEBUG_PRINT("rightKern: %d %d - %d\n", x, y, rightKern);

			// TODO: use blending to draw just a line overlay
			sglDrawLine(buffer,
					0xff000032,
					x * font->fontWidth + r.x + leftKern, y * font->fontHeight + r.y,
					x * font->fontWidth + r.x + leftKern, y * font->fontHeight + r.y + font->fontHeight - 1);
			sglDrawLine(buffer,
					0x00ff0032,
					x * font->fontWidth + r.x + rightKern, y * font->fontHeight + r.y,
					x * font->fontWidth + r.x + rightKern, y * font->fontHeight + r.y + font->fontHeight - 1);
		}
	}

	sglDrawText(buffer,
			"!@#$%^&*()_-=<>}|}]*,.\n"
			"Hello, this is a demo\n"
			"of sgl. Sgl stands\n"
			"for Simple Graphcs\n"
			"Library\n", 128, 8, font);

	sglDrawText(buffer, "kerning enabled", 128, 120, font);
	sglDrawText(buffer, "kerning disabled", 128, 140, fontWithoutKern);
			
	r = (sglRect){
		cp[0].x, cp[0].y,
		bmp->width,
		bmp->height,
	};

	sglDrawBuffer(buffer, bmp, &r, NULL);
	
	for (int i = 0; i < 4; i++)  {
		drawControlPoint(cp[i], 0x00ff00ff);
	}
}
